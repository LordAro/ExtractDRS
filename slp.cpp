/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

 #include "slp.h"
 #include "extractdrs.h"

 void ExtractSLPFile(string filename)
 {
	cout << "Extracting " << filename << '\n';
	string filedata = ReadFile(filename);

	string slpheader = filedata.substr(0, 4 + 4 + 24);
	SLP_File slpfile;
	slpfile.version = slpheader.substr(0, 4);
	slpfile.num_shapes = str2uint(slpheader, 4);
	slpfile.comment = slpheader.substr(8, 24);

//	cout << "Version: " << slpfile.version << '\n';
	cout << "Num shapes: " << slpfile.num_shapes << '\n';
//	cout << "Comment: " << slpfile.comment << '\n';

	slpfile.shapeinfos = new SLP_Info[slpfile.num_shapes];
	for (uint i = 0; i < slpfile.num_shapes; i++) {
		string shapedata = filedata.substr(32 + (32 * i), 32);

		slpfile.shapeinfos[i].data_offsets = str2uint(shapedata, 0);
		slpfile.shapeinfos[i].outline_offset = str2uint(shapedata, 4);
		slpfile.shapeinfos[i].palette_offset = str2uint(shapedata, 8);
		slpfile.shapeinfos[i].properties = str2uint(shapedata, 12);
		slpfile.shapeinfos[i].width = str2uint(shapedata, 16);
		slpfile.shapeinfos[i].height = str2uint(shapedata, 20);
		slpfile.shapeinfos[i].hotspot_x = str2uint(shapedata, 24);
		slpfile.shapeinfos[i].hotspot_y = str2uint(shapedata, 28);

		cout << "Data Offsets: " << slpfile.shapeinfos[i].data_offsets << '\n';
		cout << "Outline Offset: " << slpfile.shapeinfos[i].outline_offset << '\n';
		// palette offset is 0 for all drs files
		cout << "Palette Offset: " << slpfile.shapeinfos[i].palette_offset << '\n';

		cout << "Properties: " << slpfile.shapeinfos[i].properties << '\n';
		cout << "Width: " << slpfile.shapeinfos[i].width << '\n';
		cout << "Height: " << slpfile.shapeinfos[i].height << '\n';
		cout << "X Hotspot: " << slpfile.shapeinfos[i].hotspot_x << '\n';
		cout << "Y Hotspot: " << slpfile.shapeinfos[i].hotspot_y << '\n';
	}
 }
