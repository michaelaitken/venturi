set(LIBRARY_SOURCES 
  "${CMAKE_CURRENT_SOURCE_DIR}/services/MediaService.cpp"
)

set(LIBRARY_HEADERS 
  "${CMAKE_CURRENT_SOURCE_DIR}/entities/MediaInfo.hpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/ports/IHttpServer.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ports/IMediaRepository.hpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/services/MediaService.hpp"
)

# set(LIBRARY_INCLUDES "./")

add_library("venturi-core" STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories("venturi-core" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries("venturi-core" PUBLIC
  # Boost::boost
  Boost::system
)