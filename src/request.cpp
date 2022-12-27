#include <sstream>

#include "request.hpp"

#define pass do {} while(0)

using boost::asio::ip::tcp;

namespace http_server {

header::header(tcp::socket & socket) :
        server_addr(socket.local_endpoint().address()),
        server_port(socket.local_endpoint().port()),
        remote_addr(socket.remote_endpoint().address()),
        remote_port(socket.remote_endpoint().port()) {
    pass;
};

request::request(tcp::socket & socket) :
        header(socket) {
    pass;
};

request::state request::parse(std::string & buffer) {
    buffer_ += buffer;
    switch (state_) {
    case PARSING_METHOD:
        parse_method();
        break;
    case PARSING_HEADER:
        parse_header();
        break;
    default:
        state_ = BAD;
    }
    return state_;
}

void request::parse_method() {
    size_t pos = 0;
    
    if ((pos = buffer_.find("\n")) == std::string::npos) {
        return;
    }

    std::string line;

    line = buffer_.substr(0, pos);
    buffer_.erase(0, pos + 1);

    if ((pos = line.find(" ")) == std::string::npos) {
        state_ = BAD;
        return;
    }
    header.request_method = line.substr(0, pos);
    line.erase(0, pos + 1);

    if ((pos = line.find(" ")) == std::string::npos) {
        state_ = BAD;
        return;
    }
    header.request_uri = line.substr(0, pos);
    line.erase(0, pos + 1);

    if ((pos = header.request_uri.find("?")) != std::string::npos) {
        header.query_string = header.request_uri.substr(pos + 1, std::string::npos);
        header.request_uri.erase(pos, std::string::npos);
    }

    if ((pos = line.find("\r")) == std::string::npos) {
        state_ = BAD;
        return;
    }
    header.server_protocol = line.substr(0, pos);
    line.erase(0, pos + 1);

    if (!line.empty()) {
        state_ = BAD;
        return;
    }

    state_ = PARSING_HEADER;
    parse_header();
}

void request::parse_header() {
    size_t pos = 0;

    while ((pos = buffer_.find("\r\n")) != std::string::npos) {
        std::string line = buffer_.substr(0, pos);
        buffer_.erase(0, pos + 2);
        if (line.rfind("Host: ", 0) != 0) {
            continue;
        }
        header.http_host = line.substr(6, std::string::npos);
        state_ = GOOD;
        return;
    }
}

}
