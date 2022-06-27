#ifndef __BLYAT_MESAGE_HH__
#define __BLYAT_MESAGE_HH__

#include <blyat/id.hh>

#include <string>
#include <memory>
#include <chrono>
namespace blyat {
  typedef std::string buffer_type;

  struct message_t {
    blyat_id_t id = uuid();
    blyat_id_t source_session;
    bool binary = false;
    std::shared_ptr<buffer_type> message_buffer;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;

    const std::size_t size() const;
  };

} // namespace blyat

#endif
