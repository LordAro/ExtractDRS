/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file drs.h Declarations of types necessary to extracting drs files */

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

typedef unsigned char byte;

struct DRS_Header
{
	string header;
	string copyright;
	string version;
	string type;
	uint numtables;
	uint firstoffset;
};

struct DRS_Table
{
	uint fileid;
	uint fileoffset;
	uint filesize;
};

struct DRS_TableInfo
{
	char character;
	string extension;
	uint tbloffset;
	uint numfiles;
	struct DRS_Table *fileinfo;
};

void FioCreateDirectory(const char *name);
void ExtractDRSFile(const string &path);

#endif /* DRS_H */
