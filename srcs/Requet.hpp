#include <iostream>
#include <string>
#include <map>
#include <sstream>

class Requet{
private:

int client_fd;
Requet();

public:

std::string path;
std::string method;
std::map<std::string, std::string> headers;
std::string body;


Requet(int i);
~Requet();

int parse(std::string request);
bool is_not_method(const std::string method);

bool check_protocol(std::map<std::string, std::string>::mapped_type &test);

};