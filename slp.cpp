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

static const uint16 EMPTY_ROW = 0x8000;

SLPCommand GetCommand(uint8 byte)
{
	return static_cast<SLPCommand>(byte & 0x0F);
}

SLPShape SLPFile::ReadShapeInfo(BinaryFileReader &bfr)
{
	SLPShape ss;
	ss.data_offset    = bfr.ReadNum<uint>();
	ss.outline_offset = bfr.ReadNum<uint>();
	ss.palette_offset = bfr.ReadNum<uint>();
	ss.properties     = bfr.ReadNum<uint>();
	ss.width          = bfr.ReadNum< int>();
	ss.height         = bfr.ReadNum< int>();
	ss.hotspot_x      = bfr.ReadNum< int>();
	ss.hotspot_y      = bfr.ReadNum< int>();
	return ss;
}

SLPRow SLPFile::ReadRowOutlineOffsets(BinaryFileReader &bfr)
{
	SLPRow sr;
	sr.left  = bfr.ReadNum<uint16>();
	sr.right = bfr.ReadNum<uint16>();
	return sr;
}

std::vector<uint8> SLPFile::ReadRowData(BinaryFileReader &bfr, int width, uint16 left, uint16 right)
{
	std::vector<uint8> pixels(width); // Init with all zeros
	if (left == EMPTY_ROW) {
		bfr.SkipBytes(1); // "Read" the byte regardless
		return pixels;
	}

	SLPCommand command = CMD_End_Row;
	uint8 curr_byte = 0;

	uint cur_pixel_pos = left;
	do {
		/* Used for render hints from extended commands
		 * Note: Out of sync for the first byte, but irrelevant */
//		uint8 prev_byte = curr_byte;
		uint length = 0;

		curr_byte = bfr.ReadNum<uint8>();
		command = GetCommand(curr_byte);

		switch(command) {
			case CMD_Lesser_Block_Copy:
			case 0x04:
			case 0x08:
			case 0x0C:
				length = curr_byte >> 2;

				for (uint it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = bfr.ReadNum<uint8>();
				}
				break;

			case CMD_Lesser_Skip:
			case 0x05:
			case 0x09:
			case 0x0D:
				length = (curr_byte & 0xFC) >> 2;
				cur_pixel_pos += length;
				break;

			case CMD_Greater_Block_Copy:
				length = ((curr_byte & 0xF0) << 4) + bfr.ReadNum<uint8>();
				for (uint it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = bfr.ReadNum<uint8>();
				}
				break;

			case CMD_Greater_Skip:
				length = ((curr_byte & 0xF0) << 4) + bfr.ReadNum<uint8>();
				cur_pixel_pos += length;
				break;

			case CMD_Copy_Transform:
				length = (curr_byte & 0xF0) >> 4; // high nibble
				if (length == 0) length = bfr.ReadNum<uint8>();

				std::cerr << "Warning: CMD_Copy_Transform not fully implemented" << std::endl;
				for (uint it = 0; it < length; it++) {
					/* @todo player colours - some transform function is needed */
					pixels.at(cur_pixel_pos++) = bfr.ReadNum<uint8>();
				}
				break;

			case CMD_Fill: {
				length = (curr_byte & 0xF0) >> 4;
				if (length == 0) length = bfr.ReadNum<uint8>();

				uint8 fill_col = bfr.ReadNum<uint8>();
				for (uint it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = fill_col;
				}
				break;
			}

			case CMD_Transform: {
				length = (curr_byte & 0xF0) >> 4;
				if (length == 0) length = bfr.ReadNum<uint8>();

				std::cerr << "Warning: CMD_Transform not fully implemented" << std::endl;

				/* @todo something...? */
				uint8 col = bfr.ReadNum<uint8>();
				for (uint it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = col;
				}
				break;
			}

			case CMD_Shadow:
				length = (curr_byte & 0xF0) >> 4;
				if (length == 0) length = bfr.ReadNum<uint8>();

				for (uint it = 0; it < length; it++) {
					pixels.at(cur_pixel_pos++) = 56;
				}
				break;

			case CMD_Extended_Command:
				// Uses whole byte
				switch(curr_byte) {
					case 0x0E: // x-flip next command's bytes
					case 0x1E:
						std::cerr << "Warning: 0x0E, 0x1E commands not fully implemented" << std::endl;
						/* @todo implement */
						break;

					case 0x2E: // set transform colour
					case 0x3E:
						std::cerr << "Warning: 0x2E, 0x3E commands not fully implemented" << std::endl;
						/* @todo implement */
						break;

					case 0x4E: // Draw 'special colour 1', for 1 byte
					case 0x6E: // Draw 'special colour 2', for 1 byte
						pixels.at(cur_pixel_pos++) = (curr_byte == 0x4E) ? 242 : 0;
						break;

					case 0x5E: // Draw 'special colour 1', for (curr_byte + 1) bytes
					case 0x7E: // Draw 'special colour 2', for (curr_byte + 1) bytes
						length = bfr.ReadNum<uint8>();

						for (uint it = 0; it < length; it++) {
							pixels.at(cur_pixel_pos++) = (curr_byte == 0x5E) ? 242 : 0;
						}
						break;

					default:
						std::cerr << "CMD_Extended_Command_What? " << (uint)command << std::endl;
						exit(1); // TODO: Handle better
				}
				break;

			case CMD_End_Row:
				break;

			default:
				std::cerr << "CMD_What? " << (uint)command << std::endl;
				exit(1); // TODO: Handle better
		}

	} while (command != CMD_End_Row);

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
	slpfile.num_shapes = binfile.ReadNum<int>();
	slpfile.comment    = binfile.ReadString(24);

	for (int i = 0; i < slpfile.num_shapes; i++) {
		slpfile.shapes.push_back(slpfile.ReadShapeInfo(binfile));
	}

	for (int i = 0; i < slpfile.num_shapes; i++) {
		/* Get the outline offsets */
		assert(binfile.GetPosition() == slpfile.shapes[i].outline_offset);
		for (int j = 0; j < slpfile.shapes[i].height; j++) {
			slpfile.shapes[i].rows.push_back(slpfile.ReadRowOutlineOffsets(binfile));
		}

		/* Then get the data offsets */
		assert(binfile.GetPosition() == slpfile.shapes[i].data_offset);
		for (int j = 0; j < slpfile.shapes[i].height; j++) {
			slpfile.shapes[i].rows[j].data_start = binfile.ReadNum<uint>();
		}

		/* Finally, actually read the data. Silly data format. */
		for (int j = 0; j < slpfile.shapes[i].height; j++) {
			assert(binfile.GetPosition() == slpfile.shapes[i].rows[j].data_start);
			uint16 left  = slpfile.shapes[i].rows[j].left;
			uint16 right = slpfile.shapes[i].rows[j].right;
			std::vector<uint8> pix = slpfile.ReadRowData(binfile, slpfile.shapes[i].width, left, right);
			slpfile.shapes[i].rows[j].pixels = pix;
		}

		std::string filedir = filename.substr(0, filename.rfind(PATHSEP) + 1);
		std::string bmpfilepath = filedir + "slpextracted/";
		GenCreateDirectory(bmpfilepath);

		std::string bmpfilename = std::to_string(slpfile.id);
		bmpfilename += '-' + std::to_string(i) + ".bmp";

		std::string fullpath = bmpfilepath + bmpfilename;
		CreateBMP(fullpath, &slpfile.shapes[i]);
	}
}


