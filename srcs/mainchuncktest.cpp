//
// Created by Stephane Walter on 3/10/23.
//

#include <iostream>
#include <string>
#include <sstream>
#include "Requet.hpp"

int main()
{
    std::string request = "GET / HTTP/1.1 200 OK\r\n\n Content-Type: text/plain\r\n\n Transfer-Encoding: chunked\r\n\n \r\n\n 27\r\n\n Voici les données du premier morceau\r\n\r\n\n 1C\r\n\n et voici un second morceau\r\n\r\n\n 20\r\n\n et voici deux derniers morceaux \r\n\n 12\r\n\n sans saut de ligne\r\n\n 0\r\n\n \r\nu\r\n\r\n 20\r\n et voici deux derniers morceaux \r\n 12\r\n sans saut de ligne\r\n 0\r\n \r\n";
    std::string r2 = "GET /127.0.0.1 HTTP/1.1";

    Requet req = Requet(1);


    int t =  req.parse(r2);
    //std::string port = req.headers["Host"].substr(req.headers["Host"].find(':') + 1);
    //std::cout  <<" host= "<< port << "\n";


    std::cout << t << "\n";

    return  0;



}