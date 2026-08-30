#include <boost/config.hpp>
#if !defined(BOOST_CLANG)
#  error "Boost.Config must identify BCC64X through clang.hpp"
#endif
#if defined(BOOST_EMBTC)
#  error "Historical Boost CodeGear layer must be bypassed"
#endif
#if defined(BOOST_NO_CXX11_NOEXCEPT)
#  error "Boost.Config must retain native noexcept"
#endif
int ProbeBoostConfig() { return 0; }
