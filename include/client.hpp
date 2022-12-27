#ifndef NP_CLIENT
#define NP_CLIENT

#include <string>
#include <sys/types.h>

#include <boost/asio.hpp>

namespace http_client {

class client {
public:
    explicit client(boost::asio::io_context & io_ctx);

    void connect(std::string host, std::string port);
    void complete();
private:
    boost::asio::io_context & io_ctx_;
    boost::asio::signal_set signal_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    
    boost::asio::ip::tcp::resolver::results_type endpoints_;
    boost::asio::streambuf buffer_;
    size_t conns_{0};

    void wait_all_conns();
    void do_connect();

    void do_read();
    void do_write();
};

}

#endif