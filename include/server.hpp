#ifndef NP_SERVER
#define NP_SERVER

#include <cstdint>

#include <boost/asio.hpp>

namespace http_server {

class server {
public:
    explicit server(boost::asio::io_context & io_context, uint16_t port);
private:
    boost::asio::io_context & io_ctx_;
    boost::asio::signal_set signal_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    void wait_for_signal();
    void do_accept();
};
   
}

#endif
