set(nll_source_files
        NllScanResult.cpp
        PoiConfig.cpp
        NllScanManager.cpp
        Grid.cpp
        PoiConfigBuilder.cpp
        NllScanPlotter.cpp
        NllScanPlotterSettings.cpp)

include_directories(../../Vendors)
include_directories(../../Core)

add_library(nllscans ${nll_source_files})

#target_link_libraries(app PUBLIC core)
target_link_libraries(nllscans PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries(nllscans PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(nllscans PUBLIC fitter)
target_link_libraries(nllscans PUBLIC core)
target_link_libraries(nllscans PUBLIC utils)