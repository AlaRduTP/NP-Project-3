#include <map>

#include "response.hpp"

std::map<int, std::string> STATUS_INFO = {
    {200, "OK"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {505, "HTTP Version Not Supported"}
};

namespace http_server {

response::response(int status) : status_(status) {
    add_header("HTTP/1.1 " + std::to_string(status_) + " " + STATUS_INFO[status]);
    add_header("Transfer-Encoding: chunked");
}

void response::add_header(std::string hdr) {
    buffer_ += hdr + "\r\n";
}

const std::string & response::header() {
    return buffer_;
}

}
