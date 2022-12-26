#include <memory>
#include <sys/wait.h>
#include <unistd.h>

#include "server.hpp"
#include "session.hpp"

using boost::asio::ip::tcp;

namespace http_server {

server::server(boost::asio::io_context & io_ctx, uint16_t port)
      : io_ctx_(io_ctx),
        signal_(io_ctx, SIGCHLD),
        acceptor_(io_ctx, {tcp::v4(), port}),
        socket_(io_ctx) {
    wait_for_signal();
    do_accept();
}

void server::wait_for_signal() {
    signal_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/) {
        if (acceptor_.is_open()) {
            while (waitpid(-1, NULL, WNOHANG) > 0) {}
            wait_for_signal();
        }
    });
}

void server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket){
        if (!ec) {
            socket_ = std::move(socket);
            io_ctx_.notify_fork(boost::asio::io_context::fork_prepare);
            if (fork() == 0) {
                io_ctx_.notify_fork(boost::asio::io_context::fork_child);
                acceptor_.close();
                signal_.cancel();
                std::make_shared<session>(std::move(socket_))->start();
            } else {
                io_ctx_.notify_fork(boost::asio::io_context::fork_parent);
                socket_.close();
                do_accept();
            }
        }
    });
}

}
