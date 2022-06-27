#ifndef __BLYAT_TAG_HH__
#define __BLYAT_TAG_HH__

#include <string>
namespace blyat {

  struct tag_t {     
    std::string name;
    tag_t() = default;
    tag_t(const std::string& other_name) { name = other_name; }
    tag_t(const tag_t& other) = default;
    bool operator ==(const tag_t& other) const { return name == other.name; }
    operator std::string() const { return name; }
    operator const std::string&() const { return name; }
  };

  extern const tag_t BLYAT_NULL_TAG;
  
} // namespace blyat

#endif
