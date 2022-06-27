#include <blyat/message.hh>

namespace blyat {
  const std::size_t message_t::size() const {
    if(message_buffer) return message_buffer->size();
    else return 0;
  }
} // blyat
