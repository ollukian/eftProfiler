set(core_source_files
        IWorksSpaceWrapper.h
        WorkspaceWrapper.cpp WorkspaceWrapper.h
        IFitManager.cpp IFitManager.h)

add_library(core ${core_source_files})
#target_link_libraries(core modules)
target_link_libraries( core PUBLIC ${ROOT_LIBRARIES} )