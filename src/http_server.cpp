#include <cstdlib>
#include <iostream>

#include "http_server.hpp"

namespace http_server {

void run(uint16_t port) {
    boost::asio::io_context io_ctx;
    server s(io_ctx, port);
    io_ctx.run();
}

}

int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "./http_server <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    try {
        uint16_t port = static_cast<uint16_t>(std::atoi(argv[1]));
        http_server::run(port);
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}