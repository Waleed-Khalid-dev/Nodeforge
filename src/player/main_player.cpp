#include "PlayerApp.h"
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    
    nf::player::PlayerOptions options = nf::player::PlayerApp::ParseArgs(argc, argv);
    nf::player::PlayerApp app;

    if (!app.Initialize(options)) {
        spdlog::critical("Failed to initialize NodeForge Player");
        return 1;
    }

    return app.Run();
}
