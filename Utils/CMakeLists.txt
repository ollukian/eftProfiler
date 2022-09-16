set(utils_source_files
        WsPlotter.cpp
        NpRankingPlotter.cpp NpRankingPlotter.h)

add_library(utils ${utils_source_files})
#target_link_libraries(core modules)