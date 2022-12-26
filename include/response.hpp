#ifndef NP_RESPONSE
#define NP_RESPONSE

#include <string>

namespace http_server {

class response {
public:
    explicit response(int status);

    void add_header(std::string hdr);
    const std::string & header();
private:
    int status_;
    std::string buffer_;
};

}

#endif