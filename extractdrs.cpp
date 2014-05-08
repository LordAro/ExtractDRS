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
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#include "extractdrs.h"
#include "drs.h"

/**
 * Converts (a part of) a uint8 vector to a 4 byte uint.
 * @param arr The vector to operate on.
 * @param offset How far into the vector to start.
 * @return The converted uint.
 */
uint vec2uint(const std::vector<uint8> &vec, int offset)
{
	return vec[offset] + (vec[offset + 1] << 8) + (vec[offset + 2] << 16) + (vec[offset + 3] << 24);
}

/**
 * Create a directory with the given name
 * @note Taken from the OpenTTD project
 * @param name The name of the new directory
 */
void GenCreateDirectory(const std::string &name)
{
	mkdir(name.c_str(), 0755);
}

std::vector<uint8> ReadFile(const std::string &path)
{
	std::ifstream file;
	file.open(path.c_str(), std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << path << std::endl;
		return std::vector<uint8>();
	}

	/* Get the whole file */
	const int size = (int)file.tellg();

	uint8 *memblock = new uint8[size];

	file.seekg(0);
	file.read((char *)memblock, size);
	file.close();

	std::vector<uint8> data(memblock, memblock + size);

	delete[] memblock;
	return data;
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
