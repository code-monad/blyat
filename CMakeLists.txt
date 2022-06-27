project(blyat)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WARN_DEPRECATED FALSE) # boost::beast C++17 warnings
cmake_minimum_required(VERSION 3.2)

if(APPLE)
  set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64") # For new mac build universal
  include_directories(/opt/local/include/) # For Macports
endif(APPLE)

add_compile_options(-Wno-deprecated)
find_package(Boost REQUIRED)



set(BLYAT_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party)

add_subdirectory(${BLYAT_DEPS_DIR}/spdlog)
add_subdirectory(${BLYAT_DEPS_DIR}/entt)
add_compile_definitions(TOML_EXCEPTIONS=0)
add_subdirectory(${BLYAT_DEPS_DIR}/tomlplusplus)

set(BLYAT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BLYAT_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(BLYAT_SRCS
  ${BLYAT_SRC_DIR}/core/exception.cc
  ${BLYAT_SRC_DIR}/tag.cc
  ${BLYAT_SRC_DIR}/mock.cc
  ${BLYAT_SRC_DIR}/uuid.cc
  ${BLYAT_SRC_DIR}/message.cc
  ${BLYAT_SRC_DIR}/socket.cc
  ${BLYAT_SRC_DIR}/session.cc
  ${BLYAT_SRC_DIR}/room.cc
  ${BLYAT_SRC_DIR}/server.cc
  ${BLYAT_SRC_DIR}/config.cc
  ${BLYAT_SRC_DIR}/main.cc)

add_executable(blyat ${BLYAT_SRCS})
target_include_directories(blyat PUBLIC ${BLYAT_INC_DIR})

target_link_libraries(blyat PRIVATE spdlog::spdlog_header_only
  PRIVATE EnTT
  PRIVATE tomlplusplus::tomlplusplus)
