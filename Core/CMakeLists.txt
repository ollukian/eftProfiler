set(core_source_files
        EfPreCompHeaders.cpp
        WorkspaceWrapper.cpp
        CommandLineArgs.cpp
        Logger.cpp
        Profiler.cpp
        Core.cpp)

add_library(core ${core_source_files})
#target_link_libraries(core modules)
target_link_libraries( core PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries( core PUBLIC fitter )