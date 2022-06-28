#include <blyat/room.hh>
#include <blyat/server.hh>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <spdlog/spdlog.h>

#include <unordered_set>


namespace blyat {
  server::server(boost::asio::io_context& context) : _context{context}, _acceptor{_context}, _doc_root{"./dist"}, _manager{}, _room_mutex{}, _session_mutex{}{}
  
  blyat_id_t server::create_room(const std::string name) {
    std::lock_guard<std::mutex> lock(_room_mutex);
    blyat_id_t entity = _manager.create();
    spdlog::info("Room {}[{}] created", name, entity.str());
    _manager.emplace<room_t>(entity, entity, name, this);
    
    return entity;
  }

  void server:: run(config_fields_t configs) {
    spdlog::info("Start {}:{} with doc_root:{}", configs.service_field.listen_addr, configs.service_field.listen_port, configs.common_field.doc_root);
    _doc_root = configs.common_field.doc_root;
    auto const address = boost::asio::ip::make_address(configs.service_field.listen_addr);
    boost::asio::ip::tcp::endpoint endpoint{address, configs.service_field.listen_port};
    boost::beast::error_code ec;
    _acceptor.open(endpoint.protocol(), ec);
    if(ec) {
      spdlog::error("Failed to open {}:{}, Reason:{}", configs.service_field.listen_addr, configs.service_field.listen_port, ec.message());
      return;
    }
    // Allow address reuse
    _acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if(ec){
      spdlog::error("Failed to set reuse of {}:{}, Reason:{}", configs.service_field.listen_addr, configs.service_field.listen_port, ec.message());
      return;
    }

    _acceptor.bind(endpoint, ec);
    if(ec) {
      spdlog::error("Failed to bind {}:{}, Reason:{}", configs.service_field.listen_addr, configs.service_field.listen_port, ec.message());
      return;
    }
    int max_connections = boost::asio::socket_base::max_listen_connections;
    if(configs.service_field.session_limit != 0) {
      max_connections = configs.service_field.session_limit;
    }
    _acceptor.listen(max_connections, ec);

    if(ec) {
      spdlog::error("Failed to listen {}:{}, Reason:{}", configs.service_field.listen_addr, configs.service_field.listen_port, ec.message());
      return;
    }

    spdlog::info("Start accepting for incomming connections.");
    _default_room = create_room("_");
    do_accept();    
  }

  blyat_id_t server::has_room(const std::string name) {
    for(auto &&[entity, room]: _manager.view<room_t>().each()) {
      if(room.name() == name) return entity;
    }
    return entt::null;
  }

  blyat_id_t server::has_room_id(std::uint64_t id) {
    for(auto &&[entity, room]: _manager.view<room_t>().each()) {
      if(room.handler() == id) return entity;
    }
    return entt::null;
  }

  room_t& server::get_room_by_name(const std::string name, bool create){
    auto room_handler = has_room(name);
    if(room_handler != entt::null) {
      return _manager.get<room_t>(room_handler);
    } else {
      if(create) {
	//std::lock_guard<std::mutex> lock(_room_mutex);
	return _manager.get<room_t>(create_room(name));
      }
      throw std::runtime_error(fmt::format("No such room named {}", name));
    }
  }

  room_t& server::get_room_by_id(blyat_id_t id){
    if(id != entt::null && _manager.valid(id)) {
      return _manager.get<room_t>(id);
    } else {
      throw std::runtime_error(fmt::format("No such room with id {}", id.str()));
    }
  }

  void server::move_session_to_room(blyat_id_t session, blyat_id_t room) {
    //std::lock_guard<std::mutex> lock_session(_session_mutex);
    //std::lock_guard<std::mutex> lock_room(_room_mutex);
    if(session != entt::null && room != entt::null) {
      if(_manager.valid(session) && _manager.valid(room)) {
	auto& session_ctx = _manager.get<session_t>(session);
	//_manager.get<room_t>(room).add(session_ctx);
	_manager.get_or_emplace<std::unordered_set<blyat_id_t>>(room, std::unordered_set<blyat_id_t>()).insert(session);
	session_ctx.set_room(room);
      }
    }
  }

  void server::on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
    if(ec) {
      spdlog::error("Failed to accept a connection, Reason:{}", ec.message());
      return;
    } else {
      spdlog::info("Accepting...");
      std::lock_guard<std::mutex> lock(_session_mutex);
      auto session = _manager.create();
      _manager.emplace<session_t>(session, this, session);
      
      _manager.get<session_t>(session).create_context(std::move(socket), _doc_root);
    }
    _acceptor.async_accept(boost::asio::make_strand(_context), boost::beast::bind_front_handler(&server::on_accept,shared_from_this()));


    
  }

  void server::do_accept() {
    _acceptor.async_accept(boost::asio::make_strand(_context),
			   boost::beast::bind_front_handler(
							    &server::on_accept,
							    shared_from_this()
							    )
			   );
    
  }

}
