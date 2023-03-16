/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/16 10:39:35 by estarck           #+#    #+#             */
/*   Updated: 2023/03/16 15:28:27 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"

Request::Request(int client_fd){
    this->client_fd = client_fd;

}

Request::~Request(){}


static size_t StringToHex(std::string input)
{
    std::stringstream convert;
    size_t ret = 0;
    convert << std::hex << input;
    convert >> ret;
    return ret;
}
static std::string parse_body(std::string &request, int i){
    return request.substr(i + 2, request.size());
}

static int parse_header(std::map<std::string, std::string> &headers, std::string &request, int i){
    int point = request.find_first_of(":", i);
    int end = request.find_first_of("\r\n", point);
    headers[request.substr(i, point - i)] = request.substr(point + 2, end - point - 2);
    return end;
}

static std::string parse_chunck(std::string &request, int i){
    size_t size = 1;
    std::string ret;
    std::string size_buf;
    std::string line = request.substr(i + 2, request.size());

    int j = 0;

    while(true)
    {
        size_t  r = line.find_first_of("\r\n");
        size_buf = line.substr(i, r);
        size = StringToHex(size_buf);
        j += 2 + size_buf.size();
        if(size == 0)
            break;
        std::string buf = line.substr(i, j +size - 2);
        ret += buf;
        j += size + 4;
    }
    return ret;

}



int Request::parse(std::string request){

    unsigned long i;
    int j;

    std::cout << "parsing request\n";
    i = request.find_first_of(" ", 0);
    method = request.substr(0, i);

    std::cout << "method is " << method << "\n";
    if(method == "PUT")
        return 200;
    if(is_not_method(method))
        return 400;
    if ((unsigned long)(j = request.find_first_of(" ", i + 1)) == std::string::npos)
        return 4001;
    path = request.substr(i + 1, j - i - 1);

    std::cout << "path is " << path << "\n";
    headers["HTTP"] = request.substr(j + 1, request.find_first_of("\r", i) - j - 1);
    if(check_protocol(headers["HTTP"]) == false)
    {

        return 505;}
    std::cout << "HTTP protocol is OK\n";
    i = request.find_first_of("\n", j) + 1;
    while(i < request.size())
    {
        if(request[i] == '\r' && request[i + 1] == '\0')
            break;
        if(request[i] == '\r' && request[i + 1] == '\n')
        {
        // https://fr.wikipedia.org/wiki/Chunked_transfer_encoding
        if(strstr(headers["Transfer-Encoding"].c_str(), "chunked") != NULL)
            this->body = parse_chunck(request,i);
        else
            this->body = parse_body(request, i);
        break;
        }
        int end = parse_header(this->headers, request, i);
        if (end + 1 == '\0')
            break;
        i = end + 2;
    }
    if(headers["Host"] == "")
        return 400;                               //a modifier code 400
    return 0;

}

bool Request::is_not_method(const std::string method) {
    if(method.empty())
        return true;
    for(unsigned long  i = 0; i < method.length(); i++){
        if(!isupper(static_cast<unsigned char>(method[i])))
            return true;
    }
    return false;
}

bool Request::check_protocol(std::map<std::string, std::string>::mapped_type &test ){
    if(test == "HTTP/1.1")
        return true;
    return false;

}
