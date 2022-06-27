#ifndef __BLYAT_CORE_HTTP_HH__
#define __BLYAT_CORE_HTTP_HH__

#include <blyat/core/mime.hh>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>

namespace blyat {

  template<class Stream>
  struct send_lambda {
    Stream& stream_;
    bool& close_;
    boost::beast::error_code& ec_;

    explicit
    send_lambda(Stream& stream, bool& close, boost::beast::error_code& ec): stream_(stream), close_(close), ec_(ec){}

    template<bool isRequest, class Body, class Fields>
    void operator()(boost::beast::http::message<isRequest, Body, Fields>&& msg) const
    {
      // Determine if we should close the connection after
      close_ = msg.need_eof();

      // We need the serializer here because the serializer requires
      // a non-const file_body, and the message oriented version of
      // http::write only works with const messages.
      boost::beast::http::serializer<isRequest, Body, Fields> sr{msg};
      boost::beast::http::write(stream_, sr, ec_);
    }
  };
  
  std::string path_cat(boost::beast::string_view base,boost::beast::string_view path) {
    if(base.empty())
      return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if(result.back() == path_separator)
      result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for(auto& c : result)
      if(c == '/')
	c = path_separator;
#else
    char constexpr path_separator = '/';
    if(result.back() == path_separator)
      result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
  }

  
  template<class Body, class Allocator,class Send>
  void handle_request(boost::beast::string_view doc_root, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req, Send&& send)
  {
    // Returns a bad request response
    auto const bad_request =
      [&req](boost::beast::string_view why)
      {
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::bad_request, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
      };

    // Returns a not found response
    auto const not_found =
      [&req](boost::beast::string_view target)
      {
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, req.version()};
        res.set(boost::beast::http::field::server, "IM-A-BLYAT-SERVER-YAY");
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
      };

    // Returns a server error response
    auto const server_error =
      [&req](boost::beast::string_view what)
      {
        boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::internal_server_error, req.version()};
        res.set(boost::beast::http::field::server, "IM-A-BLYAT-SERVER-YAY");
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
      };

    // Make sure we can handle the method
    if( req.method() != boost::beast::http::verb::get &&
        req.method() != boost::beast::http::verb::head)
      return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != boost::beast::string_view::npos)
      return send(bad_request("Illegal request-target"));

    // Build the path to the requested file
    std::string path = path_cat(doc_root, req.target());
    if(req.target().back() == '/')
      path.append("index.html");

    // Attempt to open the file
    boost::beast::error_code ec;
    boost::beast::http::file_body::value_type body;
    body.open(path.c_str(), boost::beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if(ec == boost::beast::errc::no_such_file_or_directory)
      return send(not_found(req.target()));

    // Handle an unknown error
    if(ec)
      return send(server_error(ec.message()));

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if(req.method() == boost::beast::http::verb::head)
      {
        boost::beast::http::response<boost::beast::http::empty_body> res{boost::beast::http::status::ok, req.version()};
        res.set(boost::beast::http::field::server, "IM-A-BLYAT-SERVER-YAY");
        res.set(boost::beast::http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
      }

    // Respond to GET request
    boost::beast::http::response<boost::beast::http::file_body> res{
      std::piecewise_construct,
      std::make_tuple(std::move(body)),
      std::make_tuple(boost::beast::http::status::ok, req.version())};
    res.set(boost::beast::http::field::server, "IM-A-BLYAT-SERVER-YAY");
    res.set(boost::beast::http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
  }
}
#endif
