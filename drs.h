#ifndef DRS_H
#define DRS_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

using namespace std;

static const string PATHSEP = "/";
static const string EXTRACT_DIR = "extracted" + PATHSEP;

static const int HEADER_SIZE = 64;
static const int COPYRIGHT_SIZE = 40;
static const int VERSION_SIZE = 4;
static const int TYPE_SIZE = 12;

static const int TABLE_SIZE = 12;
static const int EXTENSTION_SIZE = 3;

struct Header
{
	string header;
	string copyright;
	string version;
	string type;
	uint numtables;
	uint firstoffset;
};

struct Table
{
	uint fileid;
	uint fileoffset;
	uint filesize;
};

struct TableInfo
{
	char character;
	string extension;
	uint tbloffset;
	uint numfiles;
	struct Table *fileinfo;
};

void FioCreateDirectory(const char *name);
void ExtractDRSFile(string path);

#endif /* DRS_H */
