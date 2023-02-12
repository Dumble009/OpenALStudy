cmake_minimum_required(VERSION 3.14)
project(test_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)

set(TestName GTest)
if(NOT TARGET ${TestName})

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false RELATIVE "${CMAKE_SOURCE_DIR}/tests" CONFIGURE_DEPENDS "sources/*.cpp")

add_executable(${TestName} ${TEST_SOURCES})

target_include_directories(${TestName} PUBLIC "${CMAKE_SOURCE_DIR}/libsources")
target_link_directories(${TestName} PUBLIC "${CMAKE_SOURCE_DIR}/build")
target_link_libraries(${TestName} GTest::gtest_main) #その他必要なライブラリを適宜ここに追加する

include(GoogleTest)
gtest_discover_tests(${TestName})
endif()