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
#include <string>

#include "extractdrs.h"
#include "slp.h"

static const uint8 HEADER_SIZE = 64;
static const uint8 COPYRIGHT_SIZE = 40;
static const uint8 VERSION_SIZE = 4;
static const uint8 TYPE_SIZE = 12;

static const uint8 TABLE_SIZE = 12;
static const uint8 EXTENSTION_SIZE = 3;

struct DRS_Header
{
	std::string copyright;
	std::string version;
	std::string type;
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
	std::string extension;
	uint tbloffset;
	uint numfiles;
	struct DRS_Table *fileinfo;
};

void ExtractDRSFile(const std::string &path);

#endif /* DRS_H */
