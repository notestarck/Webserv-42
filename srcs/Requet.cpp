#include "Requet.hpp"

Requet::Requet(int client_fd){
    this->client_fd = client_fd;

}

Requet::~Requet(){}


int Requet::parse(std::string request){

    unsigned long i;
    int j;

    std::cout << "parsing request\n";
    i = request.find_first_of(" ", 0);
    method = request.substr(0, i);
    if(method == "PUT")
        return 200;
    if(!is_method(method))
        return 400;
    if ((unsigned long)(j = request.find_first_of(" ", i + 1)) == std::string::npos)
        return 400;
    path = request.substr(i + 1, j - i - 1);

    std::cout << path << "\n";
    headers["HTTP"] = request.substr(j + 1, request.find_first_of("\r", i) - j - 1);
    if(check_protocol(headers["HTTP"]) == false)
        return 505;

    return 1;

}

bool Requet::is_method(const std::string method) {
    if(method.empty())
        return false;
    for(unsigned long  i = 0; i < method.length(); i++){
        if(!isupper(static_cast<unsigned char>(method[i])))
            return false;
    }
    return true;
}

bool Requet::check_protocol(std::map<std::string, std::string>::mapped_type &test ){
    if(test == "HTTP/1.1")
        return true;
    return false;

}