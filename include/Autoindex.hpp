//
// Created by Stephane Walter on 4/14/23.
//

#ifndef WEBSERV_42_AUTOINDEX_HPP
#define WEBSERV_42_AUTOINDEX_HPP

#endif //WEBSERV_42_AUTOINDEX_HPP
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <utility>

#include"


typedef std::vector<std::pair<std::pair<std::string, std::string>, int> > t_dir;
typedef std::pair<std::string, std::string> t_spair;


enum
{
    ISDIR = 1,
    ISFILE = 2
};

std::string dir_listing()
{
    std::string path = "www/";
    DIR *dir;
    struct dirent *ent;
    dir = opendir(path.c_str());

    std::string result = "<ul>";
    while ((ent = readdir(dir)) != NULL)
    {
        result += "<li><a href=\"" + (std::string)ent->d_name + ">"
                  + (std::string)ent->d_name + "<\\a><\\li>";
    }
    result += "</ul>";
    return result;
}

void ServerManager::send_autoindex_page(Client &client, std::string path)
{
    std::cout << "> Send autoindex page\n";
    std::string addr = find_path_in_root(path, client);
    std::string result = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
                         "<title>webserv</title></head><body><h1>webserv</h1><h2>Index of ";
    result += path;
    result += "<hr><div>";

    DIR *dir = NULL;
    if (path[path.size() - 1] != '/')
        path += "/";
    if ((dir = opendir(addr.c_str())) == NULL)
        return;

    struct dirent *file = NULL;
    while ((file = readdir(dir)) != NULL)
    {
        if (strcmp(file->d_name, ".") || strcmp(file->d_name, ".."))
            result += "<a href=\"" + path + file->d_name;
        else if (path[path.length() - 1] == '/')
            result += "<a href=\"" + path + file->d_name;
        else
            result += "<a href=\"" + path + file->d_name;
        result += (file->d_type == DT_DIR ? "/" : "") + (std::string)"\">";
        result += (std::string)(file->d_name) + (file->d_type == DT_DIR ? "/" : "") + "</a><br>";
    }
    closedir(dir);

    result += "</div></body></html>";

    Response response(status_info[200]);
    response.append_header("Content-Length", number_to_string(result.length()));
    response.append_header("Content-Type", "text/html");
    std::string header = response.make_header();

    int send_ret = send(client.get_socket(), header.c_str(), header.size(), 0);
    if (send_ret < 0)
    {
        send_error_page(500, client, NULL);
        return;
    }
    else if (send_ret == 0)
    {
        send_error_page(400, client, NULL);
        return;
    }
    send_ret = send(client.get_socket(), result.c_str(), result.length(), 0);
    if (send_ret < 0)
        send_error_page(500, client, NULL);
    else if (send_ret == 0)
        send_error_page(400, client, NULL);
}



int Response::handle_autoindex()
{


    std::ofstream file;
    if (locations.getAutoIndex() == "on")
    {
        std::string tmp = full_path;
        struct stat s;
        stat(full_path.c_str(),&s);
        if ((access((const char *)full_path.c_str(), F_OK) != -1) && !S_ISDIR(s.st_mode))
            return 1;
        if (S_ISDIR(s.st_mode))
        {
            full_path = "www/autoindex/e.html";
            file.open(full_path);
            file.clear();
            file << generate_autoindex(tmp);
            file.close();
            return 1;
        }
    }
    if (send_error("404", " Not Found "))
    {
        std::cout << RED << "Response 404 Not Found " << trim(_request.Getrequest()["Path"]) << " " << trim(_request.Getrequest()["Version"]) << RESET << std::endl;
        std::string message = (char *)"HTTP/1.1 404 \r\nConnection: close\r\nContent-Length: 73";
        message += "\r\n\r\n<!DOCTYPE html><head><title>Not Found</title></head><body> </body></html>";
        send(_ClientFD, message.c_str(), message.size(), 0);
        FD_CLR(_ClientFD, &w);
        FD_SET(_ClientFD, &r);
        done = 1;
    }
    return 0;
}
std::string getsize(long long st_size)
{
    std::stringstream stream;
    std::string result;
    if (st_size < 1048576)
    {
        stream << std::fixed << std::setprecision(2) << (long double)(st_size) / 1024;
        result = stream.str();
        return result + "K";
    }
    else if (st_size < 524288000)
    {
        stream << std::fixed << std::setprecision(2) << (long double)(st_size) / 1048576;
        result = stream.str();
        return result + "M";
    }
    else
    {
        stream << std::fixed << std::setprecision(2) << (long double)(st_size) / 1073741824;
        result = stream.str();
        return result + "G";
    }
}

t_dir show_dir_content(std::string path)
{
    DIR *d = opendir(path.c_str());
    t_dir ret;

    if (d)
    {
        struct dirent *dir;
        while ((dir = readdir(d)))
        {
            if (dir->d_type != DT_DIR && dir->d_name[0] != '.')
            {
                struct stat stats;

                stat((std::string(path) + "/" + std::string(dir->d_name)).c_str(), &stats);

                t_spair FileAndSize;
                FileAndSize.first = dir->d_name;
                FileAndSize.second = getsize(stats.st_size).c_str();

                t_dir::value_type PairAndType;
                PairAndType.first = FileAndSize;
                PairAndType.second = ISFILE;

                ret.push_back(PairAndType);
            }
            else if (dir->d_type == DT_DIR && dir->d_name[0] != '.')
            {
                struct stat stats;

                stat((std::string(path) + "/" + std::string(dir->d_name)).c_str(), &stats);

                t_spair DirAndSize;
                DirAndSize.first = dir->d_name;
                DirAndSize.second = "-";

                t_dir::value_type PairAndType;
                PairAndType.first = DirAndSize;
                PairAndType.second = ISDIR;

                ret.push_back(PairAndType);
            }
        }
        closedir(d);
    }
    return ret;
}
std::string generate_autoindex(std::string path)
{
    std::string ret = "";
    if (!path.empty())
    {
        t_dir vec = show_dir_content(path);
        ret += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\"><html><head><title>Index of " + path.substr(1) + "</title></head>";
        ret += "<body><h1>Index of " + path.substr(1) + "</h1>";
        ret += "<table><tr><th valign='top'></th><th><a href='?C=N;O=D'>Name</a></th><th><a href='?C=S;O=A'>Size</a></th></tr><tr>\ <th colspan='5'><hr></th></tr>\";

        for (size_t i = 0; i < vec.size(); i++)
        {
            ret += "<tr>";
            ret += "<td valign='top'><img src='" + ((vec[i].second == ISFILE) ? std::string(filecdn) : std::string(foldercdn)) + "' alt='[DIR]'></td>";
            ret += "<td><a href='" + vec[i].first.first + "'>" + vec[i].first.first + "</a></td>";
            ret += "<td align='right'> " + vec[i].first.second + " </td>";
            ret += "<td>&nbsp;</td>";
        }
        ret += "<tr><th colspan='5'><hr></th></tr></table></body>";
    }
    return ret;
}