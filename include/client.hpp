#ifndef NP_CLIENT
#define NP_CLIENT

#include <fstream>
#include <string>
#include <sys/types.h>

#include <boost/asio.hpp>

namespace http_client {

class client {
public:
    explicit client(boost::asio::io_context & io_ctx, std::string cwd);

    void connect(size_t session, std::string host, std::string port, std::string fname);
    void complete();
private:
    boost::asio::io_context & io_ctx_;
    boost::asio::signal_set signal_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    
    boost::asio::ip::tcp::resolver::results_type endpoints_;
    boost::asio::streambuf buffer_;
    size_t conns_{0};

    std::string cwd_;
    size_t session_;
    std::ifstream if_;

    void wait_all_conns();
    void do_connect();

    void do_read();
    void do_write();

    void output_command(std::string s);
    void output_shell(std::string s);
};

}

#endif