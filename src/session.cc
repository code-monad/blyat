#include <blyat/socket.hh>
#include <blyat/session.hh>
#include <blyat/message.hh>
#include <blyat/room.hh>
#include <spdlog/spdlog.h>
#include <entt/entity/helper.hpp>
#include <unordered_set>

namespace blyat {
  session_t::session_t(blyat::server* server, const blyat_id_t id, const tag_t tag) : _server{server} , _room{entt::null},  _id{id}, _tag{tag}, _echo{false} {}
  
  session_t::session_t(const blyat_id_t id, const tag_t tag) : _room{entt::null},  _id{id}, _tag{tag} {}

  std::shared_ptr<blyat::socket> session_t::create_context(boost::asio::ip::tcp::socket&& raw_socket, const std::string& doc_root) {
    _context = std::make_shared<blyat::socket>(std::move(raw_socket), this, doc_root);
    //boost::asio::post(raw_socket.get_executor(), [socket](){ socket->run();});
    _context->run();
    return _context;
  }

  blyat_id_t session_t::set_room(blyat_id_t target) {
    auto tmp = _room;
    _room = target;
    return tmp;
  }

  void session_t::join_room_by_id(const std::uint64_t room_id){
    if(_server) {
      if(_server->has_room_id(room_id) != entt::null) {
	_server->move_session_to_room(_id, room_id);
      }
    }
  }
  
  void session_t::join_room_by_name(const std::string room_name){
    if(_server) {
      bool force_create = true;
      auto& room_get = _server->get_room_by_name(room_name, force_create);
      _server->move_session_to_room(_id, room_get.handler());
      return;
    }
    spdlog::warn("Session {} can't find it's server holder context! This must be a bug!", std::string(_id));
  }

  void session_t::join_default_room() {
    if(_server) {
      _server->move_session_to_room(_id, _server->default_room());
    }
  }
  
  room_t& session_t::room() {
    return _server->get_room_by_id(_room);
  }

  void session_t::set_id(const std::uint64_t room_id) {
    _server->manager().patch<session_t>(room_id);
    _id = room_id;
  }

  void session_t::exit_from_server() {
    if(!_server) return;
    
    if(_server->has_room_id(_room) != entt::null) {
      _server->remove_session_from_room(_id, _room);
    }
    
    if(_server->manager().valid(_id)) {

      _server->remove_session(_id);
    }
    
  }

  
  void session_t::send(std::shared_ptr<message_t> message) {
    if(!_context) return;
    spdlog::info("Send message[{}] to {}", message->id.str(), _id.str());
    _context->send(message);
  }
}
