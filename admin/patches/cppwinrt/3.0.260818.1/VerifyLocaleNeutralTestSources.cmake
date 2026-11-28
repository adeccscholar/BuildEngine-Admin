# Verify locale-independent upstream tests before any BCC64X compile.
# Only the cppwinrt source tree is inspected and touched.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT IS_DIRECTORY "${SOURCE_ROOT}")
   message(FATAL_ERROR "CPPWINRT source verification requires SOURCE_ROOT")
endif()

set(_test_source "${SOURCE_ROOT}/test/test/error_info.cpp")
set(_class_source "${SOURCE_ROOT}/test/test/hresult_class_not_registered.cpp")
set(_old_source "${SOURCE_ROOT}/test/old_tests/UnitTests/hresult_error.cpp")
foreach(_source IN ITEMS "${_test_source}" "${_class_source}" "${_old_source}")
   if(NOT EXISTS "${_source}")
      message(FATAL_ERROR "CPPWINRT patched test source missing: ${_source}")
   endif()
endforeach()

file(READ "${_test_source}" _test_text)
file(READ "${_class_source}" _class_text)
file(READ "${_old_source}" _old_text)

foreach(_literal IN ITEMS
   "REQUIRE(e.message() == L\"The parameter is incorrect.\")"
   "REQUIRE(e.message() == L\"bad is not a valid absolute URI.\")")
   string(FIND "${_test_text}" "${_literal}" _position)
   if(NOT _position EQUAL -1)
      message(FATAL_ERROR "CPPWINRT locale-dependent assertion remains in error_info.cpp: ${_literal}")
   endif()
endforeach()

string(FIND "${_class_text}" "REQUIRE(hresult_class_not_registered().message() == L\"Class not registered\")" _position)
if(NOT _position EQUAL -1)
   message(FATAL_ERROR "CPPWINRT locale-dependent class-not-registered assertion remains")
endif()

foreach(_literal IN ITEMS
   "BAD is not a valid absolute URI."
   "The parameter is incorrect."
   "Access is denied."
   "The application called an interface that was marshalled for a different thread."
   "Not implemented"
   "The operation attempted to access data outside the valid range"
   "No such interface supported"
   "ClassFactory cannot supply requested class")
   string(FIND "${_old_text}" "L\"${_literal}\" == e.message()" _position)
   if(NOT _position EQUAL -1)
      message(FATAL_ERROR "CPPWINRT locale-dependent legacy assertion remains: ${_literal}")
   endif()
endforeach()

# Require the semantic replacement to exist; absence of the old string alone is insufficient.
string(REGEX MATCHALL "REQUIRE\\(!e\\.message\\(\\)\\.empty\\(\\)\\)" _old_checks "${_old_text}")
list(LENGTH _old_checks _old_check_count)
if(_old_check_count LESS 10)
   message(FATAL_ERROR "CPPWINRT legacy HRESULT test replacements incomplete: ${_old_check_count}/10")
endif()

# The archive may preserve older timestamps than existing Ninja objects.
# Refresh only the three modified translation units, not the entire build.
file(TOUCH "${_test_source}" "${_class_source}" "${_old_source}")
message(STATUS "CPPWINRT locale-independent assertions verified; three test sources refreshed")
