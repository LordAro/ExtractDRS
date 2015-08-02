/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <iostream>
#include <sstream>

#include "bmp.h"
#include "filereader.h"
#include "slp.h"

static const uint16_t EMPTY_ROW = 0x8000;

SLPShape SLPFile::ReadShapeInfo(BinaryFileReader &bfr)
{
	SLPShape ss;
	ss.data_offset    = bfr.ReadNum<uint32_t>();
	ss.outline_offset = bfr.ReadNum<uint32_t>();
	ss.palette_offset = bfr.ReadNum<uint32_t>();
	ss.properties     = bfr.ReadNum<uint32_t>();
	ss.width          = bfr.ReadNum<uint32_t>();
	ss.height         = bfr.ReadNum<uint32_t>();
	ss.hotspot_x      = bfr.ReadNum< int32_t>();
	ss.hotspot_y      = bfr.ReadNum< int32_t>();
	return ss;
}

SLPRow SLPFile::ReadRowOutlineOffsets(BinaryFileReader &bfr)
{
	SLPRow sr;
	sr.left  = bfr.ReadNum<uint16_t>();
	sr.right = bfr.ReadNum<uint16_t>();
	return sr;
}

inline SLPCmd GetCommand(uint8_t byte)
{
	return static_cast<SLPCmd>(byte & 0x0F);
}

inline uint32_t GetTop6Bits(uint8_t byte)
{
	return (byte & 0xFC) >> 2;
}

inline uint32_t Get4BitsAndNext(uint8_t byte1, uint8_t byte2)
{
	return ((byte1 & 0xF0u) << 4u) + byte2;
}

inline uint32_t GetTopNibbleOrNext(uint8_t byte, BinaryFileReader& bfr)
{
	uint length = (byte & 0xF0) >> 4;
	if (length == 0) length = bfr.ReadNum<uint8_t>();
	return length;
}

std::vector<uint8_t> SLPFile::ReadRowData(BinaryFileReader &bfr, uint32_t width, uint16_t left, uint16_t right)
{
	std::vector<uint8_t> pixels(width); // Init with all zeros
	if (left == EMPTY_ROW) {
		bfr.SkipBytes(1); // "Read" the byte regardless
		return pixels;
	}

	SLPCmd command = SLPCmd::END_ROW;
	uint8_t curr_byte = 0;

	uint cur_pixel_pos = left;
	do {
		/* Used for render hints from extended commands
		 * Note: Out of sync for the first byte, but irrelevant */
//		uint8 prev_byte = curr_byte;
		uint length = 0;

		curr_byte = bfr.ReadNum<uint8_t>();
		command = GetCommand(curr_byte);
		switch(command) {
			case SLPCmd::LESSER_BLOCK_COPY_1:
			case SLPCmd::LESSER_BLOCK_COPY_2:
			case SLPCmd::LESSER_BLOCK_COPY_3:
			case SLPCmd::LESSER_BLOCK_COPY_4:
				length = GetTop6Bits(curr_byte);

				for (uint32_t it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = bfr.ReadNum<uint8_t>();
				}
				break;

			case SLPCmd::LESSER_SKIP_1:
			case SLPCmd::LESSER_SKIP_2:
			case SLPCmd::LESSER_SKIP_3:
			case SLPCmd::LESSER_SKIP_4:
				length = GetTop6Bits(curr_byte);
				cur_pixel_pos += length;
				break;

			case SLPCmd::GREATER_BLOCK_COPY:
				length = Get4BitsAndNext(curr_byte, bfr.ReadNum<uint8_t>());
				for (uint32_t it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = bfr.ReadNum<uint8_t>();
				}
				break;

			case SLPCmd::GREATER_SKIP:
				length = Get4BitsAndNext(curr_byte, bfr.ReadNum<uint8_t>());
				cur_pixel_pos += length;
				break;

			case SLPCmd::COPY_TRANSFORM: {
				length = GetTopNibbleOrNext(curr_byte, bfr);

				/*
				 * Player colours start at palette index i*16, where 1 <= i <= 9 (8 players)
				 * They have 8 variants on the same palette 'line'.
				 * The player index is currently hardcoded to 1 (red)
				 */
				uint8_t player = 1;
				for (uint32_t it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = bfr.ReadNum<uint8_t>() + (1 + player) * 16;
				}
				break;
			}

			case SLPCmd::FILL: {
				length = GetTopNibbleOrNext(curr_byte, bfr);

				uint8_t fill_col = bfr.ReadNum<uint8_t>();
				for (uint32_t it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = fill_col;
				}
				break;
			}

			case SLPCmd::PLAYER_FILL: {
				length = GetTopNibbleOrNext(curr_byte, bfr);

				/* See SLPCmd::COPY_TRANSFORM */
				uint8_t col = bfr.ReadNum<uint8_t>() + (1 + 1) * 16;
				for (uint32_t it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = col;
				}
				break;
			}

			case SLPCmd::SHADOW:
				length = GetTopNibbleOrNext(curr_byte, bfr);

				for (uint it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = 56;
				}
				break;

			case SLPCmd::EXTENDED_COMMAND:
				// Uses whole byte
				switch(static_cast<SLPExCmd>(curr_byte)) {
					case SLPExCmd::X_FLIP_1:
					case SLPExCmd::X_FLIP_2:
						std::cerr << "Warning: " << bfr.GetFilepath() << ": SLPExCmd::X_FLIP_* commands not fully implemented" << std::endl;
						/* @todo implement */
						break;

					case SLPExCmd::SET_NORMAL_TRANSFORM:
					case SLPExCmd::SET_ALTERN_TRANSFORM:
						std::cerr << "Warning: " << bfr.GetFilepath() << ": SLPExCmd::SET_*_TRANSFORM commands not fully implemented" << std::endl;
						/* @todo implement */
						break;

					case SLPExCmd::DRAW_SPECIAL_COL_1:
					case SLPExCmd::DRAW_SPECIAL_COL_2:
						pixels.at(cur_pixel_pos++) = (static_cast<SLPExCmd>(curr_byte) == SLPExCmd::DRAW_SPECIAL_COL_1) ? 242 : 0;
						break;

					case SLPExCmd::DRAW_SPECIAL_COL_RUN_1:
					case SLPExCmd::DRAW_SPECIAL_COL_RUN_2:
						length = bfr.ReadNum<uint8_t>();

						for (uint8_t it = 0; it < length; it++) {
							pixels.at(cur_pixel_pos++) = (static_cast<SLPExCmd>(curr_byte) == SLPExCmd::DRAW_SPECIAL_COL_RUN_1) ? 242 : 0;
						}
						break;

					default:
						std::cerr << "SLPExCmd::What? " << static_cast<uint32_t>(command) << std::endl;
						exit(1); // TODO: Handle better
				}
				break;

			case SLPCmd::END_ROW:
				break;

			default:
				std::cerr << "SLPCmd::What? " << static_cast<uint8_t>(command) << std::endl;
				exit(1); // TODO: Handle better
		}

	} while (command != SLPCmd::END_ROW);

	assert(cur_pixel_pos + right == width);

	return pixels;
}

