#include <string>

bool mongoose_start();

[[maybe_unused]] void mongoose_stop();

[[maybe_unused]] std::string mongoose_get_port();

[[maybe_unused]] int mongoose_get_port_int();

[[maybe_unused]] std::string mongoose_get_ip_address();
std::string mongoose_get_url();
