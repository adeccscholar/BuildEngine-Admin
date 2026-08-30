#if !defined(__CODEGEARC__)
#  error "BCC64X preflight requires __CODEGEARC__"
#endif
#if !defined(__clang__)
#  error "BCC64X preflight requires __clang__"
#endif
#if !defined(__has_feature)
#  error "BCC64X preflight requires __has_feature"
#endif
#if !__has_feature(cxx_noexcept)
#  error "BCC64X must report cxx_noexcept"
#endif
int ProbeClangFeatures() { return 0; }
