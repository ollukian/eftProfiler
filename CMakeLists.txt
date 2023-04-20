cmake_minimum_required(VERSION 3.13)
project(eftProfiler)

set(${PROJECT_NAME}_VERSION_MAJOR  1)
set(${PROJECT_NAME}_VERSION_MINOR  3)
set(${PROJECT_NAME}_VERSION_PATCH  0)
set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_STANDARD LESS 17)
    message(WARNING "Incompatible C++ standard ${CMAKE_CXX_STANDARD}. EFT requires C++17")
endif()

message("${PROJECT_NAME} version: ${${PROJECT_NAME}_VERSION}")
message(STATUS C++ standard:    [${CMAKE_CXX_STANDARD}])
message(STATUS C++ compiler id: [${CMAKE_CXX_COMPILER_ID}])

message(CHECK_START "Checking for compiler information")
string( TOLOWER "${CMAKE_CXX_COMPILER_ID}" COMPILER_ID )
if (COMPILER_ID STREQUAL "gcc")
    message(CHECK_PASS "Found GCC ${CMAKE_CXX_COMPILER_VERSION}")
elseif (COMPILER_ID STREQUAL "clang")
    message(CHECK_PASS "Found Clang ${CMAKE_CXX_COMPILER_VERSION}")
elseif (COMPILER_ID STREQUAL "msvc")
    message(CHECK_PASS "Found MSVC ${CMAKE_CXX_COMPILER_VERSION}")
else()
    message(CHECK_FAIL "Unknown compiler")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
## Check for compiler flags
message(STATUS "Build type: ${BUILD_TYPE}")




#add_compile_options(-D__ROOFIT_NOBANNER=1)

# Enable address sanitizer (gcc/clang only)
#function(eft_check_warnings target_name)
#    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#        message(STATUS "Address sanitizer enabled")
#        target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined)
#        target_compile_options(${target_name} PRIVATE -fno-sanitize=signed-integer-overflow)
#        target_compile_options(${target_name} PRIVATE -fno-sanitize-recover=all)
#        target_compile_options(${target_name} PRIVATE -fno-omit-frame-pointer)
#        target_link_libraries(${target_name}  PRIVATE -fsanitize=address,undefined -fuse-ld=gold)
#    endif()
#endfunction()

message(STATUS "Compile in the ${CMAKE_BUILD_TYPE} mode")

#########################################################
# 3rd-party libraries                                   #
#########################################################
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package( ROOT REQUIRED COMPONENTS RIO Hist RooFit RooStats RooFitCore)
include(${ROOT_USE_FILE})

# bind spdlog
list(APPEND CMAKE_PREFIX_PATH Vendors)
include_directories(Vendors)
#find_package(spdlog REQUIRED)

# bind json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(Vendors/nlohmann_json)
#########################################################

include_directories(Core)
include_directories(Utils)
include_directories(Fitter)
include_directories(Application)
include_directories(Test)

add_subdirectory(Core)
add_subdirectory(Utils)
add_subdirectory(Fitter)
add_subdirectory(Application)
add_subdirectory(Test)

add_executable(eftProfiler main.cpp Test/Tester.cpp)

if (BUILD_TYPE STREQUAL "debug")
    message(STATUS "Adding compiler flags for debugging")
    message(STATUS "Address sanitizer enabled")
    target_compile_options(eftProfiler PRIVATE -fsanitize=address,undefined)
    target_compile_options(eftProfiler PRIVATE -fno-sanitize=signed-integer-overflow)
    target_compile_options(eftProfiler PRIVATE -fno-sanitize-recover=all)
    target_compile_options(eftProfiler PRIVATE -fno-omit-frame-pointer)
    target_link_libraries(eftProfiler  PRIVATE -fsanitize=address,undefined -fuse-ld=gold)
    if (MSVC)
        add_compile_options(/W3)
    else()
        add_compile_options(-Wall -Wextra -pedantic -Werror -Wformat=2 -Wformat-truncation
            -fstack-usage
            -ffunction-sections -fdata-sections
            )
    endif()
endif()

# target ROOT
target_link_libraries( eftProfiler PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries(eftProfiler PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(eftProfiler PUBLIC core fitter utils app tester)


#install(TARGETS eftProfiler LIBRARY DESTINATION lib)

install(TARGETS eftProfiler
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(TARGETS eftProfiler EXPORT eftProfiler DESTINATION export)