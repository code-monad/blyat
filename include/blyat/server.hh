#ifndef __BLYAT_SERVER_HH__
#define __BLYAT_SERVER_HH__

#include <blyat/id.hh>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>

#include <entt/entity/registry.hpp>
#include <blyat/config.hh>

#include <map>
#include <mutex>

namespace blyat {
  class room_t;
  
  class server : public std::enable_shared_from_this<server>{
  public:
    server(boost::asio::io_context& context);
    blyat_id_t create_room(const std::string name = "ROOM");
    room_t& get_room_by_name(const std::string name, bool create = false);
    room_t& get_room_by_id(blyat_id_t id);
    entt::basic_registry<blyat_id_t>& manager() { return _manager; }
    blyat_id_t has_room(const std::string name);
    blyat_id_t has_room_id(std::uint64_t id);
    void run(config_fields_t configs);
    void move_session_to_room(blyat_id_t session, blyat_id_t room);
    blyat_id_t default_room() { return _default_room; };

  private:
    void do_accept();
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
    blyat_id_t _default_room;
    boost::asio::io_context& _context;
    boost::asio::ip::tcp::acceptor _acceptor;
    std::string _doc_root;
    entt::basic_registry<blyat_id_t> _manager;
    std::mutex _room_mutex;
    std::mutex _session_mutex;
  };
}

#endif
