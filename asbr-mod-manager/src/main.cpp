#include <borealis.hpp>

#include "main_activity.hpp"
#include "mod_manager.hpp"

// Title ID de JoJo's Bizarre Adventure: All-Star Battle R (Switch)
static const char* GAME_DATA = "/atmosphere/contents/010017A0128C4000/romfs/data";
static const char* MODS_ROOT = "/switch/asbr-mods";

int main(int, char**) {
    if (!brls::Application::init()) return EXIT_FAILURE;
    brls::Application::createWindow("ASBR Mod Manager");

    static asbr::ModManager mgr(MODS_ROOT, GAME_DATA);
    brls::Application::pushActivity(new MainActivity(mgr));

    while (brls::Application::mainLoop()) {
    }
    return EXIT_SUCCESS;
}