void ExtractSLPFile(const std::string &filename)
{
	SLPFile slpfile;
	BinaryFileReader binfile(filename);
	std::cout << "Extracting " << filename << std::endl;

	/* Get the file id */
	std::string idstr = filename.substr(filename.rfind(PATHSEP) + 1, filename.rfind('.') - filename.rfind(PATHSEP) - 1);
	slpfile.id = std::stoi(idstr);

	if (binfile.GetRemaining() < 64) {
		std::cerr << "File is too small. Only " << binfile.GetRemaining() << " bytes long." << std::endl;
		return;
	}

	slpfile.version    = binfile.ReadString( 4);
	slpfile.num_shapes = binfile.ReadNum<uint32_t>();
	slpfile.comment    = binfile.ReadString(24);

	for (uint32_t i = 0; i < slpfile.num_shapes; i++) {
		slpfile.shapes.push_back(slpfile.ReadShapeInfo(binfile));
	}

	for (uint32_t i = 0; i < slpfile.num_shapes; i++) {
		/* Get the outline offsets */
		assert(binfile.GetPosition() == slpfile.shapes.at(i).outline_offset);
		for (uint32_t j = 0; j < slpfile.shapes.at(i).height; j++) {
			slpfile.shapes.at(i).rows.push_back(slpfile.ReadRowOutlineOffsets(binfile));
		}

		/* Then get the data offsets */
		assert(binfile.GetPosition() == slpfile.shapes.at(i).data_offset);
		for (uint32_t j = 0; j < slpfile.shapes.at(i).height; j++) {
			slpfile.shapes.at(i).rows.at(j).data_start = binfile.ReadNum<uint32_t>();
		}

		/* Finally, actually read the data. Silly data format. */
		for (uint32_t j = 0; j < slpfile.shapes.at(i).height; j++) {
			assert(binfile.GetPosition() == slpfile.shapes.at(i).rows.at(j).data_start);
			uint16_t left  = slpfile.shapes.at(i).rows.at(j).left;
			uint16_t right = slpfile.shapes.at(i).rows.at(j).right;
			std::vector<uint8_t> pix = slpfile.ReadRowData(binfile, slpfile.shapes.at(i).width, left, right);
			slpfile.shapes.at(i).rows.at(j).pixels = pix;
		}

		std::string filedir = filename.substr(0, filename.rfind(PATHSEP) + 1);
		std::string bmpfilepath = filedir + "slpextracted/";
		GenCreateDirectory(bmpfilepath);

		std::string bmpfilename = std::to_string(slpfile.id);
		bmpfilename += '-' + std::to_string(i) + ".bmp";

		std::string fullpath = bmpfilepath + bmpfilename;
		CreateBMP(fullpath, slpfile.shapes.at(i));
	}
}
