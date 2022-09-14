set(core_source_files
        IWorkspaceWrapper.h
        WorkspaceWrapper.cpp WorkspaceWrapper.h
)

add_library(core ${core_source_files})
#target_link_libraries(core modules)
target_link_libraries( core PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries( core PUBLIC fitter )