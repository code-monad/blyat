#ifndef __BLYAT_ROOM_HH__
#define __BLYAT_ROOM_HH__

#include <blyat/id.hh>
#include <blyat/message.hh>
#include <blyat/session.hh>
#include <blyat/server.hh>

#include <entt/entity/registry.hpp>

namespace blyat {
  class message_t;
  class server;
  class room_t {
  public:
    room_t(blyat_id_t handler, const std::string name = " BLYAT_ROOM", blyat::server* server = nullptr);
    void broadcast(std::shared_ptr<message_t> message);
    void broadcast(const message_t&& message);
    blyat_id_t handler() { return _handler; }
    blyat_id_t add(session_t& session);

    std::string name() const { return _name; }
    
  private:
    blyat_id_t _handler;
    blyat::server* _server;
    std::string _name;
  }; // room_t
} // namespace blyat
#endif // __BLYAT_ROOM_HH__
