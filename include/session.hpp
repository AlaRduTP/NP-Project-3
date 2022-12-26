#ifndef NP_SESSION
#define NP_SESSION

#include <boost/asio.hpp>

#include "request.hpp"

namespace http_server {

class session : public std::enable_shared_from_this<session> {
public:
    explicit session(boost::asio::ip::tcp::socket socket);

    void start();
private:
    boost::asio::ip::tcp::socket socket_;
    std::array<char, 1024> buffer_;
    request request_;

    void dispatch();

    void do_read();
    void do_write(const char * buf, std::size_t len);

    void write_header(int http_status);
    void write_chunk(const char * buf, std::size_t buf_size);
    void set_envs();
    void work();
};
   
}

#endif
