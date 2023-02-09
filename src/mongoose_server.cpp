#include "mongoose.h"
#include "utils.h"
#include "settings.h"
#include <iostream>

#include <sstream>

// If port was set to 0, then real port will be known
// only after the webserver was started.

std::string g_mongoose_port;
int g_mongoose_port_int = 0;
std::string g_mongoose_ip_address;
std::string g_mongoose_url;

struct mg_context* g_mongoose_context = nullptr;
extern std::string g_cgi_env_from_argv;

static int mongoose_error_message(const struct mg_connection* conn, const char *message) {
    // Called when mongoose is about to log an error message.
    // If callback returns non-zero, mongoose does not log anything
    // to a file ("error_log_file" option).
//    LOG(ERROR) << message;
    return 0;
}

static void mongoose_end_request(const struct mg_connection* conn, int reply_status_code) {
    // Called when mongoose has finished processing request.
    mg_request_info* request = mg_get_request_info(
            const_cast<mg_connection*>(conn));
    std::string message;
    message.append(request->request_method);
    message.append(" ");
    std::stringstream ss;
    ss << reply_status_code;
    message.append(ss.str());
    message.append(" ");
    message.append(request->uri);
    if (request->query_string) {
        message.append("?");
        message.append(request->query_string);
    }
//    std::cout << message;
}

bool mongoose_start() {
    std::cout<<"Start Mongoose server";
    json_value* app_settings = get_app_settings();

    // ip_address, port_str
    std::string ip_address((*app_settings)["web_server"]["listen_on"][0]);
    long port_int = (*app_settings)["web_server"]["listen_on"][1];
    if (ip_address.empty()) {
        ip_address = "127.0.0.1";
    }
    g_mongoose_ip_address = ip_address;
    std::stringstream port_ss1;
    port_ss1 << port_int;
    std::string port_str = port_ss1.str();
    if (port_str.empty()) {
        port_str = "0";
    }

    // listening_ports (from ip_address and port_str)
    std::string listening_ports;
    if (ip_address == "*") {
        listening_ports = port_str;
    } else {
        listening_ports = ip_address + ":" + port_str;
    }
    std::cout<< "\nlistening_ports=" << listening_ports;

    // www_directory
    std::string www_directory((*app_settings)["web_server"]["www_directory"]);
    www_directory = get_full_path(www_directory);
    std::cout << "\nwww_directory=" << www_directory;

    // index_files
    const json_value index_files_array = (*app_settings)["web_server"]["index_files"];
    std::string index_files;
    for (int i = 0; i < 32; i++) {
        const char* file = index_files_array[i];
        if (strlen(file)) {
            if (index_files.length()) {
                index_files.append(",");
            }
            index_files.append(file);
        }
    }
    if (index_files.empty()) {
        index_files = "index.html,index.php";
    }
    std::cout << "\nindex_files=" << index_files;
    std::string hj("kjhjkkhkl");
    // cgi_interpreter
    std::string cgi_interpreter(
            (*app_settings)["web_server"]["cgi_interpreter"]);
    cgi_interpreter = get_full_path(cgi_interpreter);
    std::cout << "\ncgi_interpreter=" << cgi_interpreter;

    // cgi_pattern (from cgi_extensions)
    const json_value cgi_extensions = (*app_settings)["web_server"]["cgi_extensions"];
    std::string cgi_pattern;
    for (int i = 0; i < 32; i++) {
        const char* extension = cgi_extensions[i];
        if (strlen(extension)) {
            if (cgi_pattern.length()) {
                cgi_pattern.append("|");
            }
            cgi_pattern.append("**.").append(extension).append("$");
        }
    }
    if (cgi_pattern.empty()) {
        cgi_pattern = "**.php$";
    }
    std::cout << "\ncgi_pattern=" << cgi_pattern;

    // 404_handler
    std::string _404_handler((*app_settings)["web_server"]["404_handler"]);

    // Hide files patterns.
    const json_value hide_files = (*app_settings)["web_server"]["hide_files"];
    std::string hide_files_patterns;
    for (int i = 0; i < 100; i++) {
        const char* pattern = hide_files[i];
        if (strlen(pattern)) {
            if (hide_files_patterns.length()) {
                hide_files_patterns.append("|");
            }
            hide_files_patterns.append("**/").append(pattern).append("$");
        }
    }
    std::cout << "\nhide_files_patterns=" << hide_files_patterns;

    // cgi_environment (same as cgi_env)
    std::string cgi_env;
    cgi_env.append("TMP=").append("/tmp").append(",");
    cgi_env.append("TEMP=").append("/tmp").append(",");
    cgi_env.append("TMPDIR=").append("/tmp").append(",");
    if (ip_address != "*") {
        cgi_env.append("SERVER_NAME=").append(ip_address)
               .append(",");
        cgi_env.append("SERVER_ADDR=").append(ip_address)
               .append(",");
    }

    if (g_cgi_env_from_argv.length()) {
        cgi_env.append(",").append(g_cgi_env_from_argv);
    }
    std::cout << "\ncgi_environment=" << cgi_env;

    // Mongoose options
    const char* options[] = {
        "document_root", www_directory.c_str(),
        "listening_ports", listening_ports.c_str(),
        "index_files", index_files.c_str(),
        "cgi_interpreter", cgi_interpreter.c_str(),
        "cgi_pattern", cgi_pattern.c_str(),
        "cgi_environment", cgi_env.c_str(),
        "404_handler", _404_handler.c_str(),
        "hide_files_patterns", hide_files_patterns.c_str(),
        nullptr
    };


    // Start mongoose
    mg_callbacks callbacks = {nullptr};
    callbacks.log_message = &mongoose_error_message;
    callbacks.end_request = &mongoose_end_request;
    g_mongoose_context = mg_start(&callbacks, nullptr, options);
    if (g_mongoose_context == nullptr) {
        std::cout << "\nmg_start() failed";
        return false;
    }

    // When port was set to 0 then a random free port was assigned
    // by OS.
    int port = mg_get_listening_port(g_mongoose_context);
    g_mongoose_port_int = port;
    std::stringstream port_ss2;
    port_ss2 << port;
    g_mongoose_port = port_ss2.str();
    if (ip_address == "*") {
        g_mongoose_url = "http://127.0.0.1:" + port_ss2.str() + "/";
    } else {
        g_mongoose_url = "http://" + ip_address + ":" + port_ss2.str() + "/";
    }
    std::cout << "\nMongoose url: " << g_mongoose_url;
    std::cout << "\n";
    return true;
}

[[maybe_unused]] std::string mongoose_get_port() {
    return g_mongoose_port;
}

[[maybe_unused]] int mongoose_get_port_int() {
    return g_mongoose_port_int;
}

[[maybe_unused]] std::string mongoose_get_ip_address() {
    return g_mongoose_ip_address;
}

std::string mongoose_get_url() {
    return g_mongoose_url;
}
