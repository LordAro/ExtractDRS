#include <vector>

#include "drs.h"

using namespace std;

vector<string> ListFiles(const char *path)
{
	DIR *dirFile = opendir(path);
	vector<string> filelist;
	if (dirFile) {
		struct dirent *hFile;
		errno = 0;
		while ((hFile = readdir(dirFile)) != NULL) {
			// Ignore hidden files
			if (hFile->d_name[0] == '.') continue;

			if (strstr(hFile->d_name, ".drs")) {
				string fullfile = path;
				fullfile += hFile->d_name;
				cout << "Found: " << fullfile << "\n";
				filelist.push_back(fullfile);
			}
		}
	} else {
		cout << "Error opening directory: " << path << '\n';
	}
	return filelist;
}

int main(int argc, char **argv)
{
	if (argc != 2 || argv[1][strlen(argv[1]) - 1] != '/') {
		cout << "Only one argument allowed, which must end in '/'\n";
		return 1;
	}
	std::string drsdirname = argv[1];
	vector<string> filelist = ListFiles(drsdirname.c_str());

	FioCreateDirectory(EXTRACT_DIR.c_str());
	for (uint i = 0; i < filelist.size(); i++) {
		ExtractDRSFile(filelist[i]);
	}
	return 0;
}
