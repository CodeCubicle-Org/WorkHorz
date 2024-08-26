#ifndef WHZ_REQUEST_H_
#define WHZ_REQUEST_H_

#include <string>

namespace whz {

class request {
 public:
  request();
  ~request(); // TODO: Evaluate

 private:
  std::string m_method;
  std::string m_uri; // TODO: This can be different
  std::size_t m_header_buffer_size;
};

}; // namespace whz

#endif // WHZ_REQUEST_H_
