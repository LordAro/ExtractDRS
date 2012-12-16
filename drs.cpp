/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file drs.cpp Functions related to extracting .drs files */

#include "drs.h"
#include "slp.h"
#include "extractdrs.h"

/**
 * Converts (a part of) a string to a 4 byte uint
 * @param str The string to operate on
 * @param offset How far into the string to start
 * @return The converted uint
 */
uint str2uint(const string &str, int offset)
{
	return (byte)str[offset] + ((byte)str[offset + 1] << 8) + ((byte)str[offset + 2] << 16) + ((byte)str[offset + 3] << 24);
}

/**
 * Create a directory with the given name
 * @note Taken from the OpenTTD project
 * @param name The name of the new directory
 */
void FioCreateDirectory(const char *name)
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
	string filename = path.substr(path.find(PATHSEP) + 1, path.length());
	cout << "Reading " << path << ":\n";

	string filedata = ReadFile(path);

	if (filedata.length() < (uint)HEADER_SIZE) {
		cerr << "File is too small: Only " << filedata.length() << " bytes long\n";
		return;
	}

	/* Get the header */
	DRS_Header header;
	string headertext = filedata.substr(0, HEADER_SIZE);
	header.copyright = headertext.substr(0, COPYRIGHT_SIZE);
	header.version = headertext.substr(COPYRIGHT_SIZE, VERSION_SIZE);
	header.type = headertext.substr(COPYRIGHT_SIZE + VERSION_SIZE, TYPE_SIZE);
	header.numtables = str2uint(headertext, 56);
	header.firstoffset = str2uint(headertext, 60);

	/* Get tables */
	DRS_TableInfo *tableinfos = new DRS_TableInfo[header.numtables];
	for (uint i = 0; i < header.numtables; i++) {
		string tableinfotext = filedata.substr(HEADER_SIZE + (i * TABLE_SIZE), TABLE_SIZE);
		tableinfos[i].character = tableinfotext[0];

		/* Get and re-order the extension */
		tableinfos[i].extension = tableinfotext.substr(1, 3);
		swap(tableinfos[i].extension[0], tableinfos[i].extension[2]);

		tableinfos[i].tbloffset = str2uint(tableinfotext, 4);
		tableinfos[i].numfiles = str2uint(tableinfotext, 8);

		cout << "TableInfo No." << i + 1 << ":\n";
		cout << "\tExtension: " << tableinfos[i].extension << '\n';
		cout << "\tNumber of files: " << tableinfos[i].numfiles << '\n';

		tableinfos[i].fileinfo = new DRS_Table[tableinfos[i].numfiles];
		/* Construct the directory path, without extension */
		string filedir = EXTRACT_DIR + filename.substr(0, filename.length() - 4) + PATHSEP;
		cout << "Files being extracted to: " << filedir << '\n';
		FioCreateDirectory(filedir.c_str());
		for (uint j = 0; j < tableinfos[i].numfiles; j++) {
			string tabletext = filedata.substr(tableinfos[i].tbloffset + (j * TABLE_SIZE), TABLE_SIZE);
			tableinfos[i].fileinfo[j].fileid = str2uint(tabletext, 0);
			tableinfos[i].fileinfo[j].fileoffset = str2uint(tabletext, 4);
			tableinfos[i].fileinfo[j].filesize = str2uint(tabletext, 8);

			stringstream ss;
			ss << tableinfos[i].fileinfo[j].fileid;
			string outfilename = filedir;
			outfilename += ss.str();
			outfilename += ".";
			outfilename += tableinfos[i].extension;
			ofstream outputfile;
			outputfile.open(outfilename.c_str(), ios::out | ios::binary);
			if (!outputfile.is_open()) {
				cerr << "Error writing to " << outfilename << '\n';
				continue;
			}
			outputfile << filedata.substr(tableinfos[i].fileinfo[j].fileoffset, tableinfos[i].fileinfo[j].filesize);
			outputfile.close();
			if (tableinfos[i].extension == "slp") {
				ExtractSLPFile(outfilename);
			}
		}
		cout << '\n';
	}
	cout << '\n';
}
