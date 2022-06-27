#ifndef __BLYAT_SOCKET_HH__
#define __BLYAT_SOCKET_HH__

#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace blyat {
  class message_t;
  // Adjust settings on the stream
  template<class NextLayer>
  void setup_stream(boost::beast::websocket::stream<NextLayer>& ws)
  {
    // These values are tuned for Autobahn|Testsuite, and
    // should also be generally helpful for increased performance.

    boost::beast::websocket::permessage_deflate pmd;
    pmd.client_enable = true;
    pmd.server_enable = true;
    pmd.compLevel = 3;
    ws.set_option(pmd);

    ws.auto_fragment(false);
    
    // Autobahn|Testsuite needs this
    ws.read_message_max(64 * 1024 * 1024);
  }

  class session_t;
  
  class socket : public std::enable_shared_from_this<socket> {
  public:
    explicit socket(boost::asio::ip::tcp::socket&& raw_socket, session_t* session, const std::string& doc_root);

    void run();
    void on_accept(boost::beast::error_code ec);
    boost::beast::websocket::stream<boost::beast::tcp_stream>& stream() { return _ws; }
    session_t* session() { return _session; }
    const std::string doc_root() const { return _doc_root; }
    
    
    void do_read();
    boost::beast::websocket::request_type& req() { return _req; }
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    
    void send(std::shared_ptr<message_t> message);
    void on_send(std::shared_ptr<message_t> message);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);

  private:
    std::string _doc_root;
    boost::beast::websocket::stream<boost::beast::tcp_stream> _ws;
    session_t* _session;
    boost::beast::flat_buffer _buffer;
    boost::beast::websocket::request_type _req;
    std::vector<std::shared_ptr<message_t>> _queue;

  };

}

#endif
