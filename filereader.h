/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file filereader.h Declarations relating to reading files. */

#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include <string>

/** Helper class for reading binary files. */
class BinaryFileReader {
public:
	BinaryFileReader(const std::string &filepath);
	~BinaryFileReader();

	/**
	 * Reads a number of type T from the file.
	 * @tparam T Type of number to read.
	 * @return The number.
	 */
	template<typename T>
	T ReadNum()
	{
		T num;
		this->fp.read(reinterpret_cast<char *>(&num), sizeof(num));
		return num;
	}

	std::string ReadString(size_t len);
	void ReadBlob(std::ostream &os, size_t len);
	void SkipBytes(size_t len);

	size_t GetPosition();
	size_t GetRemaining();

private:
	std::ifstream fp;     ///< File pointer.
	size_t file_size = 0; ///< Size of the file.
};

#endif /* FILEREADER_H */
