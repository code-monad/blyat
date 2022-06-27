#include <blyat/core/exception.hh>
#include <sstream>
#include <stdexcept>

blyat::net::exception::exception() {}

blyat::net::invalid_uri::invalid_uri(const std::string& what_arg)
    : std::invalid_argument(what_arg) {}

blyat::net::invalid_uri::invalid_uri(const char* what_arg) : std::invalid_argument(what_arg) {}

const char* blyat::net::invalid_uri::what() const noexcept {
  std::ostringstream ss;
  ss << _what << " is not a valid uri";
  return std::move(ss.str().c_str());
}
