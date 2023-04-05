/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/17 15:02:17 by estarck           #+#    #+#             */
/*   Updated: 2023/03/17 15:16:36 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <fstream>
#include <string>

#include "./ParsConfig.hpp"
#include "./Server.hpp"
#include "./Connection.hpp"

#include <vector>

extern std::vector<Server *>		_server;
extern std::vector<ParsConfig *>	_config;
extern Connection					_connection;
extern volatile bool				boolStart;

void	delProg();

void 	signal_handler(int signal);

int 	countServer(std::ifstream &config_file);
bool	checkExtension(std::string extension);

#endif /* _UTILS_H_ */