#ifndef DRS_H
#define DRS_H

using namespace std;

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

#endif /* DRS_H */
