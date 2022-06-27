
#include <blyat/id.hh>
#include <blyat/room.hh>
#include <blyat/message.hh>
#include <entt/entity/entity.hpp>
#include <unordered_set>
namespace blyat {

  room_t::room_t(blyat_id_t handler,const std::string name, blyat::server* server) : _handler{handler}, _server{server},_name{name} {}

  void room_t::broadcast(const message_t&& message){
    auto message_holder = std::make_shared<message_t>(std::move(message));
    broadcast(message_holder);
  }
  
  void room_t::broadcast(std::shared_ptr<message_t> message){
    if(!_server) return;
    auto& sessions = _server->manager().get_or_emplace<std::unordered_set<blyat_id_t>>(_handler, std::unordered_set<blyat_id_t>());
    for(const auto session : sessions) {
      auto& session_ctx = _server->manager().get<session_t>(session);
      if(session_ctx.id() != message->source_session
	 || session_ctx.echo()
	 ) session_ctx.send(message);
    }
  }


  blyat_id_t room_t::add(session_t& session) {
    if(!_server) {
      return entt::null;
    }
    auto session_id = _server->manager().create();
    _server->manager().emplace<session_t>(session_id, session);
    return session_id;
  }

  
}
