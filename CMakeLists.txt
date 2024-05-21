list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Osmium REQUIRED)

if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()

