#include "Api.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    int port = 8080;
    if (argc > 1) {
        try { port = std::stoi(argv[1]); } catch (...) {}
    }
    httplib::Server svr;
    svr.set_keep_alive_max_count(100);
    Api api;
    api.registerRoutes(svr);

    svr.set_logger([](const auto& req, const auto& res) {
        (void)req; (void)res;
    });

    std::cout << "Strawcha Bank API running on http://localhost:" << port << std::endl;
    svr.listen("0.0.0.0", port);
    return 0;
}