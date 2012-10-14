#ifndef DRS_H
#define DRS_H

using namespace std;

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
