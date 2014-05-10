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

enum class SLPCmd {
	LESSER_BLOCK_COPY_1 = 0x00,
	LESSER_SKIP_1       = 0x01,
	GREATER_BLOCK_COPY  = 0x02,
	GREATER_SKIP        = 0x03,
	LESSER_BLOCK_COPY_2 = 0x04, ///< Another LBC.
	LESSER_SKIP_2       = 0x05, ///< Another LS.
	COPY_TRANSFORM      = 0x06,
	FILL                = 0x07,
	LESSER_BLOCK_COPY_3 = 0x08, ///< Another LBC.
	LESSER_SKIP_3       = 0x09, ///< Another LS.
	TRANSFORM           = 0x0A,
	SHADOW              = 0x0B,
	LESSER_BLOCK_COPY_4 = 0x0C, ///< Another LBC.
	LESSER_SKIP_4       = 0x0D, ///< Another LS.
	EXTENDED_COMMAND    = 0x0E,
	END_ROW             = 0x0F,
};

void ExtractSLPFile(const std::string &filename);

#endif /* SLP_H */
