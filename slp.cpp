/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

 #include "slp.h"
 #include "extractdrs.h"

ushort str2ushort(const string &str, int offset)
{
	return ((byte)str[offset] << 0) + ((byte)str[offset + 1] << 8);
}

uint GetCommand(uint number)
{
	return number & 0xF;
}

void ExtractSLPFile(string filename)
{
	cout << "Extracting " << filename << '\n';
	string filedata = ReadFile(filename);

	string slpheader = filedata.substr(0, 4 + 4 + 24);
	SLP_File slpfile;
	slpfile.header.version = slpheader.substr(0, 4);
	slpfile.header.num_shapes = str2uint(slpheader, 4);
	slpfile.header.comment = slpheader.substr(8, 24);

//	cout << "Version: " << slpfile.header.version << '\n';
	cout << "Num shapes: " << slpfile.header.num_shapes << '\n';
//	cout << "Comment: " << slpfile.header.comment << '\n';

	/* For each shape */
	slpfile.shape = new SLP_Shape[slpfile.header.num_shapes];
	for (uint i = 0; i < slpfile.header.num_shapes; i++) {
		cout << "Shape " << i + 1 << '\n';
		string shapedata = filedata.substr(32 + (32 * i), 32);

		slpfile.shape[i].info.data_offset = str2uint(shapedata, 0);
		slpfile.shape[i].info.outline_offset = str2uint(shapedata, 4);
		slpfile.shape[i].info.palette_offset = str2uint(shapedata, 8);
		slpfile.shape[i].info.properties = str2uint(shapedata, 12);
		slpfile.shape[i].info.width = str2uint(shapedata, 16);
		slpfile.shape[i].info.height = str2uint(shapedata, 20);
		slpfile.shape[i].info.hotspot_x = str2uint(shapedata, 24);
		slpfile.shape[i].info.hotspot_y = str2uint(shapedata, 28);

		cout << "Data Offsets: " << slpfile.shape[i].info.data_offset << '\n';
		cout << "Outline Offset: " << slpfile.shape[i].info.outline_offset << '\n';
		// Palette offset is 0 for all drs files
//		cout << "Palette Offset: " << slpfile.shape[i].info.palette_offset << '\n';

		// Properties = 0, 8, 16 or 24
		cout << "Properties: " << slpfile.shape[i].info.properties << '\n';
		cout << "Width: " << slpfile.shape[i].info.width << '\n';
		cout << "Height: " << slpfile.shape[i].info.height << '\n';

		// Note: hotspot_x/hotspot_y could be outside the width/height
		cout << "X Hotspot: " << slpfile.shape[i].info.hotspot_x << '\n';
		cout << "Y Hotspot: " << slpfile.shape[i].info.hotspot_y << '\n';

		/* For each row in the shape */
		slpfile.shape[i].row = new SLP_Row[slpfile.shape[i].info.height];
		for (int j = 0; j < slpfile.shape[i].info.height; j++) {
			cout << "Scanning line " << j + 1 << " of " <<  slpfile.shape[i].info.height << '\n';
			/* Get outline data for each line */
			string outlinedata = filedata.substr(slpfile.shape[i].info.outline_offset + (4 * j), 4);

			slpfile.shape[i].row[j].left = str2ushort(outlinedata, 0);
			slpfile.shape[i].row[j].right = str2ushort(outlinedata, 2);

			string datapointer = filedata.substr(slpfile.shape[i].info.data_offset + (4 * j), 4);
			slpfile.shape[i].row[j].datastart = str2uint(datapointer, 0);

			/* Casting magics */
			uint startbyte = (byte)filedata.substr(slpfile.shape[i].row[j].datastart, 1)[0];
			uint curpos = slpfile.shape[i].row[j].datastart;
			cout << "Startbyte: " << startbyte << '\n';
			cout << "File len: " << filedata.length() << '\n';
			cout << "Datastart: " << slpfile.shape[i].row[j].datastart << '\n';
			uint command = 0x0F;
			do {
				uint curbyte = (byte)filedata.substr(curpos, 1)[0];
				command = GetCommand(curbyte);
				cout << "\tCommand: " << command << '\n';
				switch (command) {
					case CMD_Colour_List:
					case 4:
					case 8:
					case 0x0C:

					case CMD_Skip:
					case 5:
					case 9:
					case 0x0D:
					case CMD_Big_Colour_List:
					case CMD_Big_Skip:

					case CMD_Player_Colour_List:
					case CMD_Fill:

					case CMD_Player_Colour_Fill:
					case CMD_Shadow_Transparent:

					case CMD_Shadow_Player:
					case CMD_End_Row:

					case CMD_Outline:

					case CMD_Outline_Span:
						break;
					default:
						cout << "CMD_What? " << command << '\n';
						/* Get user input, as this shouldn't happen */
						string s;
						getline(cin, s);
						break;
				}
				curpos++;
			} while (command != 0x0F);
		}

		cout << '\n';
	//		slpfile.shaperows[j].pixels = new SLP_Pixel[slpfile.shapeinfos[i].width];
	}
}
