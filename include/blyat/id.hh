#ifndef __BLYAT_ID_HH__
#define __BLYAT_ID_HH__

#include <entt/entity/entity.hpp>
#include <blyat/uuid.hh>
namespace blyat {
  struct blyat_id_t {
    using entity_type = std::uint64_t;
    static constexpr auto null = entt::null;

    operator std::string() const { return std::to_string(value); }
    std::string str() const { return std::to_string(value); }
    constexpr operator entity_type() const noexcept { return value; }
    operator bool() const { return value != entt::null; }
    bool operator !=(const blyat_id_t& other) const {return value!=other.raw_value(); }
    bool operator ==(const blyat_id_t& other) const { return value==other.raw_value(); }
    bool operator ==(const entity_type& other) const { return value==other; }
    bool operator !=(const entity_type& other) const { return value!=other; }
    blyat_id_t(blyat::uuid uuid) noexcept : value{uuid} {}
    entity_type raw_value() const noexcept { return value; }
    constexpr blyat_id_t(entity_type val = null) noexcept : value{val} {}
    constexpr blyat_id_t(const blyat_id_t& other) noexcept : value{other.value} {}

  private:
    entity_type value;
  };

  
} // namespace blyat


namespace std {

  template<>
  struct hash<blyat::blyat_id_t>
  {
    std::size_t operator()(const blyat::blyat_id_t& id) const
    {
      return hash<uint64_t>()((uint64_t)id);
    }
  };

}

#endif // __BLYAT_ID_HH__
