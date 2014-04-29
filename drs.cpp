/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file drs.cpp Functions related to extracting .drs files */

#include <iterator>
#include <sstream>
#include <fstream>

#include "drs.h"

/**
 * Actually extract the drs file
 * @param path The path to the drs file
 */
void ExtractDRSFile(const std::string &path)
{
    int dirstartpos = path.rfind(PATHSEP) + 1;
	std::string filename = path.substr(dirstartpos, path.length() - dirstartpos);
	std::cout << "Reading " << path << ":\n";

	const std::vector<uint8> filedata = ReadFile(path);

	if (filedata.empty() || filedata.size() < HEADER_SIZE) {
		std::cerr << "File is too small: Only " << filedata.size() << " bytes long\n";
		return;
	}
	std::vector<uint8>::const_iterator p_filedata = filedata.begin();

	/* Get the header */
	DRS_Header header;
	header.copyright = std::string(p_filedata, p_filedata + COPYRIGHT_SIZE);
	p_filedata += COPYRIGHT_SIZE;
	header.version = std::string(p_filedata, p_filedata + VERSION_SIZE);
	p_filedata += VERSION_SIZE;
	header.type = vec2uint(filedata, p_filedata - filedata.begin());
	p_filedata += TYPE_SIZE;

	header.numtables = vec2uint(filedata, p_filedata - filedata.begin());
	header.firstoffset = vec2uint(filedata, p_filedata - filedata.begin() + 4);

	/* Get tables */
	DRS_TableInfo *tableinfos = new DRS_TableInfo[header.numtables];
	for (uint i = 0; i < header.numtables; i++) {
		p_filedata = filedata.begin() + HEADER_SIZE + (i * TABLE_SIZE);
		tableinfos[i].character = *p_filedata;
		++p_filedata;

		/* Get and re-order the extension */
		tableinfos[i].extension = std::string(p_filedata, p_filedata + 3);
		std::swap(tableinfos[i].extension[0], tableinfos[i].extension[2]);
		p_filedata += 3;

		tableinfos[i].tbloffset = vec2uint(filedata, p_filedata - filedata.begin());
		tableinfos[i].numfiles = vec2uint(filedata, p_filedata - filedata.begin() + 4);

		std::cout << "TableInfo No." << i + 1 << ':' << std::endl;
		std::cout << "\tCharacter: " << (int)tableinfos[i].character << std::endl;
		std::cout << "\tExtension: " << tableinfos[i].extension << std::endl;
		std::cout << "\tNumber of files: " << tableinfos[i].numfiles << std::endl;

		tableinfos[i].fileinfo = new DRS_Table[tableinfos[i].numfiles];
		/* Construct the directory path, without extension */
		std::string filedir = EXTRACT_DIR + filename.substr(0, filename.length() - 4) + PATHSEP;
		std::cout << "Files being extracted to: " << filedir << std::endl;
		GenCreateDirectory(filedir);
		for (uint j = 0; j < tableinfos[i].numfiles; j++) {
			p_filedata = filedata.begin() + tableinfos[i].tbloffset + (j * TABLE_SIZE);

			tableinfos[i].fileinfo[j].fileid = vec2uint(filedata, p_filedata - filedata.begin());
			tableinfos[i].fileinfo[j].fileoffset = vec2uint(filedata, p_filedata - filedata.begin() + 4);
			tableinfos[i].fileinfo[j].filesize = vec2uint(filedata, p_filedata - filedata.begin() + 8);

			std::stringstream ss;
			ss << filedir;
			ss << tableinfos[i].fileinfo[j].fileid;
			ss << '.';
			ss << tableinfos[i].extension;
			std::ofstream outfile;
			outfile.open(ss.str().c_str(), std::ios::binary);
			if (!outfile.is_open()) {
				std::cerr << "Error writing to " << ss.str() << std::endl;
				continue;
			}
			p_filedata = filedata.begin() + tableinfos[i].fileinfo[j].fileoffset;
			//outfile.write(*p_filedata, tableinfos[i].fileinfo[j].filesize);
			std::ostream_iterator<uint8> oi(outfile);
			std::copy(p_filedata, p_filedata + tableinfos[i].fileinfo[j].filesize, oi);
			outfile.close();
			if (tableinfos[i].extension == "slp") {
				ExtractSLPFile(ss.str());
			}
		}

		delete[] tableinfos[i].fileinfo;
		std::cout << std::endl;
	}
	delete[] tableinfos;
	std::cout << std::endl;
}
