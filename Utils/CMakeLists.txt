set(utils_source_files
        WsPlotter.cpp
        NpRankingPlotter.cpp NpRankingPlotter.h
        FitUtils.cpp FitUtils.h
        StringUtils.cpp StringUtils.h
        FileSystemUtils.cpp FileSystemUtils.h
        ColourUtils.cpp ColourUtils.h
        PlotterUtils.cpp PlotterUtils.h
        WorkspaceUtils.cpp WorkspaceUtils.h
        Scene.cpp Scene.h
        Drawing/Latex.cpp Drawing/Latex.h
        Drawing/Drawable.cpp Drawing/Drawable.h RooVarUtils.cpp RooVarUtils.h)

add_library(utils ${utils_source_files})
target_link_libraries(utils PRIVATE nlohmann_json::nlohmann_json)
#target_link_libraries(core modules)