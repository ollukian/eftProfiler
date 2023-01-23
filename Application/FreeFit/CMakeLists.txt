set(freefit_source_files
        FreeFitManager.cpp)

include_directories(../../Vendors/nlohmann_json/include/)
include_directories(../../Core)

add_library(freefit ${freefit_source_files})

#target_link_libraries(app PUBLIC core)
target_link_libraries(freefit PUBLIC ${ROOT_LIBRARIES} )
target_link_libraries(freefit PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(freefit PUBLIC fitter)
target_link_libraries(freefit PUBLIC core)
target_link_libraries(freefit PUBLIC utils)