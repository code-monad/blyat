#ifndef __BLYAT_CORE_STRING_HH
#define __BLYAT_CORE_STRING_HH

#include <string>
#include <string_view>

namespace blyat {
namespace string {

// Defines a range of a string(_view)

template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
using range_t = std::pair<typename std::basic_string<_Char_T, _Traits_T>::iterator,
                          typename std::basic_string<_Char_T, _Traits_T>::iterator>;

using view_range_t = std::pair<std::size_t, std::size_t>;

template <typename _Char_T, class _Traits_T = std::char_traits<_Char_T>>
using crange_t = std::pair<typename std::basic_string<_Char_T, _Traits_T>::const_iterator,
                           typename std::basic_string<_Char_T, _Traits_T>::const_iterator>;

} // namespace string
} // namespace blyat


#endif
