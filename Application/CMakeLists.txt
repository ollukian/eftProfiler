set(app_source_files
        IFitManager.cpp IFitManager.h
        FitManager.cpp FitManager.h)

add_library(app ${app_source_files})

#target_link_libraries(app PUBLIC core)
target_link_libraries( app PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries(app PUBLIC fitter)