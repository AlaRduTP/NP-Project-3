#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#include "client.hpp"

using boost::asio::ip::tcp;

namespace http_client {

client::client(boost::asio::io_context & io_ctx)
      : io_ctx_(io_ctx), signal_(io_ctx, SIGCHLD),
        socket_(io_ctx), resolver_(io_ctx) {
}

void client::connect(std::string host, std::string port) {
    endpoints_ = resolver_.resolve(host, port);
    ++conns_;
    do_connect();
}

void client::complete() {
    wait_all_conns();
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
                io_ctx_.notify_fork(boost::asio::io_context::fork_prepare);
                if (fork() == 0) {
                    io_ctx_.notify_fork(boost::asio::io_context::fork_child);
                    do_read();
                } else {
                    io_ctx_.notify_fork(boost::asio::io_context::fork_parent);
                    socket_.close();
                }
            } else {
                --conns_;
                exit(EXIT_SUCCESS);
            }
        });
}

void client::do_read() {
    boost::asio::async_read_until(socket_, buffer_, "% ",
        [this](const boost::system::error_code & ec, size_t size) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string buf(size, '\0');
                is.read(&buf[0], size);
                std::cout << buf << std::flush;
                do_write();
            } else {
                exit(EXIT_SUCCESS);
            }
        });
}

void client::do_write() {
    std::cout << "exit\n" << std::flush;
    boost::asio::async_write(socket_, boost::asio::buffer("exit\n", 5),
        [this](boost::system::error_code ec, size_t /*length*/) {
            if (!ec) {
                do_read();
            } else {
                exit(EXIT_SUCCESS);
            }
        });
}

}
