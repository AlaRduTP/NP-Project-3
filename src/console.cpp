#include <cstdlib>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "client.hpp"
#include "console.hpp"

namespace http_client {

console::console() {
    parse_query_string();
}

const std::array<std::array<std::string, 3>, 5> & console::queries() {
    return qs_;
}

void console::parse_query_string() {
    std::istringstream qs(getenv("QUERY_STRING"));
    std::string env;
    while (std::getline(qs, env, '&')) {
        if (env.length() < 3) {
            continue;
        }
        std::istringstream is(env);
        char k, n, d;
        is >> k >> n >> d;
        if (d != '=') {
            continue;
        }
        switch (k) {
        case 'h':
            k = 0;
            break;
        case 'p':
            k = 1;
            break;
        case 'f':
            k = 2;
            break;
        default:
            continue;
        }
        n -= '0';
        if (n < 0 || n > 4) {
            continue;
        }
        is >> qs_[n][k];
    }
}

}

int main() {
    std::cout << "Content-type: text/html\r\n\r\n" << http_client::HTML << std::flush;

    http_client::console console;

    boost::asio::io_context io_ctx;
    http_client::client c(io_ctx, "test_case");

    for (size_t i = 0; i < 5; ++i) {
        auto & query = console.queries()[i];
        if (query[0].empty()) {
            continue;
        }
        std::cout
            << "<script>document.getElementById('tabs').innerHTML += '<button class=\"tablinks\" onclick=\"openTab(event, \\'t"
            << i << "\\')\">s" << i << "</button>';</script>\n"
            << "<script>document.getElementById('tabcontents').innerHTML += '<div id=\"t" << i
            << "\" class=\"tabcontent\"><h3>" << query[0] << ':' << query[1] << "</h3><pre id=\"s" << i
            << "\"></pre></div>';</script>\n" << std::flush;
        c.connect(i, query[0], query[1], query[2]);
    }

    c.complete();
}
