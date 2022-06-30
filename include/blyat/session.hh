#ifndef __BLYAT_SESSION_HH__
#define __BLYAT_SESSION_HH__

#include <blyat/id.hh>
#include <blyat/tag.hh>
#include <memory>
#include <boost/asio/ip/tcp.hpp>

namespace blyat {

  class message_t;
  class room_t;
  class server;
  class socket;
  class session_t {
  public:
    session_t(blyat::server* server, const blyat_id_t id = blyat::uuid(), const tag_t tag = BLYAT_NULL_TAG);
    session_t(const blyat_id_t id = blyat::uuid(), const tag_t tag = BLYAT_NULL_TAG);
    session_t(const session_t& other) : _server{ other._server}, _echo{other._echo}, _room{other._room}, _context{other._context}, _id{other._id}, _tag{other._tag} {}
    std::shared_ptr<blyat::socket> create_context(boost::asio::ip::tcp::socket&& socket, const std::string& doc_root);
    void join_default_room();
    void join_room_by_id(const std::uint64_t room_id);
    void join_room_by_name(const std::string room_name);
    void exit_from_server();
    blyat_id_t set_room(blyat_id_t target); // return previous
    void set_id(const std::uint64_t room_id);
    blyat_id_t room_ctx() { return _room; }
    room_t& room();
    void send(std::shared_ptr<message_t> message);
    const blyat_id_t& id() const { return _id; }
    const blyat_id_t& server_id() const { return _id; }
    void set_echo(bool set_to) { _echo = set_to; }
    bool echo() const { return _echo; }
    void release_socket() { _context.reset(); }
  private:
    bool _echo;
    server* _server;
    blyat_id_t _room;
    std::shared_ptr<blyat::socket> _context;
    blyat_id_t _id;
    tag_t _tag;
  }; // session

} // namespace blyat

#endif
