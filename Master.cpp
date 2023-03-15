/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Master.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/15 15:50:10 by estarck           #+#    #+#             */
/*   Updated: 2023/03/15 17:23:46 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Master.hpp"

Master::Master(std::ifstream &config_file)
{}

Master::Master(const Master &srcs)
{}

Master::~Master()
{}

Master &Master::operator=(const Master &srcs)
{ return (*this); }
