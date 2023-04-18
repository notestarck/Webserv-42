/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autoindex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/18 13:44:04 by estarck           #+#    #+#             */
/*   Updated: 2023/04/18 17:10:25 by estarck          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Autodindex.hpp"

typedef std::vector<std::pair<std::pair<std::string, std::string>, int> > t_dir;
typedef std::pair<std::string, std::string> t_spair;

static std::string getsize(long long st_size)
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

std::string	getDate(struct stat &st)
{
	std::string	date;
	char		buffer[256];
	struct tm	*date_tm;


	date_tm = std::localtime(&st.st_mtimespec.tv_sec);

	std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", date_tm);
	date = buffer;
	return (date);
}


t_dir show_dir_content(std::string path)
{
	DIR *d = opendir(path.c_str());
	t_dir	ret;

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
				FileAndSize.second = getsize(stats.st_size).c_str() ;
				FileAndSize.second.append("   ");
				FileAndSize.second.append(getDate(stats));

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

void	startAutoIndex(Client &client, std::string filePath, std::string path)
{
	client._autoIndex = show_dir_content(path.data());
	
	std::ifstream	file(filePath, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
    	{
			std::cout << line.find("%DIRECTORY%") << std::endl;
			
			if(line.find("%DIRECTORY%") != std::string::npos)
				client._bodyRep.append("<h1>Auto Index : " + path + "</h1>");
			else if(line.find("%TYPE%") != std::string::npos)
			{
				for(size_t i = 0; i < client._autoIndex.size(); i++ )
					client._bodyRep.append("<p>" + std::to_string(client._autoIndex[i].second) + "</p>");
			}
			else if(line.find("%NAME%") != std::string::npos)
			{
				for(size_t i = 0; i < client._autoIndex.size(); i++ )
					client._bodyRep.append("<p>" + client._autoIndex[i].first.first + "</p>");
			}
			else if(line.find("%DATE%") != std::string::npos)
			{
				for(size_t i = 0; i < client._autoIndex.size(); i++ )
					client._bodyRep.append("<p>" + client._autoIndex[i].first.second + "</p>");
			}
			else
				client._bodyRep.append(line + "\n");
			//std::cout << client._bodyRep << std::endl;
    	    line.clear();
    	}
		createHttpResponse(client, 200, "text/html");
		sendHttpResponse(client);
	}
	file.close();
}