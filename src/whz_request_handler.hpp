#ifndef WHZ_REQUEST_HANDLER_H_
#define WHZ_REQUEST_HANDLER_H_

#include <filesystem>
#include "whz_common.hpp"
#include "whz_utils.hpp"

namespace whz {
class request_handler {
 public:
  explicit request_handler(std::filesystem::path document_root);

  auto handle_request(const whz::request& req, whz::reply& rep) -> void;

 private:
  std::filesystem::path document_root;
};
}; // namespace whz

#endif // WHZ_REQUEST_HANDLER_H_
