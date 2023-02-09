#include "utils.h"
#include "mongoose_server.h"
#include "settings.h"
#include <cstdlib>

std::string g_cgi_env_from_argv;

int main(int argc, char **argv) {
    if (argv) {
        for (int i = 0; i < argc; i++) {
            std::string arg = argv[i];
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string name = arg.substr(0, pos);
                std::string value = arg.substr(pos+1, std::string::npos);
                if (name == "--cgi-environment" && value.length()) {
                    g_cgi_env_from_argv.assign(value);
                }
            }
        }
    }


    // If reading settings.json fails exit app immediately
    json_value* app_settings = get_app_settings();
    if (get_app_settings_error().length()) {
        std::string error = get_app_settings_error();
        error.append(".\nApplication will terminate immediately.");
        return 1;
    }

    // Start Mongoose server
    mongoose_start();

    std::string target((*app_settings)["ui"]["target"]);
    std::string command = target+" --url="+mongoose_get_url();

    //start ui
    std::system(command.c_str());
    return 0;
}
