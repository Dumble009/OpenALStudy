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

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS "sources/*.cpp")

add_executable(${ProjectName} ${SOURCES})

target_include_directories(${ProjectName} PUBLIC "sources" "C:/Program Files (x86)/OpenAL 1.1 SDK/include")
#target_link_libraries(${ProjectName} CircleCITest)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)