#ifndef __BLYAT_UUID_HH__
#define __BLYAT_UUID_HH__

#include <functional>
#include <string>
#include <cstdint>

namespace blyat {
  class uuid {
  public:
    uuid();
    uuid(std::uint64_t raw_id);
    uuid(const uuid&) = default;

    operator std::uint64_t() const { return _raw_id; }
    operator std::string() const { return std::to_string(_raw_id); }

  private:
    std::uint64_t _raw_id;
  };
} // namespace blyat


template<>
struct std::hash<blyat::uuid> {
  std::size_t operator()(const blyat::uuid& id) const {
    return std::hash<std::uint64_t>()((std::uint64_t)id);
  }
};

#endif
