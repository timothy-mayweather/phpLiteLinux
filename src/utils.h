#include <string>

std::string get_executable_dir();
std::string get_file_contents(const std::string& file);
std::string get_full_path(std::string path);
int random(unsigned int min, unsigned int max, int recursion_level=0);
