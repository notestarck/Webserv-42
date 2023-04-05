//
// Created by stephane walter on 19/03/2023.
//

#ifndef WEBSERV_42_RESPONSE_HPP
#define WEBSERV_42_RESPONSE_HPP


#include <string>
#include <map>


class Response{
private:
    std::map<std::string, std::string> headers;

    Response();
public:
    std::string status_code;
    std::string status_word;
    std::string body;
    Response(std::string status);
    ~Response();

    void append_header(std::string one, std::string two);
    int get_body_size();
    void set_body(std::string &str);

    void make_status_body();
    void make_status_body(std::string url);

    std::string make_header();
    std::string run_resp();



};

#endif //WEBSERV_42_RESPONSE_HPP
