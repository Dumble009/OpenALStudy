cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(ProjectName a)

project(${ProjectName} VERSION 0.1.0)

if(NOT DEFINED ENV{CIRCLECI})
include("C:/MyLib/CMakeModules/deploy.cmake")
endif()

include(CTest)
enable_testing()
add_subdirectory(tests)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS "sources/*.cpp")

add_executable(${ProjectName} ${SOURCES})

include(FetchContent)

FetchContent_Declare(
  wavAgent
  GIT_REPOSITORY  https://github.com/Dumble009/WavAgent.git
  GIT_TAG         origin/master
)
FetchContent_MakeAvailable(wavAgent)

target_include_directories(${ProjectName} PUBLIC "sources" "C:/Program Files (x86)/OpenAL 1.1 SDK/include" "${wavAgent_SOURCE_DIR}/libsources")
target_link_directories(${ProjectName} PUBLIC "C:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64" "${wavAgent_BINARY_DIR}")
target_link_libraries(${ProjectName} OpenAL32 WavAgent)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)