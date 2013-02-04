/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file drs.cpp Functions related to extracting .drs files */

#include <cstring>
#include <iterator>

#include "drs.h"
#include "slp.h"
#include "extractdrs.h"

/**
 * Create a directory with the given name
 * @note Taken from the OpenTTD project
 * @param name The name of the new directory
 */
void CreateDirectory(const char *name)
{
#if defined(WIN32) || defined(WINCE)
	CreateDirectory(name, NULL);
#elif defined(OS2) && !defined(__INNOTEK_LIBC__)
	mkdir(name);
#elif defined(__MORPHOS__) || defined(__AMIGAOS__)
	size_t len = strlen(name) - 1;
	if (name[len] '/') {
		name[len] = '\0'; // Don't want a path-separator on the end
	}

	mkdir(name, 0755);
#else
	mkdir(name, 0755);
#endif
}

/**
 * Actually extract the drs file
 * @param path The path to the drs file
 */
void ExtractDRSFile(const string &path)
{
    int dirstartpos = path.rfind(PATHSEP) + 1;
	string filename = path.substr(dirstartpos, path.length() - dirstartpos);
	cout << "Reading " << path << ":\n";

	const vector<byte> filedata = ReadFile(path);

	if (filedata.empty() || filedata.size() < HEADER_SIZE) {
		std::cerr << "File is too small: Only " << filedata.size() << " bytes long\n";
		return;
	}
	vector<byte>::const_iterator p_filedata = filedata.begin();

	/* Get the header */
	DRS_Header header;
	header.copyright = string(p_filedata, p_filedata + COPYRIGHT_SIZE);
	p_filedata += COPYRIGHT_SIZE;
	header.version = string(p_filedata, p_filedata + VERSION_SIZE);
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
		p_filedata++;

		/* Get and re-order the extension */
		tableinfos[i].extension = string(p_filedata, p_filedata + 3);
		std::swap(tableinfos[i].extension[0], tableinfos[i].extension[2]);
		p_filedata += 3;

		tableinfos[i].tbloffset = vec2uint(filedata, p_filedata - filedata.begin());
		tableinfos[i].numfiles = vec2uint(filedata, p_filedata - filedata.begin() + 4);

		cout << "TableInfo No." << i + 1 << ':' << endl;
		cout << "\tCharacter: " << (int)tableinfos[i].character << endl;
		cout << "\tExtension: " << tableinfos[i].extension << endl;
		cout << "\tNumber of files: " << tableinfos[i].numfiles << endl;

		tableinfos[i].fileinfo = new DRS_Table[tableinfos[i].numfiles];
		/* Construct the directory path, without extension */
		string filedir = EXTRACT_DIR + filename.substr(0, filename.length() - 4) + PATHSEP;
		cout << "Files being extracted to: " << filedir << endl;
		CreateDirectory(filedir.c_str());
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
				std::cerr << "Error writing to " << ss.str() << endl;
				continue;
			}
			p_filedata = filedata.begin() + tableinfos[i].fileinfo[j].fileoffset;
			//outfile.write(*p_filedata, tableinfos[i].fileinfo[j].filesize);
			std::ostream_iterator<byte> oi(outfile);
			std::copy(p_filedata, p_filedata + tableinfos[i].fileinfo[j].filesize, oi);
			outfile.close();
			if (tableinfos[i].extension == "slp") {
				ExtractSLPFile(ss.str());
			}
		}

		delete[] tableinfos[i].fileinfo;
		cout << endl;
	}
	delete[] tableinfos;
	cout << endl;
}
