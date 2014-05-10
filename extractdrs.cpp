/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file extractdrs.cpp Starting of ExtractDRS and some preliminary functions */

#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#include "extractdrs.h"
#include "drs.h"

/**
 * Create a directory with the given name
 * @note Taken from the OpenTTD project
 * @param name The name of the new directory
 */
void GenCreateDirectory(const std::string &name)
{
	mkdir(name.c_str(), 0755);
}

/**
 * Generate a list of files with a .drs extension
 * @param path The directory to look in for files
 * @return A list of paths to drs files, in string format
 */
std::vector<std::string> ListFiles(const char *path)
{
	std::vector<std::string> filelist;
	DIR *dirFile = opendir(path);
	if (dirFile) {
		struct dirent *hFile;
		errno = 0;
		while ((hFile = readdir(dirFile)) != nullptr) {
			/* Ignore hidden files */
			if (hFile->d_name[0] == '.') continue;

			if (strstr(hFile->d_name, ".drs")) {
				std::string fullfile = path;
				fullfile += hFile->d_name;
				std::cout << "Found: " << fullfile << std::endl;
				filelist.push_back(fullfile);
			}
		}
		closedir(dirFile);
	} else {
		std::cerr << "Error opening directory: " << path << std::endl;
	}
	return filelist;
}

/**
 * Main entry point of the program
 * @param argc An integer argument count of the command line arguments
 * @param argv An argument vector of the command line arguments
 * @return The exit status of the program
 */
int main(int argc, char **argv)
{
	if (argc != 2 || argv[1][strlen(argv[1]) - 1] != '/') {
		std::cerr << "Only one argument allowed, which must end in '/'" << std::endl;
		return 1;
	}
	std::vector<std::string> filelist = ListFiles(argv[1]);

	GenCreateDirectory(EXTRACT_DIR);
	for (uint i = 0; i < filelist.size(); i++) {
		ExtractDRSFile(filelist[i]);
	}
	return 0;
}
