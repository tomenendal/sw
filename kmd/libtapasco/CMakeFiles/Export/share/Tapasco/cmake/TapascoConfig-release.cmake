#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tapasco" for configuration "Release"
set_property(TARGET tapasco APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tapasco PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libtapasco.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tapasco )
list(APPEND _IMPORT_CHECK_FILES_FOR_tapasco "${_IMPORT_PREFIX}/lib64/libtapasco.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
