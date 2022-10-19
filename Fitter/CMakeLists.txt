set(fitter_source_files
        IFitter.h
        Fitter.cpp Fitter.h
        IFitResult.cpp IFitResult.h ErrorsReEvaluation.h FitSettings.cpp FitSettings.h)

add_library(fitter ${fitter_source_files})
#target_link_libraries(core modules)
target_link_libraries( fitter PUBLIC ${ROOT_LIBRARIES} )