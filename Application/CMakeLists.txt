set(app_source_files
        IFitManager.cpp IFitManager.h
        FitManager.cpp FitManager.h
        NpRankingStudyRes.cpp NpRankingStudyRes.h
        FitManagerConfig.h RankingPlotterSettings.h
        Ranking/OneNpManager.cpp Ranking/OneNpManager.h
        Ranking/StudyRes.cpp Ranking/StudyRes.h
        HesseStudyResult.cpp HesseStudyResult.h)

include_directories(../Vendors)

add_library(app ${app_source_files})

#target_link_libraries(app PUBLIC core)
target_link_libraries( app PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(app PUBLIC fitter)
target_link_libraries(app PUBLIC core)
target_link_libraries(app PUBLIC utils)