#ifndef __BLYAT_URI_HH__
#define __BLYAT_URI_HH__

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/beast/core.hpp>
#include <blyat/core/string.hh>
#include <blyat/core/exception.hh>
#include <blyat/core/symbol.hh>
#include <optional>
#include <string>
#include <string_view>
#include <spdlog/spdlog.h>
namespace blyat {
namespace net {

template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
blyat::string::view_range_t
parse_sequence(std::basic_string_view<_Char_T, _Traits_T>& content, _Char_T letter,
               std::size_t pos = 0, const bool reverse_search = false,
               const bool skip_lead_slash = false, // skip the leading slash
               const bool allow_optional = false,  // letter could be optional
               const bool throw_not_found = false  // throw an exception if not matched
) {
  if (skip_lead_slash) {
    while (content.at(pos) == symbol::basic_slash<_Char_T>()) {
      pos++;
    }
  }
  std::size_t letter_index = 0;
  if (reverse_search)
    letter_index = content.rfind(letter, pos);
  else
    letter_index = content.find(letter, pos);
  if (letter_index != std::basic_string_view<_Char_T, _Traits_T>::npos) {
    std::size_t offset = pos > content.size() ? content.size() : pos;
    return std::make_pair(offset, letter_index);
  }
  if (throw_not_found)
    throw net::invalid_uri(std::string(__PRETTY_FUNCTION__) + " not a valid uri");
  if (allow_optional)
    return std::make_pair(pos, content.size());
  return std::make_pair(std::basic_string_view<_Char_T, _Traits_T>::npos,
                        std::basic_string_view<_Char_T, _Traits_T>::npos);
}

template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>> class basic_uri {
  typedef std::basic_string_view<_Char_T, _Traits_T> string_view_type;
  typedef std::basic_string<_Char_T, _Traits_T> string_type;

public:
  basic_uri() : _size{0}, _data{} {}
  explicit basic_uri(string_view_type uri_data) : _data{uri_data} { this->parse(_data); }
  explicit basic_uri(string_type uri_data) : _data{uri_data.c_str()} { this->parse(_data); }
  explicit basic_uri(const _Char_T* uri_data) : _data{uri_data} { this->parse(_data); }

  constexpr bool empty() noexcept { return _data.empty(); }
  const std::size_t size() noexcept { return _size; }

  basic_uri& parse(string_view_type data) {
    clear();
    _data = data;

    // Perform this only for meanable
    if (!data.empty()) {
      std::size_t npos = string_view_type::npos;
      // scheme
      auto scheme_range = parse_sequence(data, symbol::basic_colon<_Char_T>());
      _scheme = data.substr(scheme_range.first, scheme_range.second);
      auto host_offset = scheme_range.second + 1;
      auto auth_offset = scheme_range.second + 1;
      auto auth_range = parse_sequence(data, symbol::basic_at<_Char_T>(), auth_offset);
      if (auth_range.first != npos) {
        // contains an auth sequence
        _auth = data.substr(auth_range.first, auth_range.second - auth_range.first);
        host_offset = auth_range.second + 1;
      }

      // host
      auto host_range =
          parse_sequence(data, symbol::basic_slash<_Char_T>(), host_offset, false, true, true);
      _host = string_view_type(data.substr(host_range.first, host_range.second - host_range.first));

      auto query_offset = data.size();
      auto target_end = data.size();
      // fragment
      auto frag_range = parse_sequence(data, symbol::basic_hash<_Char_T>(), data.size(), true);
      if (frag_range.second != npos) {
        _fragment = string_view_type(data.substr(frag_range.second + 1));
        query_offset = frag_range.second - 1;
        target_end = frag_range.second;
      }

      auto query_range =
          parse_sequence(data, symbol::basic_question<_Char_T>(), query_offset, true);
      if (query_range.second != npos) {
        _query = string_view_type(data.substr(query_range.second + 1, query_range.first - query_range.second));
        target_end = query_range.second;
      }

      _target = string_view_type(data.substr(host_range.second, target_end - host_range.second));
    }

    return *this;
  }

  // Checkout
  // https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Generic_syntax
  // for details of each identifiers

  string_view_type scheme() const noexcept { return _scheme; }

  string_view_type auth() const noexcept { return _auth; }

  string_view_type host() const noexcept { return _host; }

  string_view_type target() const noexcept { return _target; }

  std::vector<string_view_type> target_seq() const {
    if(_target == "/") {
      return {};
    }
    std::basic_string_view<_Char_T, _Traits_T> target_cmp = _target;
    std::vector<string_view_type> result;
    for (std::size_t pos = 0; pos != string_view_type::npos && pos < _target.size();) {
      auto next =
          parse_sequence(target_cmp, symbol::basic_slash<_Char_T>(), pos, false, true, true);
      pos += 1;
      result.push_back(_target.substr(pos, next.second - pos));
      pos = next.second;
    }

    return result;
  }

  string_view_type query() const noexcept { return _query; }

  string_view_type fragment() const noexcept { return _fragment; }

  string_view_type data() const noexcept { return _data; }

  string_type str() { return string_type{_data.data()}; }

  void clear() {
    _size = 0;
    _data = string_view_type();
    _scheme = string_view_type();
    _host = string_view_type();
    _target = string_view_type();
    _query = string_view_type();
    _fragment = string_view_type();
  }

  virtual ~basic_uri(){};

private:
  std::size_t _size;
  string_view_type _data; // holds the raw uri string
  string_view_type _auth;
  string_view_type _scheme;
  string_view_type _host;
  unsigned _port;
  string_view_type _target;
  string_view_type _query;
  string_view_type _fragment;
};

template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
basic_uri<_Char_T, _Traits_T> parse(const std::basic_string_view<_Char_T, _Traits_T>& uri) {
  return basic_uri(uri);
}

template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
basic_uri<_Char_T, _Traits_T> parse(const std::basic_string<_Char_T, _Traits_T>& uri) {
  return basic_uri(uri);
}

template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
basic_uri<_Char_T, _Traits_T> parse(const _Char_T* uri) {
  return basic_uri(uri);
}

typedef basic_uri<char> uri;
typedef basic_uri<wchar_t> wuri;


} // namespace net
} // namespace blyat

#endif
