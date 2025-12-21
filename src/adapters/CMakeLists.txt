set(LIBRARY_SOURCES 
  "${CMAKE_CURRENT_SOURCE_DIR}/http/BeastHttpServer.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/http/HttpSession.cpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/storage/FileSystemRepository.cpp"
)

set(LIBRARY_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/http/BeastHttpServer.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/http/HttpSession.hpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/storage/FileSystemRepository.hpp"
)

# set(LIBRARY_INCLUDES "./")

add_library("venturi-adapters" STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories("venturi-adapters" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries("venturi-adapters" PUBLIC
  # Boost::boost
  Boost::system
)