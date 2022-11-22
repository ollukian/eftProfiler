set(tester_source_files
        FileSystemTester.cpp
        TestColourUtils.cpp
        TestCommandLineArguments.cpp
        TestPlotterUtils.cpp
        TestRooFit.cpp
        TestStringUtils.cpp)

add_library(tester ${tester_source_files})
#target_link_libraries(core modules)
target_link_libraries( tester PUBLIC ${ROOT_LIBRARIES} )