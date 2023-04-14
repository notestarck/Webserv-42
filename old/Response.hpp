/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 15:45:52 by estarck           #+#    #+#             */
/*   Updated: 2023/04/06 15:46:34 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_42_RESPONSE_HPP
#define WEBSERV_42_RESPONSE_HPP

#include <string>
#include <map>

class Response
{
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
};//Class Reponse

#endif /* WEBSERV_42_RESPONSE_HPP */
