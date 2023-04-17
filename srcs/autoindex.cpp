//
// Created by Stephane Walter on 4/17/23.
//
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <utility>

typedef std::vector<std::pair<std::pair<std::string, std::string>, int> > t_dir;
typedef std::pair<std::string, std::string> t_spair;


enum
{
	ISDIR = 1,
	ISFILE = 2
};
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


t_dir show_dir_content(std::string path) {
	DIR *d = opendir(path.c_str());
	t_dir ret;

	if (d) {
		struct dirent *dir;
		while ((dir = readdir(d))) {
			if (dir->d_type != DT_DIR && dir->d_name[0] != '.') {
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
			} else if (dir->d_type == DT_DIR && dir->d_name[0] != '.') {
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


int main() {

	t_dir test;
	test = show_dir_content("../Webserv-42/www/style");


	std::sort(test.begin(), test.end());

	for(int i = 0; i < test.size(); i++ ) {
		std::cout << test[i].second << " " << test[i].first.first << " " << test[i].first.second << std::endl;


	}
	return 0;
}