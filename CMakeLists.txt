cmake_minimum_required(VERSION 3.17)
project(eftProfiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_STANDARD LESS 17)
    message(WARNING "Incompatible C++ standard ${CMAKE_CXX_STANDARD}. EFT requires C++17")
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


install(TARGETS eftProfiler LIBRARY DESTINATION lib)

install(TARGETS eftProfiler
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(TARGETS eftProfiler EXPORT eftProfiler DESTINATION export)