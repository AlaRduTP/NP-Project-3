#ifndef NPS_REQUEST
#define NPS_REQUEST

#include <string>

#include <boost/asio.hpp>

namespace http_server {

struct header {
    explicit header(boost::asio::ip::tcp::socket & socket);

    std::string request_method;
    std::string request_uri;
    std::string query_string;
    std::string server_protocol;
    std::string http_host;
    boost::asio::ip::address server_addr;
    int server_port;
    boost::asio::ip::address remote_addr;
    int remote_port;
};

class request {
public:
    explicit request(boost::asio::ip::tcp::socket & socket);

    enum state {
        PARSING_METHOD,
        PARSING_HEADER,
        GOOD,
        BAD
    };
    struct header header;

    state parse(std::string & buffer);
private:
    state state_{PARSING_METHOD};
    std::string buffer_;

    void parse_method();
    void parse_header();
};

}

#endif