#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "client.hpp"

int main() {
    std::cout << "Content-type: text/plain\r\n\r\n" << std::flush;

    boost::asio::io_context io_ctx;
    http_client::client c(io_ctx);

    c.connect("localhost", "8787");
    c.connect("localhost", "8787");
    c.connect("localhost", "8787");
    c.complete();
}
