//
// Created by Stephane Walter on 4/14/23.
//
#include "Autoindex.hpp"

pour verifier qu un repertoir existe  :


struct stat st;
if (stat(dir_name.c_str(), &st) == 0) {
std::cout << "Le répertoire " << dir_name << " existe déjà." <<
std::endl;
}

int main() {
	// Définir le nom du répertoire à parcourir
	std::string dir_name = "mon_repertoire";

	// Ouvrir le répertoire
	DIR* dir = opendir(dir_name.c_str());
	if (dir == NULL) {
		std::cerr << "Impossible d'ouvrir le répertoire." << std::endl;
		return 1;
	}

	// Parcourir les fichiers du répertoire
	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL) {
		// Ignorer les fichiers "." et ".."
		if (std::string(ent->d_name) == "." || std::string(ent->d_name) == "..") {
			continue;
		}
		// Afficher le nom du fichier
		std::cout << ent->d_name << std::endl;
	}

	// Fermer le répertoire
	closedir(dir);

	return 0;
}