#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <sys/stat.h>

using namespace std;

struct Table
{
	unsigned long fileid;
	unsigned long fileoffset;
	unsigned long filesize;
};

struct TableInfo
{
	unsigned char character;
	string extension;
	unsigned long tbloffset;
	unsigned long numfiles;
	struct Table *fileinfo;
};

/**
 * Converts (a part of) a string to a 4 bit long int.
 * @param str The string to operate on.
 * @param offset How far into the file to start.
 * @return the 'reformed' long int.
 */
unsigned long str2long(string str, int offset)
{
	return (unsigned char)str[offset] + ((unsigned char)str[offset + 1] << 8) + ((unsigned char)str[offset + 2] << 16) + ((unsigned char)str[offset + 3] << 24);
}

/**
 * Create a directory with the given name
 * @param name the name of the new directory.
 */
void FioCreateDirectory(const char *name)
{
#if defined(WIN32) || defined(WINCE)
	CreateDirectory(name, NULL);
#elif defined(OS2) && !defined(__INNOTEK_LIBC__)
	mkdir(name);
#elif defined(__MORPHOS__) || defined(__AMIGAOS__)
	size_t len = strlen(name) - 1;
	if (name[len] '/') {
		name[len] = '\0'; // Don't want a path-separator on the end
	}

	mkdir(name, 0755);
#else
	mkdir(name, 0755);
#endif
}

static const int HEADER_SIZE = 64;
static const int COPYRIGHT_SIZE = 40;
static const int VERSION_SIZE = 4;
static const int TYPE_SIZE = 12;

static const int TABLE_SIZE = 12;
static const int EXTENSTION_SIZE = 3;

int main (int argc, char **argv)
{
	ifstream file;
	file.open("gamedata.drs", ios::binary | ios::ate);
	if (file.is_open()) {
		/* Get the whole file */
		int size = (int)file.tellg();
		unsigned char *memblock = new unsigned char[size];
		file.seekg(0);
		file.read((char *)memblock, size);
		file.close();
		string drstext(reinterpret_cast<char *>(memblock), size);
		delete[] memblock;

		/* Get the header */
		string header = drstext.substr(0, HEADER_SIZE);
		string cpyright = header.substr(0, COPYRIGHT_SIZE);
		string version = header.substr(COPYRIGHT_SIZE, VERSION_SIZE);
		string type = header.substr(COPYRIGHT_SIZE + VERSION_SIZE, TYPE_SIZE);
		long numtables = str2long(header, 56);
		long firstoff = str2long(header, 60);

		cout << "Header:";
		cout << "\tCopyright info\t\t:" << cpyright << "\n";
		cout << "\tFile version\t\t:" << version << "\n";
		cout << "\tFile type\t\t:" << type << "\n";
		cout << "\tNumber of tables\t:" << numtables << "\n";
		cout << "\tOffset of first file\t:" << firstoff << "\n";
		printf("\n");

		/* Get tables */
		TableInfo *tableinfos = new TableInfo[numtables];
		for (int i = 0; i < numtables; i++) {
			string tableinfotext = drstext.substr(HEADER_SIZE + (i * TABLE_SIZE), TABLE_SIZE);
			tableinfos[i].character = tableinfotext[0];

			/* Reorder the extension */
			tableinfos[i].extension = tableinfotext.substr(1, 3);
			char tmp = tableinfos[i].extension[0];
			tableinfos[i].extension[0] = tableinfos[i].extension[2];
			tableinfos[i].extension[2] = tmp;

			tableinfos[i].tbloffset = str2long(tableinfotext, 4);
			tableinfos[i].numfiles = str2long(tableinfotext, 8);

			cout << "TableInfo No." << i + 1 << ":";
			cout << "\tCharacter\t:" << tableinfos[i].character << "\n";
			cout << "\t\tExtension\t:" << tableinfos[i].extension << "\n";
			cout << "\t\tTable offset\t:" << tableinfos[i].tbloffset << "\n";
			cout << "\t\tNum files\t:" << tableinfos[i].numfiles << "\n";
			printf("\n");

			tableinfos[i].fileinfo = new Table[tableinfos[i].numfiles];
			FioCreateDirectory("gamedata/");
			for (int j = 0; j < tableinfos[i].numfiles; j++) {
				string tabletext = drstext.substr(tableinfos[i].tbloffset + (j * TABLE_SIZE), TABLE_SIZE);
				tableinfos[i].fileinfo[j].fileid = str2long(tabletext, 0);
				tableinfos[i].fileinfo[j].fileoffset = str2long(tabletext, 4);
				tableinfos[i].fileinfo[j].filesize = str2long(tabletext, 8);

				cout << "\t\tTable No." << j + 1 << ":";
				cout << "\tFile ID\t\t:" << tableinfos[i].fileinfo[j].fileid << "\n";
				cout << "\t\t\t\tFile Offset\t:" << tableinfos[i].fileinfo[j].fileoffset << "\n";
				cout << "\t\t\t\tFile Size\t:" << tableinfos[i].fileinfo[j].filesize << "\n";

				ofstream outputfile;
				string filename("gamedata/");
				stringstream ss;
				ss << tableinfos[i].fileinfo[j].fileid;
				filename.append(ss.str());
				filename.append(".");
				filename.append(tableinfos[i].extension);
				outputfile.open(filename.c_str(), ios::out | ios::binary);
				if (outputfile.is_open()) {
					outputfile << drstext.substr(tableinfos[i].fileinfo[j].fileoffset, tableinfos[i].fileinfo[j].filesize);
					outputfile.close();
				}
			}
		}

		printf("\n");
	}
	file.close();
	return 0;
}
