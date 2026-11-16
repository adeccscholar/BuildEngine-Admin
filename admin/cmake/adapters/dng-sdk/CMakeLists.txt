cmake_minimum_required(VERSION 3.31)
project(dng_sdk LANGUAGES CXX)

# Explicitly curated from the pinned 1.7.1.2802 upstream Meson producer.
# No independent toolchain, static RTL or implicit third-party sources.
add_library(dng_sdk SHARED
   dng_sdk/source/dng_1d_function.cpp
   dng_sdk/source/dng_1d_table.cpp
   dng_sdk/source/dng_abort_sniffer.cpp
   dng_sdk/source/dng_area_task.cpp
   dng_sdk/source/dng_bad_pixels.cpp
   dng_sdk/source/dng_big_table.cpp
   dng_sdk/source/dng_bottlenecks.cpp
   dng_sdk/source/dng_camera_profile.cpp
   dng_sdk/source/dng_color_space.cpp
   dng_sdk/source/dng_color_spec.cpp
   dng_sdk/source/dng_date_time.cpp
   dng_sdk/source/dng_exceptions.cpp
   dng_sdk/source/dng_exif.cpp
   dng_sdk/source/dng_file_stream.cpp
   dng_sdk/source/dng_filter_task.cpp
   dng_sdk/source/dng_fingerprint.cpp
   dng_sdk/source/dng_gain_map.cpp
   dng_sdk/source/dng_globals.cpp
   dng_sdk/source/dng_host.cpp
   dng_sdk/source/dng_hue_sat_map.cpp
   dng_sdk/source/dng_ifd.cpp
   dng_sdk/source/dng_image.cpp
   dng_sdk/source/dng_image_writer.cpp
   dng_sdk/source/dng_info.cpp
   dng_sdk/source/dng_iptc.cpp
   dng_sdk/source/dng_jpeg_image.cpp
   dng_sdk/source/dng_jpeg_memory_source.cpp
   dng_sdk/source/dng_jxl.cpp
   dng_sdk/source/dng_lens_correction.cpp
   dng_sdk/source/dng_linearization_info.cpp
   dng_sdk/source/dng_local_string.cpp
   dng_sdk/source/dng_lossless_jpeg.cpp
   dng_sdk/source/dng_lossless_jpeg_shared.cpp
   dng_sdk/source/dng_matrix.cpp
   dng_sdk/source/dng_memory.cpp
   dng_sdk/source/dng_memory_stream.cpp
   dng_sdk/source/dng_misc_opcodes.cpp
   dng_sdk/source/dng_mosaic_info.cpp
   dng_sdk/source/dng_mutex.cpp
   dng_sdk/source/dng_negative.cpp
   dng_sdk/source/dng_opcode_list.cpp
   dng_sdk/source/dng_opcodes.cpp
   dng_sdk/source/dng_orientation.cpp
   dng_sdk/source/dng_parse_utils.cpp
   dng_sdk/source/dng_pixel_buffer.cpp
   dng_sdk/source/dng_point.cpp
   dng_sdk/source/dng_preview.cpp
   dng_sdk/source/dng_pthread.cpp
   dng_sdk/source/dng_rational.cpp
   dng_sdk/source/dng_read_image.cpp
   dng_sdk/source/dng_rect.cpp
   dng_sdk/source/dng_ref_counted_block.cpp
   dng_sdk/source/dng_reference.cpp
   dng_sdk/source/dng_render.cpp
   dng_sdk/source/dng_resample.cpp
   dng_sdk/source/dng_safe_arithmetic.cpp
   dng_sdk/source/dng_shared.cpp
   dng_sdk/source/dng_simple_image.cpp
   dng_sdk/source/dng_spline.cpp
   dng_sdk/source/dng_stream.cpp
   dng_sdk/source/dng_string.cpp
   dng_sdk/source/dng_string_list.cpp
   dng_sdk/source/dng_tag_types.cpp
   dng_sdk/source/dng_temperature.cpp
   dng_sdk/source/dng_tile_iterator.cpp
   dng_sdk/source/dng_tone_curve.cpp
   dng_sdk/source/dng_update_meta.cpp
   dng_sdk/source/dng_utils.cpp
   dng_sdk/source/dng_xmp.cpp
   dng_sdk/source/dng_xmp_sdk.cpp
   dng_sdk/source/dng_xy_coord.cpp
)
set_target_properties(dng_sdk PROPERTIES
   OUTPUT_NAME dng_sdk
   WINDOWS_EXPORT_ALL_SYMBOLS ON
   CXX_VISIBILITY_PRESET default
   CXX_STANDARD 23
   CXX_STANDARD_REQUIRED ON
   CXX_EXTENSIONS OFF
)
foreach(theRoot DNG_ZLIB_ROOT DNG_JPEG_ROOT DNG_JXL_ROOT)
   if(NOT DEFINED ${theRoot} OR NOT IS_DIRECTORY "${${theRoot}}")
      message(FATAL_ERROR "Missing managed dependency root: ${theRoot}")
   endif()
endforeach()
foreach(theName IN ITEMS
   "dng_jpeg:${DNG_JPEG_ROOT}:jpeg;libjpeg"
   "dng_zlib:${DNG_ZLIB_ROOT}:zlib;zlib1;zlibstatic"
   "dng_jxl:${DNG_JXL_ROOT}:jxl;libjxl"
   "dng_jxl_threads:${DNG_JXL_ROOT}:jxl_threads;libjxl_threads")
   string(REPLACE ":" ";" theFields "${theName}")
   list(GET theFields 0 theVariable)
   list(GET theFields 1 theRoot)
   list(GET theFields 2 theNames)
   string(REPLACE ";" " " theNames "${theNames}")
   separate_arguments(theCandidates NATIVE_COMMAND "${theNames}")
   find_library(${theVariable} NAMES ${theCandidates}
      PATHS "${theRoot}/lib/win64/${CMAKE_BUILD_TYPE}"
      NO_DEFAULT_PATH REQUIRED)
endforeach()
target_include_directories(dng_sdk
   PUBLIC
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dng_sdk/source>"
      "$<INSTALL_INTERFACE:include/dng_sdk/source>"
   PRIVATE
      "${DNG_JPEG_ROOT}/include"
      "${DNG_ZLIB_ROOT}/include"
      "${DNG_JXL_ROOT}/include"
)
target_compile_definitions(dng_sdk
   PUBLIC
      qDNGBigEndian=0
      qDNGUseXMP=0
      qDNGThreadSafe=1
      qDNGUseLibJPEG=1
      qDNGValidate=0
      qDNGValidateTarget=1
   PRIVATE
      qWinOS=1
      AdobePrivate=0
      qDNGReportErrors=0
)
target_link_libraries(dng_sdk PRIVATE
   "${dng_jpeg}" "${dng_zlib}" "${dng_jxl}" "${dng_jxl_threads}")
include(GNUInstallDirs)
install(TARGETS dng_sdk
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY dng_sdk/source/
   DESTINATION include/dng_sdk/source
   FILES_MATCHING PATTERN "*.h")
