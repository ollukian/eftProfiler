set(app_source_files
        IFitManager.cpp
        FitManager.cpp
        NpRankingStudyRes.cpp
        Ranking/OneNpManager.cpp
        Ranking/StudyRes.cpp
        Ranking/MissingNpsProcessor.cpp
        HesseStudyResult.cpp
        Ranking/CorrelationStudyProcessor.cpp
        Ranking/CorrelationStudyPlotSettings.cpp)

include_directories(../Vendors)
include_directories(../Core)

add_subdirectory(NllScans)

add_library(app ${app_source_files})

#target_link_libraries(app PUBLIC core)
target_link_libraries( app PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(app PUBLIC fitter)
target_link_libraries(app PUBLIC core)
target_link_libraries(app PUBLIC utils)
target_link_libraries(app PUBLIC nllscans)