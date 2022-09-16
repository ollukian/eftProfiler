set(core_source_files
        IWorkspaceWrapper.h
        WorkspaceWrapper.cpp WorkspaceWrapper.h
        CmdLineArgs.cpp CmdLineArgs.h Logger.h Logger.cpp)

add_library(core ${core_source_files})
#target_link_libraries(core modules)
target_link_libraries( core PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries( core PUBLIC fitter )