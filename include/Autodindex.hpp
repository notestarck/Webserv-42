/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Autodindex.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/18 13:43:54 by estarck           #+#    #+#             */
/*   Updated: 2023/04/18 16:33:03 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_42_AUTOINDEX_HPP
#define WEBSERV_42_AUTOINDEX_HPP

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "../include/Client.hpp"
#include "../include/Connection.hpp"
#include "../include/ParsConfig.hpp"

#include <vector>
enum
{
    ISDIR = 1,
    ISFILE = 2
};

void	startAutoIndex(Client &client, std::string filePath, std::string path);


#endif /* WEBSERV_42_AUTOINDEX_HPP */