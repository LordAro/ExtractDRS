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
#include <vector>

#include "extractdrs.h"
#include "filereader.h"
#include "slp.h"

static const uint8 HEADER_SIZE = 64;
static const uint8 TABLE_SIZE = 12;

struct DRSTable {
	int file_id;
	int file_offset;
	int file_size;
};

struct DRSTableInfo {
	char character;
	std::string extension;
	int table_offset;
	int num_files;
	std::vector<DRSTable> file_infos;

};

class DRSFile {
public:
	void ReadHeader(BinaryFileReader &bfr);
	DRSTableInfo ReadTableInfo(BinaryFileReader &bfr);
	DRSTable ReadTable(BinaryFileReader &bfr);

	/* Header */
	std::string copyright;
	std::string version;
	std::string type;
	int num_tables;
	int first_offset;

	std::vector<DRSTableInfo> infos;
};

void ExtractDRSFile(const std::string &path);

#endif /* DRS_H */
