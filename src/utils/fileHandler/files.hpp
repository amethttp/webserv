#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include "WebServer/Client/Client.hpp"

std::string getMIME(const std::string &target);
void normalizeTrailingSlash(std::string &path);
bool pathExists(const std::string &path);
int checkPath(const std::string &path);
std::string readFileToString(const std::string &path);
std::string getFileExtension(const std::string &path);
