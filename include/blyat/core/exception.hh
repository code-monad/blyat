#ifndef __BLYAT_CORE_EXCEPTION_HH
#define __BLYAT_CORE_EXCEPTION_HH
#include <stdexcept>
#include <string_view>
namespace blyat {
namespace net {
class exception : public std::exception {
public:
  exception();
  virtual const char* what() const noexcept = 0;
  virtual ~exception() = default;
};

class invalid_uri : public blyat::net::exception, public std::invalid_argument {
public:
  explicit invalid_uri(const std::string& what_arg);
  explicit invalid_uri(const char* what_arg);
  const char* what() const noexcept;
  ~invalid_uri() = default;

private:
  std::string_view _what;
};

} // namespace net
} // namespace blyat

#endif
