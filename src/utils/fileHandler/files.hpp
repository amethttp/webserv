#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

void normalizeTrailingSlash(std::string &path);
bool pathExists(std::string &path);
int checkPath(std::string &path);
std::string readFileToString(const char *path);