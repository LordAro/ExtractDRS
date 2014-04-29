/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>

#include "bmp.h"
#include "slp.h"

uint16 vec2uint16(const std::vector<uint8> &vec, int offset)
{
	return (vec[offset] << 0) + (vec[offset + 1] << 8);
}

uint GetCommand(uint number)
{
	return number & 0xF;
}

void ExtractSLPFile(std::string filename)
{
	SLP_File slpfile;
	std::cout << "Extracting " << filename << std::endl;

	/* Get the file id */
	std::string idstr = filename.substr(filename.rfind(PATHSEP) + 1, filename.rfind('.') - filename.rfind(PATHSEP) - 1);
	std::stringstream(idstr) >> slpfile.id;

	const std::vector<uint8> filedata = ReadFile(filename);
	if (filedata.empty() || filedata.size() < 64) {
		// (Header + 1 shapedata)
		std::cerr << "File is too small. Only " << filedata.size() << " bytes long." << std::endl;
		return;
	}
	std::vector<uint8>::const_iterator p_filedata = filedata.begin();

	slpfile.header.version = std::string(p_filedata, p_filedata + 4);
	p_filedata += 4;
	slpfile.header.num_shapes = vec2uint(filedata, p_filedata - filedata.begin());
	p_filedata += 4;
	slpfile.header.comment = std::string(p_filedata, p_filedata + 24);

//	std::cout << "Version: " << slpfile.header.version << std::endl;
//	std::cout << "Num shapes: " << slpfile.header.num_shapes << std::endl;
//	std::cout << "Comment: " << slpfile.header.comment << std::endl;

	if (slpfile.header.num_shapes == 0) return;
	/* For each shape */
	slpfile.shape = new SLP_Shape[slpfile.header.num_shapes];
	for (uint i = 0; i < slpfile.header.num_shapes; i++) {
//		cout << "Shape " << i + 1 << endl;
		p_filedata = filedata.begin() + 32 + (32 * i);

		slpfile.shape[i].info.data_offset    = vec2uint(filedata, p_filedata - filedata.begin());
		slpfile.shape[i].info.outline_offset = vec2uint(filedata, p_filedata - filedata.begin() + 4);
		slpfile.shape[i].info.palette_offset = vec2uint(filedata, p_filedata - filedata.begin() + 8);
		slpfile.shape[i].info.properties     = vec2uint(filedata, p_filedata - filedata.begin() + 12);
		slpfile.shape[i].info.width          = vec2uint(filedata, p_filedata - filedata.begin() + 16);
		slpfile.shape[i].info.height         = vec2uint(filedata, p_filedata - filedata.begin() + 20);
		slpfile.shape[i].info.hotspot_x      = vec2uint(filedata, p_filedata - filedata.begin() + 24);
		slpfile.shape[i].info.hotspot_y      = vec2uint(filedata, p_filedata - filedata.begin() + 28);

		if (slpfile.shape[i].info.data_offset > filedata.size() || slpfile.shape[i].info.outline_offset > filedata.size()) return;

//		std::cout << "Data Offsets: " << slpfile.shape[i].info.data_offset << std::endl;
//		std::cout << "Outline Offset: " << slpfile.shape[i].info.outline_offset << std::endl;
		// Palette offset is 0 for all drs files
//		std::cout << "Palette Offset: " << slpfile.shape[i].info.palette_offset << std::endl;

		// Properties = 0, 8, 16 or 24
//		std::cout << "Properties: " << slpfile.shape[i].info.properties << std::endl;
//		std::cout << "Width: " << slpfile.shape[i].info.width << std::endl;
//		std::cout << "Height: " << slpfile.shape[i].info.height << std::endl;

		// Note: hotspot_x/hotspot_y could be outside the width/height
//		std::cout << "X Hotspot: " << slpfile.shape[i].info.hotspot_x << std::endl;
//		std::cout << "Y Hotspot: " << slpfile.shape[i].info.hotspot_y << std::endl;

		if (slpfile.shape[i].info.height == 0 || slpfile.shape[i].info.width == 0) return;
		/* For each row in the shape */
		slpfile.shape[i].row = new SLP_Row[slpfile.shape[i].info.height];
		for (int j = 0; j < slpfile.shape[i].info.height; j++) {
			std::cout << "Scanning line " << j + 1 << " of " <<  slpfile.shape[i].info.height << std::endl;

			/* Get outline data for each line */
			p_filedata = filedata.begin() + slpfile.shape[i].info.outline_offset + (4 * j);

			slpfile.shape[i].row[j].left = vec2uint16(filedata, p_filedata - filedata.begin());
			slpfile.shape[i].row[j].right = vec2uint16(filedata, p_filedata - filedata.begin() + 2);

			p_filedata =  filedata.begin() + slpfile.shape[i].info.data_offset + (4 * j);
			slpfile.shape[i].row[j].datastart = vec2uint(filedata, p_filedata - filedata.begin());

			uint curpos = slpfile.shape[i].row[j].datastart;
			uint command = 0x0F;
			uint8 curbyte = 0;

			slpfile.shape[i].row[j].pixel = new uint8[slpfile.shape[i].info.width]();

			/* Leave a line blank */
			if (slpfile.shape[i].row[j].left == 0x8000) continue;

			int curpixelpos = slpfile.shape[i].row[j].left;
			do {
				/* Used for render hints from extended commands
				 * Note: Out of sync for the first byte, but irrelevant */
//				uint8 prevbyte = curbyte;
				uint length = 0;

				curbyte = filedata[curpos];
				command = GetCommand(curbyte);
				switch(command) {
					case CMD_Lesser_Block_Copy:
					case 4:
					case 8:
					case 0x0C:
						length = curbyte >> 2;
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						for (uint it = 0; it < length; it++) {
							slpfile.shape[i].row[j].pixel[curpixelpos + it] = filedata[curpos + 1 + it];
						}
						curpos += length;
						curpixelpos += length;
						break;

					case CMD_Lesser_Skip:
					case 5:
					case 9:
					case 0x0D:
						length = (curbyte & 0xFC) >> 2;
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						curpixelpos += length;
						break;

					case CMD_Greater_Block_Copy:
						length = ((curbyte & 0xF0) << 4) + filedata[curpos + 1];
						for (uint it = 0; it < length; it++) {
							slpfile.shape[i].row[j].pixel[curpixelpos + it] = filedata[curpos + 2 + it];
						}
						curpixelpos += length;
						curpos += length + 1;
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						break;

					case CMD_Greater_Skip:
						length = ((curbyte & 0xF0) << 4) + filedata[curpos + 1];
						curpixelpos += length;
						curpos++;
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						break;

					case CMD_Copy_Transform:
						length = (curbyte & 0xF0) >> 4; // high nibble
						if (length == 0) {
							length = filedata[curpos + 1];
							curpos++;
						}
						for (uint it = 0; it < length; it++) {
							/* @todo player colours */
							slpfile.shape[i].row[j].pixel[curpixelpos + it] = filedata[curpos + it];
						}
						curpixelpos += length;
						curpos += length;
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						break;

					case CMD_Fill:
						length = (curbyte & 0xF0) >> 4;
						if (length == 0) {
							length = filedata[curpos + 1];
							curpos++;
						}
						for (uint it = 0; it < length; it++) {
							slpfile.shape[i].row[j].pixel[curpixelpos + it] = filedata[curpos + 1];
						}
						curpixelpos += length;
						curpos++; // colour at this byte
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						break;

					case CMD_Transform:
						length = (curbyte & 0xF0) >> 4;
						if (length == 0) {
							length = filedata[curpos + 1];
							curpos++;
						}
						for (uint it = 0; it < length; it++) {
							/* @todo something...? */
							slpfile.shape[i].row[j].pixel[curpixelpos + it] = filedata[curpos + 1];
						}
						curpixelpos += length;
						curpos++;
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						break;

					case CMD_Shadow:
						length = (curbyte & 0xF0) >> 4;
						if (length == 0) {
							length = filedata[curpos + 1];
							curpos++;
						}
						for (uint it = 0; it < length; it++) {
							slpfile.shape[i].row[j].pixel[curpixelpos + it] = 56;
						}
						curpixelpos += length;
//						std::cout << "\tCommand: " << command << ':' << length << std::endl;
						break;

					case CMD_Extended_Command:
//						std::cout << "\tCommand: " << command << ":Extended Command:" << (uint)curbyte << std::endl;
						// Uses whole byte
						switch(curbyte) {
							case 0x0E: // x-flip next command's bytes
							case 0x1E:
								/* @todo implement */
								break;

							case 0x2E: // set transform colour
							case 0x3E:
								/* @todo implement */
								break;

							case 0x4E: // Draw 'special colour 1', for 1 byte
							case 0x6E: // Draw 'special colour 2', for 1 byte
								slpfile.shape[i].row[j].pixel[curpixelpos] = (curbyte == 0x4E) ? 242 : 0;
								curpixelpos++;
								break;

							case 0x5E: // Draw 'special colour 1', for (curbyte + 1) bytes
							case 0x7E: // Draw 'special colour 2', for (curbyte + 1) bytes
								length = filedata[curpos + 1];

								for (uint it = 0; it < length; it++) {
									slpfile.shape[i].row[j].pixel[curpixelpos + it] = (curbyte == 0x5E) ? 242 : 0;
								}
//								std::cout << "\t\tLength: " << length << '\n';
								curpixelpos += length;
								curpos++;
								break;

							default:
								std::cout << "CMD_What? " << command << std::endl;
								/* Debug, as this shouldn't happen */
								std::cout << filename << '\n';
								std::string s;
								getline(std::cin, s);
								return;
						}
						break;

					case CMD_End_Row:
//						std::cout << "\tCommand: 15:End of row" << std::endl;
						break;

					default:
						std::cout << "CMD_What? " << command << std::endl;
						/* Debug, as this shouldn't happen */
						std::cout << filename << std::endl;
						std::string s;
						getline(std::cin, s);
						return;
				}
				curpos++;
			} while (command != 0x0F);

		}

		/* @todo Tidy this mess up */
		std::string filedir = filename.substr(0, filename.rfind(PATHSEP) + 1);
		std::stringstream bmpfilename;
		bmpfilename << filedir;
		bmpfilename << "slpextracted/";
		GenCreateDirectory(bmpfilename.str());
		bmpfilename << slpfile.id;
		bmpfilename << '-';
		bmpfilename << i;
		bmpfilename << ".bmp";
		if (CreateBMP(bmpfilename.str(), &slpfile.shape[i])) {
//			std::cout << "Saved BMP to: " << bmpfilename.str() << std::endl;
		}

		for (int j = 0; j < slpfile.shape[i].info.height; j++) {
			delete[] slpfile.shape[i].row[j].pixel;
		}
		delete[] slpfile.shape[i].row;

//		std::cout << std::endl;
	}

	delete[] slpfile.shape;
}


