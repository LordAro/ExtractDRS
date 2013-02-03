/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>

#include "slp.h"
#include "bmp.h"
#include "extractdrs.h"
#include "drs.h"

ushort vec2ushort(const vector<byte> vec, int offset) {
	return (vec[offset] << 0) + (vec[offset + 1] << 8);
}

uint GetCommand(uint number) {
	return number & 0xF;
}

void ExtractSLPFile(string filename) {
	std::cout << "Extracting " << filename << endl;

	const vector<byte> filedata = ReadFile(filename);
	if (filedata.empty() || filedata.size() < 64) {
		// (Header + 1 shapedata)
		std::cerr << "File is too small. Only " << filedata.size() << " bytes long." << endl;
		return;
	}
	vector<byte>::const_iterator p_filedata = filedata.begin();

	SLP_File slpfile;
	slpfile.header.version = string(p_filedata, p_filedata + 4);
	p_filedata += 4;
	slpfile.header.num_shapes = vec2uint(filedata, p_filedata - filedata.begin());
	p_filedata += 4;
	slpfile.header.comment = string(p_filedata, p_filedata + 24);

//	cout << "Version: " << slpfile.header.version << endl;
//	cout << "Num shapes: " << slpfile.header.num_shapes << endl;
//	cout << "Comment: " << slpfile.header.comment << endl;

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

//		cout << "Data Offsets: " << slpfile.shape[i].info.data_offset << endl;
//		cout << "Outline Offset: " << slpfile.shape[i].info.outline_offset << endl;
		// Palette offset is 0 for all drs files
//		cout << "Palette Offset: " << slpfile.shape[i].info.palette_offset << endl;

		// Properties = 0, 8, 16 or 24
//		cout << "Properties: " << slpfile.shape[i].info.properties << endl;
//		cout << "Width: " << slpfile.shape[i].info.width << endl;
//		cout << "Height: " << slpfile.shape[i].info.height << endl;

		// Note: hotspot_x/hotspot_y could be outside the width/height
//		cout << "X Hotspot: " << slpfile.shape[i].info.hotspot_x << endl;
//		cout << "Y Hotspot: " << slpfile.shape[i].info.hotspot_y << endl;

		/* For each row in the shape */
		slpfile.shape[i].row = new SLP_Row[slpfile.shape[i].info.height];
		for (int j = 0; j < slpfile.shape[i].info.height; j++) {
			cout << "Scanning line " << j + 1 << " of " <<  slpfile.shape[i].info.height << endl;

			/* Get outline data for each line */
			p_filedata = filedata.begin() + slpfile.shape[i].info.outline_offset + (4 * j);

			slpfile.shape[i].row[j].left = vec2ushort(filedata, p_filedata - filedata.begin());
			slpfile.shape[i].row[j].right = vec2ushort(filedata, p_filedata - filedata.begin() + 2);

			p_filedata =  filedata.begin() + slpfile.shape[i].info.data_offset + (4 * j);
			slpfile.shape[i].row[j].datastart = vec2uint(filedata, p_filedata - filedata.begin());

			uint curpos = slpfile.shape[i].row[j].datastart;
			uint command = 0x0F;
			byte curbyte = 0;

			slpfile.shape[i].row[j].pixel = new byte[slpfile.shape[i].info.width]();

			/* Leave a line blank */
			if (slpfile.shape[i].row[j].left == 0x8000) continue;

			int curpixelpos = slpfile.shape[i].row[j].left;
			do {
				/*
				 * Used for render hints from extended commands
				 * Note: Out of sync for the first byte, but irrelevant
				 */
				byte prevbyte = curbyte;
				uint length = 0;

				byte curbyte = filedata[curpos];
				command = GetCommand(curbyte);
				switch(command) {
					case CMD_Lesser_Block_Copy:
					case 4:
					case 8:
					case 0x0C:
						length = curbyte >> 2;
//						cout << "\tCommand: " << command << ':' << length << endl;
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
//						cout << "\tCommand: " << command << ':' << length << endl;
						curpixelpos += length;
						break;

					case CMD_Greater_Block_Copy:
						length = ((curbyte & 0xF0) << 4) + filedata[curpos + 1];
						for (uint it = 0; it < length; it++) {
							slpfile.shape[i].row[j].pixel[curpixelpos + it] = filedata[curpos + 2 + it];
						}
						curpixelpos += length;
						curpos += length + 1;
//						cout << "\tCommand: " << command << ':' << length << endl;
						break;

					case CMD_Greater_Skip:
						length = ((curbyte & 0xF0) << 4) + filedata[curpos + 1];
						curpixelpos += length + 1;
						curpos++;
//						cout << "\tCommand: " << command << ':' << length << endl;
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
//						cout << "\tCommand: " << command << ':' << length << endl;
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
//						cout << "\tCommand: " << command << ':' << length << endl;
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
//						cout << "\tCommand: " << command << ':' << length << endl;
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
//						cout << "\tCommand: " << command << ':' << length << endl;
						break;

					case CMD_Extended_Command:
//						cout << "\tCommand: " << command << ":Extended Command:" << (uint)curbyte << endl;
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
//								cout << "\t\tLength: " << length << '\n';
								curpixelpos += length;
								curpos++;
								break;

							default:
								cout << "CMD_What? " << command << endl;
								/* Debug, as this shouldn't happen */
								cout << filename << '\n';
								string s;
								getline(std::cin, s);
								break;
						}
						break;

					case CMD_End_Row:
//						cout << "\tCommand: 15:End of row" << endl;
						break;

					default:
						cout << "CMD_What? " << command << endl;
						/* Debug, as this shouldn't happen */
						cout << filename << endl;
						string s;
						getline(std::cin, s);
						break;
				}
				curpos++;
			} while (command != 0x0F);

		}

		/* @todo Tidy this mess up */
		string filedir = filename.substr(0, filename.rfind(PATHSEP) + 1);
		std::stringstream bmpfilename;
		bmpfilename << filedir;
		bmpfilename << "slpextracted/";
		FioCreateDirectory(bmpfilename.str().c_str());
		bmpfilename << filename.substr(filename.rfind(PATHSEP) + 1, filename.length() - (filename.rfind(PATHSEP) + 1) - 4);
		bmpfilename << '-';
		bmpfilename << i;
		bmpfilename << ".bmp";
		if (CreateBMP(bmpfilename.str(), &slpfile.shape[i])) {
//			cout << "Saved BMP to: " << bmpfilename.str() << endl;
		}

		for (int j = 0; j < slpfile.shape[i].info.height; j++) {
			delete[] slpfile.shape[i].row[j].pixel;
		}
		delete[] slpfile.shape[i].row;

//		cout << endl;
	}

	delete[] slpfile.shape;
}


