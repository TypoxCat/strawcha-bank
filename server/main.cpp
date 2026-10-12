#include "Api.h"
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char** argv) {
    int port = 8080;
    if (const char* env = std::getenv("PORT")) {
        try { port = std::stoi(env); } catch (...) {}
    }
    if (argc > 1) {
        try { port = std::stoi(argv[1]); } catch (...) {}
    }

    httplib::Server svr;
    svr.set_keep_alive_max_count(100);

    // CORS: allow the static frontend (GitHub Pages) to call this API.
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization, X-Staff-Key"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"}
    });
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        if (req.method == "OPTIONS") {
            res.status = 204;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    Api api;
    api.registerRoutes(svr);

    svr.set_logger([](const auto& req, const auto& res) {
        (void)req; (void)res;
    });

    std::cout << "Strawcha Bank API running on http://0.0.0.0:" << port << std::endl;
    svr.listen("0.0.0.0", port);
    return 0;
}
