// Copyright (c) 2012 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "json.h"
#include "utils.h"

std::string g_app_settings_error;

json_value* get_app_settings() {
    static auto* ret = new json_value();
    static bool settings_fetched = false;
    if (settings_fetched)
        return ret;
    settings_fetched = true;

    std::string settings_file = get_executable_dir() + "/settings.json";
    std::string contents = get_file_contents(settings_file);
    if (contents.empty()) {
        g_app_settings_error = "Error while reading settings.json file";
        return ret;
    }

    json_settings settings;
    memset(&settings, 0, sizeof(json_settings));
    char error[256];
    json_value* json_parsed = json_parse_ex(&settings, contents.c_str(),
                                            contents.length(), &error[0]);
    if (json_parsed == nullptr) {
        g_app_settings_error = "Error while parsing settings.json file";
        return ret;
    }
    ret = json_parsed;
    return ret;
}

std::string get_app_settings_error() {
    return g_app_settings_error;
}
