/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file extractdrs.cpp Starting of ExtractDRS and some preliminary functions */

#include "drs.h"
#include "extractdrs.h"

#include <vector>

string ReadFile(string path)
{
	ifstream file;
	file.open(path.c_str(), ios::in | ios::binary | ios::ate);
	if (!file.is_open()) {
		cerr << "Error opening file: " << path << '\n';
		return NULL;
	}

	/* Get the whole file */
	int size = (int)file.tellg();

	char *memblock = new char[size];
	file.seekg(0);
	file.read(memblock, size);
	file.close();
	string filedata(memblock, size);
	delete[] memblock;
	file.close();

	return filedata;
}

/**
 * Generate a list of files with a .drs extension
 * @param path The directory to look in for files
 * @return A list of paths to drs files, in string format
 */
vector<string> ListFiles(const char *path)
{
	DIR *dirFile = opendir(path);
	vector<string> filelist;
	if (dirFile) {
		struct dirent *hFile;
		errno = 0;
		while ((hFile = readdir(dirFile)) != NULL) {
			/* Ignore hidden files */
			if (hFile->d_name[0] == '.') continue;

			if (strstr(hFile->d_name, ".drs")) {
				string fullfile = path;
				fullfile += hFile->d_name;
				cout << "Found: " << fullfile << '\n';
				filelist.push_back(fullfile);
			}
		}
	} else {
		cerr << "Error opening directory: " << path << '\n';
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
		cerr << "Only one argument allowed, which must end in '/'\n";
		return 1;
	}
	string drsdirname = argv[1];
	vector<string> filelist = ListFiles(drsdirname.c_str());

	FioCreateDirectory(EXTRACT_DIR.c_str());
	for (uint i = 0; i < filelist.size(); i++) {
		ExtractDRSFile(filelist[i]);
	}
	return 0;
}
