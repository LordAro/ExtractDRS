/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file filereader.cpp Definitions of methods relating to reading files. */

#include <algorithm>
#include <fstream>
#include <string>

#include "filereader.h"

/**
 * Constructor.
 * @param filepath File to open.
 */
BinaryFileReader::BinaryFileReader(const std::string &filepath)
{
	this->fp.open(filepath, std::ios::binary);
	if (!this->fp.is_open()) throw;

	this->fp.seekg(0, this->fp.end);
	this->file_size = this->fp.tellg();

	this->fp.seekg(0, this->fp.beg);
}

/** Destructor. */
BinaryFileReader::~BinaryFileReader()
{
	if (this->fp != nullptr && this->fp.is_open()) this->fp.close();
}

/**
 * Reads a string from the file.
 * @param len Length of string to read.
 * @return The string.
 */
std::string BinaryFileReader::ReadString(size_t len)
{
	char* temp = new char[len + 1];
	this->fp.read(temp, len);
	temp[len] = '\0';
	std::string str = temp;
	delete[] temp;
	return str;
}

/**
 * Writes an arbitrary amount of data (e.g. an embedded file) to another stream.
 * Uses inout param due to bugs in libstdc++ relating to moving streams.
 * @param[out] os The stream to write to.
 * @param len Length to write.
 */
void BinaryFileReader::ReadBlob(std::ostream &os, size_t len)
{
	std::copy_n(std::istreambuf_iterator<char>(this->fp), len, std::ostreambuf_iterator<char>(os));
	this->fp.get(); // Advance the pointer past what we just read.
}

/**
 * Skip a number of bytes in the file.
 * @param len Number of bytes to skip.
 */
void BinaryFileReader::SkipBytes(size_t len)
{
	this->fp.seekg(len, std::ios::cur);
}

/**
 * Gets the number of bytes read so far.
 * @return Current position in the file.
 */
size_t BinaryFileReader::GetPosition()
{
	return this->fp.tellg();
}

/**
 * Gets the number of bytes that haven't yet been read.
 * @return The remaining number of bytes.
 */
size_t BinaryFileReader::GetRemaining()
{
	return this->file_size - this->GetPosition();
}
