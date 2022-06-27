#ifndef __BLYAT_CORE_SYMBOL_HH
#define __BLYAT_CORE_SYMBOL_HH


namespace blyat {
  namespace symbol {
    // Below defines a set of symbol chars and their alias, mostly used in
    // the URI parsing, and one shall never use using-declaration to
    // include this namespace (due to the hash sign)

    // The colon symblo, for template
    template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
    constexpr const _Char_T basic_colon() noexcept {}

    template <> constexpr const char basic_colon<char>() noexcept { return ':'; }

    template <> constexpr const wchar_t basic_colon<wchar_t>() noexcept { return L':'; }

    // covinience alias
    constexpr const char colon = basic_colon<char>();
    constexpr const wchar_t wcolon = basic_colon<wchar_t>();

    // The @ symblo, for template
    template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
    constexpr const _Char_T basic_at() noexcept {}

    template <> constexpr const char basic_at<char>() noexcept { return '@'; }

    template <> constexpr const wchar_t basic_at<wchar_t>() noexcept { return L'@'; }

    // covinience alias
    constexpr const char at = basic_at<char>();
    constexpr const wchar_t wat = basic_at<wchar_t>();

    // The slash symblo, for template
    template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
    constexpr const _Char_T basic_slash() noexcept {}
    template <> constexpr const char basic_slash<char>() noexcept { return '/'; }
    template <> constexpr const wchar_t basic_slash<wchar_t>() noexcept { return L'/'; }

    // covinience alias
    constexpr const char slash = basic_slash<char>();
    constexpr const wchar_t wslash = basic_slash<wchar_t>();

    // The question symblo, for template
    template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
    constexpr const _Char_T basic_question() noexcept {}
    template <> constexpr const char basic_question<char>() noexcept { return '?'; }
    template <> constexpr const wchar_t basic_question<wchar_t>() noexcept { return L'?'; }

    // covinience alias
    constexpr const char question = basic_question<char>();
    constexpr const wchar_t wquestion = basic_question<wchar_t>();

    
    // The and symblo, for template
    template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
    constexpr const _Char_T basic_ampersand() noexcept {}
    template <> constexpr const char basic_ampersand<char>() noexcept { return '&'; }
    template <> constexpr const wchar_t basic_ampersand<wchar_t>() noexcept { return L'&'; }
    
    // covinience alias
    constexpr const char ampersand = basic_ampersand<char>();
    constexpr const wchar_t wampersand = basic_ampersand<wchar_t>();

    // The hash symblo, for template
    template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
    constexpr const _Char_T basic_hash() noexcept {}
    template <> constexpr const char basic_hash<char>() noexcept { return '#'; }
    template <> constexpr const wchar_t basic_hash<wchar_t>() noexcept { return L'#'; }

    // covinience alias
    // WARN: Don't be confused between it and a hash function
    constexpr const char hash = basic_hash<char>();
    constexpr const wchar_t whash = basic_hash<wchar_t>();
  }
} // namespace blyat

#endif
