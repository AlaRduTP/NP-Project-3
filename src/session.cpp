#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "response.hpp"
#include "session.hpp"

#define pass do {} while(0)

using boost::asio::ip::tcp;

namespace http_server {

session::session(tcp::socket socket) :
        socket_(std::move(socket)), request_(socket_) {
    pass;
}

void session::start() {
    do_read();
}

void session::dispatch() {
    header & hdr = request_.header;
    if (hdr.request_method != "GET") {
        return write_header(405);
    }
    if (hdr.request_uri.length() < 4) {
        return write_header(404);
    }
    if (hdr.request_uri.compare(hdr.request_uri.length() - 4, 4, ".cgi")) {
        return write_header(404);
    }
    if (hdr.server_protocol != "HTTP/1.1") {
        return write_header(505);
    }
    work();
}

void session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::string s(buffer_.begin(), buffer_.begin() + length);
                switch (request_.parse(s)) {
                case request::GOOD:
                    dispatch();
                    break;
                case request::BAD:
                    break;
                default:
                    do_read();
                }
            }
        });
}

void session::do_write(const char * buf, std::size_t len) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(buf, len),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                pass;
            }
        });
}

void session::write_chunk(const char * buf, std::size_t buf_size) {
    char chunk[2048];
    size_t chunk_size = snprintf(chunk, sizeof(chunk), "%lX\r\n%s\r\n", buf_size, buf);
    do_write(chunk, chunk_size);
}

void session::write_header(int http_status) {
    response resp(http_status);
    resp.add_header("");
    do_write(resp.header().c_str(), resp.header().length());
    write_chunk("\0", 0);
}

void session::set_envs() {
    auto & hdr = request_.header;
    setenv("REQUEST_METHOD", hdr.request_method.c_str(), 1);
    setenv("REQUEST_URI", hdr.request_uri.c_str(), 1);
    setenv("QUERY_STRING", hdr.query_string.c_str(), 1);
    setenv("SERVER_PROTOCOL", hdr.server_protocol.c_str(), 1);
    setenv("HTTP_HOST", hdr.http_host.c_str(), 1);
    setenv("SERVER_ADDR", hdr.server_addr.to_string().c_str(), 1);
    setenv("SERVER_PORT", std::to_string(hdr.server_port).c_str(), 1);
    setenv("REMOTE_ADDR", hdr.remote_addr.to_string().c_str(), 1);
    setenv("REMOTE_PORT", std::to_string(hdr.remote_port).c_str(), 1);
}

void session::work() {
    int output[2];
    if (pipe(output) == -1) {
        return;
    }

    pid_t pid;
    if ((pid = fork()) == -1) {
        return;
    }

    if (pid == 0) {
        socket_.close();

        close(STDOUT_FILENO);
        dup(output[1]);
        close(output[1]);

        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        std::string path(cwd);
        path += request_.header.request_uri;
        char * argv[] = {NULL};

        set_envs();
        if (execvp(path.c_str(), argv) == -1) {
            exit(EXIT_FAILURE);
        }
    }

    close(output[1]);

    bool parsing_header = true;
    response resp(200);
    std::string s;

    char buf[1024];
    ssize_t len;

    while ((len = read(output[0], buf, sizeof(buf) - 1)) > 0) {
        buf[len] = '\0';
        if (parsing_header) {
            s += std::string(buf);
            size_t pos = 0;
            while ((pos = s.find('\n')) != std::string::npos) {
                std::string line = s.substr(0, pos);
                if (pos && s[pos - 1] == '\r') {
                    line.erase(pos - 1, std::string::npos);
                }
                s.erase(0, pos + 1);
                resp.add_header(line);
                if (line.empty()) {
                    do_write(resp.header().c_str(), resp.header().length());
                    parsing_header = false;
                    if (!s.empty()) {
                        write_chunk(s.c_str(), s.length());
                    }
                    break;
                }
            }
            continue;
        }
        write_chunk(buf, len);
    }
    buf[0] = '\0';
    write_chunk(buf, 0);

    close(output[0]);
    wait(NULL);
}

}
