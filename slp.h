/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SLP_H
#define SLP_H

#include <vector>

#include "extractdrs.h"
#include "filereader.h"

struct SLPRow {
	std::vector<uint8> pixels; // 8bpp
	uint16 left;
	uint16 right;
	uint data_start; // Position pointer to the start of the actual data
};

struct SLPShape {
	/* Info block */
	uint data_offset;
	uint outline_offset;
	uint palette_offset;
	uint properties;
	int width;
	int height;
	int hotspot_x;
	int hotspot_y;

	std::vector<SLPRow> rows;
};

class SLPFile {
public:
	SLPShape ReadShapeInfo(BinaryFileReader &bfr);
	SLPRow ReadRowOutlineOffsets(BinaryFileReader &bfr);
	std::vector<uint8> ReadRowData(BinaryFileReader &bfr, int width, uint16 left, uint16 right);

	int id;

	/* Header */
	std::string version;
	int num_shapes;
	std::string comment;

	std::vector<SLPShape> shapes;
};

enum SLPCommand {
	CMD_Lesser_Block_Copy  = 0x00, // also 4, 8, 0x0c
	CMD_Lesser_Skip        = 0x01, // also 5, 9, 0x0d
	CMD_Greater_Block_Copy = 0x02,
	CMD_Greater_Skip       = 0x03,

	CMD_Copy_Transform     = 0x06,
	CMD_Fill               = 0x07,

	CMD_Transform          = 0x0A,
	CMD_Shadow             = 0x0B,

	CMD_Extended_Command   = 0x0E,
	CMD_End_Row            = 0x0F,
};

void ExtractSLPFile(const std::string &filename);

#endif /* SLP_H */
