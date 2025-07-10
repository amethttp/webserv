#include <ctime>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include "utils/http.hpp"
#include "Context/Context.hpp"
#include "Location/Location.hpp"
#include "Result/ExecutionResult.hpp"

typedef std::pair<std::string, std::string> t_cgi;

#define CHILD_OK 0
#define CGI_TIMEOUT 2
#define BUFFER_SIZE 4096

bool timedOut(time_t start);
std::string readOutput(int pipefd[2]);
t_httpCode waitForOutput(pid_t child, int pipefd[2], time_t start, t_body &body);
bool matchCGI(std::string &path, Location &location, t_cgi &cgi);