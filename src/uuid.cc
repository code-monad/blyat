#include <blyat/uuid.hh>

#include <random>


namespace blyat {
  static std::random_device RANDOM_DEVICE;
  static std::mt19937_64 RANDOM_ENGINE(RANDOM_DEVICE());
  static std::uniform_int_distribution<uint64_t> UNIFORM_DISTRIBUTION;

  uuid::uuid() : _raw_id(UNIFORM_DISTRIBUTION(RANDOM_ENGINE)) { }
  uuid::uuid(std::uint64_t raw_id) : _raw_id {raw_id} {}
  
}
