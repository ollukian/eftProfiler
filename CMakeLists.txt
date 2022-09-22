cmake_minimum_required(VERSION 3.13)
project(eftProfiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_STANDARD LESS 17)
    message(WARNING "Incompatible C++ standard ${CMAKE_CXX_STANDARD}. EFT requires C++17")
endif()

message(STATUS "Compile in the ${CMAKE_BUILD_TYPE} mode")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Adding compiler flags for debugging")
    if (MSVC)
        add_compile_options(/W3)
    else()
        add_compile_options(-Wall -Wextra -pedantic -Werror -Wformat=2 -Wformat-truncation
                -fstack-usage
                -ffunction-sections -fdata-sections --gc-sections --print-gc-sections
                )
    endif()
endif()

#########################################################
# 3rd-party libraries                                   #
#########################################################
# bind root packages - it's required only to open TFiles with TH1D to extract cross-section
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

add_subdirectory(Core)
add_subdirectory(Utils)
add_subdirectory(Fitter)
add_subdirectory(Application)

add_executable(eftProfiler main.cpp)

# target ROOT
target_link_libraries( eftProfiler PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries(eftProfiler PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(eftProfiler PUBLIC core fitter utils app)


#install(TARGETS eftProfiler LIBRARY DESTINATION lib)

install(TARGETS eftProfiler
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(TARGETS eftProfiler EXPORT eftProfiler DESTINATION export)