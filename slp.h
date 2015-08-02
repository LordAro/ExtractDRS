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
	PLAYER_FILL         = 0x0A,
	SHADOW              = 0x0B,
	LESSER_BLOCK_COPY_4 = 0x0C, ///< Another LBC.
	LESSER_SKIP_4       = 0x0D, ///< Another LS.
	EXTENDED_COMMAND    = 0x0E,
	END_ROW             = 0x0F,
};

enum class SLPExCmd {
	X_FLIP_1               = 0x0E,
	X_FLIP_2               = 0x1E,
	SET_NORMAL_TRANSFORM   = 0x2E,
	SET_ALTERN_TRANSFORM   = 0x3E,
	DRAW_SPECIAL_COL_1     = 0x4E,
	DRAW_SPECIAL_COL_RUN_1 = 0x5E,
	DRAW_SPECIAL_COL_2     = 0x6E,
	DRAW_SPECIAL_COL_RUN_2 = 0x7E,
};

struct SLPRow {
	std::vector<uint8_t> pixels; // 8bpp
	uint16_t left;
	uint16_t right;
	uint32_t data_start; // Position pointer to the start of the actual data
};

struct SLPShape {
	/* Info block */
	uint32_t data_offset;
	uint32_t outline_offset;
	uint32_t palette_offset;
	uint32_t properties;
	uint32_t width;
	uint32_t height;
	int32_t hotspot_x;
	int32_t hotspot_y;

	std::vector<SLPRow> rows;
};

class SLPFile {
public:
	SLPShape ReadShapeInfo(BinaryFileReader &bfr);
	SLPRow ReadRowOutlineOffsets(BinaryFileReader &bfr);
	std::vector<uint8_t> ReadRowData(BinaryFileReader &bfr, uint32_t width, uint16_t left, uint16_t right);

	int id;

	/* Header */
	std::string version;
	uint32_t num_shapes;
	std::string comment;

	std::vector<SLPShape> shapes;
};

void ExtractSLPFile(const std::string &filename);

#endif /* SLP_H */
