#include <blyat/socket.hh>
#include <blyat/message.hh>
#include <blyat/session.hh>
#include <blyat/room.hh>
#include <blyat/core/uri.hh>
#include <blyat/core/http.hh>
#include <string>
#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string.hpp>

namespace blyat {
  
  socket::socket(boost::asio::ip::tcp::socket&& raw_socket, session_t* session, const std::string& doc_root) : _ws{std::move(raw_socket)}, _session{session}, _doc_root{doc_root} {}

  void socket::run() {
    _ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
    boost::beast::websocket::permessage_deflate pmd;
    pmd.client_enable = true;
    pmd.server_enable = true;
    pmd.compLevel = 3;
    _ws.set_option(pmd);
    _ws.auto_fragment(false);

    // Autobahn|Testsuite needs this
    _ws.read_message_max(100 * 1024 * 1024);
    
    _ws.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type& res){
      res.set(boost::beast::http::field::server, "IM-A-BLYAT-SERVER-YAY");
    }));

    boost::beast::http::async_read(_ws.next_layer(), _buffer, _req, [self=shared_from_this()](boost::beast::error_code ec, std::size_t) {
      std::string session_id = "UNKNONW_SESSION";
      if(self->session()) session_id = self->session()->id().str();
      if(ec) {
	spdlog::error("Error performing HTTP read on session {}", session_id);
	return;
      }
      if(boost::beast::websocket::is_upgrade(self->req())) {
	spdlog::info("Upgrade requested, try accept it... ");
	if(self->session()) {
	  std::string url(self->req()[boost::beast::http::field::host]);
	  std::string protocol(self->req()[boost::beast::http::field::protocol]);
	  url = std::string("http://") + url +  std::string(self->req().target());
	  auto uri = blyat::net::parse(url);
	  std::string target_str{uri.target().begin(),uri.target().end()};
	  std::string query = {uri.query().begin(),uri.query().end()};
	  std::vector<std::string> target_seq{};
	  boost::trim_if(target_str, boost::is_any_of("/"));
	  boost::split(target_seq, target_str, boost::is_any_of("/"));
	  //spdlog::info("target is :{}, query is :{}", target, query);
	  //spdlog::info("scheme is:{}, target is :{}, query is :{}", uri.scheme(), uri.target(), uri.query());
	  
	  std::vector<std::string> query_lists{};
	  boost::split(query_lists, query, boost::is_any_of("&"));
	  std::string session_name;
	  std::uint64_t session_id;

	  std::map<std::string, std::string> query_sets{};
	  
	  for(int i = 0; i < query_lists.size(); i++) {
	    std::vector<std::string> query_elem{};
	    boost::split(query_elem, query_lists[i], boost::is_any_of("="));
	    if(query_elem.size() > 1) {
	      query_sets[query_elem[0]] = query_elem[1];
	    }
	  }

	  

	  for(auto& [query, value]: query_sets) {
	    // if setting echo
	    if(query == "echo") {
	      auto FORMED_ECHO_VALUE = boost::algorithm::to_upper_copy(value);
	      if(FORMED_ECHO_VALUE == "YES" || FORMED_ECHO_VALUE == "TRUE") {
		spdlog::info("Session wants to get echo message, perform it.");
		if(self->session()) self->session()->set_echo(true);
	      }
	    }

	    // if setting session id
	    if(query == "id") {
	      try {
		session_id = std::stoll(value);
		//self->session()->set_id(session_id);
	      } catch(std::exception& ex) {
		spdlog::error("Ill-formed Session-ID {}, skipped.", value);
	      }
	    }
	    
	    if(query == "name") {
	      session_name = value;
	    }
	  }
	  
	  if(target_seq.empty() ) { // join default room
	    self->session()->join_default_room();
	    spdlog::info("Session join default room.");
	  } else if(target_seq.size() == 1) { // join by name
	    self->session()->join_room_by_name(target_seq.front());
	  } else if(target_seq.size() > 1) {
	    if(target_seq.front() == "room") { // by name
	      std::string room_name(target_seq[1]);
	      spdlog::info("Session {} try join room {}",session_id, room_name);

	      try{
		self->session()->join_room_by_name(room_name);
	      } catch(std::exception& e) {
		spdlog::error("Session {} failed to join room {}, reason: {}", session_id, room_name, e.what());
	      }
	    }
	    if(target_seq.front() == "room_id") { // by id
	      try {
		std::uint64_t room_id = std::stoll(std::string(target_seq[1]));
		spdlog::info("Session {} try join room_id {}", session_id, room_id);
		self->session()->join_room_by_id(room_id);
		} catch(std::exception& ex) {
		  spdlog::error("Ill-formed Room-ID {}, skipped.", target_seq[1]);
		}
	      //self->session()->join_room_by_id(room_id);
	    }
	  }
	  
	}
	//self->stream().accept(self->req(), ec);
	
	self->stream().set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
	self->stream().binary(true);
	self->stream().async_accept(self->req(), boost::beast::bind_front_handler(&socket::on_accept, self));
      } else { // a normal http request
	spdlog::info("Normal http request, try handle it.");
	bool close = false;
	send_lambda<boost::asio::ip::tcp::socket> lambda{self->stream().next_layer().socket(), close, ec};
	boost::beast::http::request<boost::beast::http::string_body> req{self->req()};
	handle_request(self->doc_root(), std::move(req), lambda);
      }
    });

  }

  void socket::on_accept(boost::beast::error_code ec){
    if(ec) {
      spdlog::error("Failed to accept:{}", ec.message());
    } else {
      spdlog::info("Session {} upgradred", _session?_session->id().str():"UNKNONW_SESSION");
      do_read();
    }
  }

  void socket::do_read() {
    _ws.async_read(_buffer,boost::beast::bind_front_handler(&socket::on_read,shared_from_this()));
  }

  void socket::on_read(boost::beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if(ec == boost::beast::websocket::error::closed) {
      spdlog::info("Session {} is going to close.", _session?_session->id().str():"UNKNONW_SESSION");
      if(_session) {
	_session->exit_from_server();
      }
      return;
    }

    if(ec) {
      spdlog::error("Session {} got an error on read:{}", _session?_session->id().str():"UNKNONW_SESSION", ec.message());
      // clear if buffer remains some message
      _buffer.consume(_buffer.size());
      if(_ws.is_open()) return do_read();
      else {
	if(_session) return _session->exit_from_server();
      }
    }

    _ws.text(_ws.got_text());
    //_ws.binary(!_ws.got_text());
    blyat::message_t message;
    if(_session) {
      try {
	message.source_session = _session->id();
	//message.message_buffer = _buffer.data();
	//_session->received();
	message.message_buffer = std::make_shared<std::string>(boost::beast::buffers_to_string(_buffer.data()));
	_buffer.consume(_buffer.size());
	message.binary = _ws.got_text();
	if(_ws.got_text()) spdlog::info("Got message from Session[{}] : {}", std::string(message.source_session), *message.message_buffer);
	else spdlog::info("Got message from Session[{}] : {} bytes", std::string(message.source_session), message.message_buffer->size());
	_session->room().broadcast(std::move(message));
      } catch(std::exception& e) {
	spdlog::error("Failed to broadcast {}'s message, Reason:{}", _session->id().str(), e.what());
      }
    }

    do_read();
  }

  void socket::on_send(std::shared_ptr<message_t> message) {
      _queue.push_back(message);

      if(_queue.size() > 1) {
	return; // already writing
      }
      _ws.async_write(
		      boost::asio::buffer(*_queue.front()->message_buffer),
		      boost::beast::bind_front_handler(&socket::on_write,shared_from_this()));
      
  }
  
  
  void socket::send(std::shared_ptr<message_t> message) {
    boost::asio::post(_ws.get_executor(),boost::beast::bind_front_handler(&socket::on_send,shared_from_this(),message));
  }

  void socket::on_write(boost::beast::error_code ec, std::size_t bytes_transferred) {
      if(ec) {
	spdlog::error("Session {} failed writing socket:{}", _session?_session->id().str():"UNKNONW_SESSION", ec.message());
      }

      if(!_ws.is_open()) { // session closed, just clear buffer and close all context
	_queue.clear();
	if(_session) return _session->exit_from_server();
	return;
      }

      _queue.erase(_queue.begin());

      if(!_queue.empty()) { // keep writing waited queue
	if(_queue.front()->binary) _ws.binary(true);
	_ws.async_write(
			boost::asio::buffer(*_queue.front()->message_buffer),
			boost::beast::bind_front_handler(&socket::on_write,shared_from_this()));
      }
      
    }


}
