#include "whz_request_handler.hpp"

#include <fstream>
#include <iostream>

namespace whz {
    request_handler::request_handler(std::filesystem::path document_root)
            : document_root(std::move(document_root)) {}

    auto request_handler::handle_request(const request& req, reply& rep) -> void {
        auto request_path = url_decode(req.uri);

        if (!request_path) {
            rep = reply::stock_reply(reply::bad_request);
            return;
        }

        std::string_view req_path = request_path.value();

        if (req_path.empty() || !req_path.starts_with("/") ||
            req_path.contains("..")) {
            rep = reply::stock_reply(reply::bad_request);
            return;
        }

        if (req_path[req_path.size() - 1] == '/') {
            request_path.value() += "index.html";
        }

        // TODO(bc): Security sanitization is needed
        // Here be dragons

        std::string full_path = std::string{document_root} + request_path.value();
        std::ifstream is(full_path, std::ios::in | std::ios::binary);

        if (!is) {
            rep = reply::stock_reply(reply::not_found);
            return;
        }

        rep.status = reply::ok;
        // TODO(bc): There should be a better way to do this
        char buf[512];
        while (is.read(buf, sizeof(buf)).gcount() > 0)
            rep.content.append(buf, is.gcount());

        rep.headers.resize(2);
        rep.headers[0].name = "Content-Length";
        rep.headers[0].value = std::to_string(rep.content.size());
        rep.headers[1].name = "Content-Type";
        rep.headers[1].value = "text/html"; // TODO(bc): Support other mime_types by
        // reading the extension of request
    }

}; // namespace whz