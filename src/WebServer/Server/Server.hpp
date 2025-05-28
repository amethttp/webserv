#pragma once

#include <string>
#include <vector>
#include "Session/Session.hpp"
#include "Location/Location.hpp"

using namespace std;

class Server
{
private:
	vector<Location> locations;
	vector<int> ports;
	vector<string> names;
	vector<Session> sessions;

public:
};
