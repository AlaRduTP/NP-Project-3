#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#include "client.hpp"

using boost::asio::ip::tcp;

static void html_encode(std::string & s) {
    std::string buffer;
    buffer.reserve(s.size());
    for(size_t pos = 0; pos != s.size(); ++pos) {
        switch(s[pos]) {
        case '&':
            buffer.append("&amp;");
            break;
        case '\"':
            buffer.append("&quot;");
            break;
        case '\'':
            buffer.append("&apos;");
            break;
        case '<':
            buffer.append("&lt;");
            break;
        case '>':
            buffer.append("&gt;");
            break;
        case '\n':
            buffer.append("&NewLine;");
            break;
        case '\r':
            break;
        default:
            buffer.append(&s[pos], 1);
            break;
        }
    }
    s.swap(buffer);
}

namespace http_client {

client::client(boost::asio::io_context & io_ctx, std::string cwd)
      : io_ctx_(io_ctx), signal_(io_ctx, SIGCHLD),
        socket_(io_ctx), resolver_(io_ctx), cwd_(cwd) {
}

void client::connect(size_t session, std::string host, std::string port, std::string fname) {
    io_ctx_.notify_fork(boost::asio::io_context::fork_prepare);
    if (fork() == 0) {
        io_ctx_.notify_fork(boost::asio::io_context::fork_child);
        session_ = session;
        if_.open(cwd_ + '/' + fname);
        endpoints_ = resolver_.resolve(host, port);
        do_connect();
    } else {
        io_ctx_.notify_fork(boost::asio::io_context::fork_parent);
        ++conns_;
    }
}

void client::complete() {
    if (conns_) {
        wait_all_conns();
    }
    io_ctx_.run();
}

void client::wait_all_conns() {
    signal_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/) {
        int status = 0;
        while (waitpid(-1, &status, WNOHANG) > 0) {
            --conns_;
        }
        if (conns_) {
            wait_all_conns();
        }
    });
}

void client::do_connect() {
    boost::asio::async_connect(socket_, endpoints_,
        [this](boost::system::error_code ec, tcp::endpoint /*endpoints*/) {
            if (!ec) {
                do_read();
            } else {
                exit(EXIT_SUCCESS);
            }
        });
    io_ctx_.run();
}

void client::do_read() {
    boost::asio::async_read_until(socket_, buffer_, "% ",
        [this](const boost::system::error_code & ec, size_t size) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string buf(size, '\0');
                is.read(&buf[0], size);
                output_shell(buf);
                do_write();
            } else {
                exit(EXIT_SUCCESS);
            }
        });
}

void client::do_write() {
    if (!if_) {
        exit(EXIT_SUCCESS);
    }
    std::string line;
    std::getline(if_, line);
    line += '\n';
    output_command(line);
    boost::asio::async_write(socket_, boost::asio::buffer(line),
        [this](boost::system::error_code ec, size_t /*length*/) {
            if (!ec) {
                do_read();
            } else {
                exit(EXIT_SUCCESS);
            }
        });
}

void client::output_command(std::string s) {
    html_encode(s);
    std::cout
        << "<script>document.getElementById('s" << session_
        << "').innerHTML += '<b>" << s << "</b>';</script>\n" << std::flush;
}

void client::output_shell(std::string s) {
    html_encode(s);
    std::cout
        << "<script>document.getElementById('s" << session_
        << "').innerHTML += '" << s << "';</script>\n" << std::flush;
}

}
