/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file drs.cpp Functions related to extracting .drs files */

#include <cassert>
#include <fstream>

#include "drs.h"
#include "filereader.h"

void DRSFile::ReadHeader(BinaryFileReader &bfr)
{
	this->copyright    = bfr.ReadString(40);
	this->version      = bfr.ReadString( 4);
	this->type         = bfr.ReadString(12);
	this->num_tables   = bfr.ReadNum<uint32_t>();
	this->first_offset = bfr.ReadNum<uint32_t>();
}

DRSTableInfo DRSFile::ReadTableInfo(BinaryFileReader &bfr)
{
	DRSTableInfo dti;
	dti.character    = bfr.ReadNum<uint8_t>();
	dti.extension    = bfr.ReadString(3);

	/* Extension is reversed, for whatever reason. */
	std::swap(dti.extension.at(0), dti.extension.at(2));

	dti.table_offset = bfr.ReadNum<uint32_t>();
	dti.num_files    = bfr.ReadNum<uint32_t>();
	return dti;
}

DRSTable DRSFile::ReadTable(BinaryFileReader &bfr)
{
	DRSTable dt;
	dt.file_id     = bfr.ReadNum<int32_t>();
	dt.file_offset = bfr.ReadNum<uint32_t>();
	dt.file_size   = bfr.ReadNum<uint32_t>();
	return dt;
}

/**
 * Actually extract the drs file
 * @param path The path to the drs file
 */
void ExtractDRSFile(const std::string &path)
{
    size_t dirstartpos = path.rfind(PATHSEP) + 1;
	std::string filename = path.substr(dirstartpos, path.length() - dirstartpos);
	std::cout << "Reading " << path << ":\n";

	BinaryFileReader binfile(path);
	DRSFile drsfile;
	std::string filedir = EXTRACT_DIR + filename.substr(0, filename.length() - 4) + PATHSEP;
	std::cout << "Files being extracted to: " << filedir << std::endl;
	GenCreateDirectory(filedir);

	if (binfile.GetRemaining() < HEADER_SIZE) {
		std::cerr << "File is too small: Only " << binfile.GetRemaining() << " bytes long\n";
		return;
	}

	drsfile.ReadHeader(binfile);

	for (uint32_t i = 0; i < drsfile.num_tables; i++) {
		drsfile.infos.push_back(drsfile.ReadTableInfo(binfile));
	}

	for (size_t i = 0; i < drsfile.num_tables; i++) {
		for (size_t j = 0; j < drsfile.infos.at(i).num_files; j++) {
			assert(binfile.GetPosition() == drsfile.infos.at(i).table_offset + j * TABLE_SIZE);

			drsfile.infos.at(i).file_infos.push_back(drsfile.ReadTable(binfile));
		}
	}

	for (size_t i = 0; i < drsfile.num_tables; i++) {
		for (size_t j = 0; j < drsfile.infos.at(i).num_files; j++) {
			assert(binfile.GetPosition() == drsfile.infos.at(i).file_infos.at(j).file_offset);

			std::string out_file = filedir;
			out_file += std::to_string(drsfile.infos.at(i).file_infos.at(j).file_id);
			out_file += '.';
			out_file += drsfile.infos.at(i).extension;
			std::ofstream out_fs(out_file, std::ios::binary);
			if (!out_fs.is_open()) {
				std::cerr << "Error writing to " << out_file << std::endl;
				/* TODO: Abort instead? */
				binfile.SkipBytes(drsfile.infos.at(i).file_infos.at(j).file_size);
				continue;
			}
			binfile.ReadBlob(out_fs, drsfile.infos.at(i).file_infos.at(j).file_size);
			out_fs.close();
			if (drsfile.infos.at(i).extension == "slp") {
				ExtractSLPFile(out_file);
			}
		}
	}
}
