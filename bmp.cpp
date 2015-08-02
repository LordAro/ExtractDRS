/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <cstring>

#include "bmp.h"
#include "palette.h"

/**
 * Writes an SLP shape to a .bmp file
 * @param filename filename of the intended file, including extension.
 * @param shape #SLP_Shape to get the data from.
 * @note Based on functions in OpenTTD project.
 * @return true iff the file was written.
 */
bool CreateBMP(const std::string &filename, const SLPShape &shape)
{
	uint32_t width = shape.width;
	uint32_t height = shape.height;

	std::ofstream f;
	f.open(filename.c_str(), std::ios::binary);
	if (!f.is_open()) {
		std::cerr << "Error writing to " << filename << '\n';
		return false;
	}

	/* Each scanline must be aligned on a 4 byte boundary
	 * (x + (n - 1)) AND FLIP(n - 1) */
	uint32_t bytewidth = (width + 3u) & ~3u; // bytes per line in file

	/* Size of palette */
	const uint32_t pal_size = sizeof(RgbQuad) * 256;

	/* Setup the file header */
	BitmapFileHeader bfh;
	bfh.type = 19778; // 'BM' constant
	bfh.size = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + pal_size + bytewidth * height;
	bfh.reserved = 0;
	bfh.off_bits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + pal_size;

	/* Setup the info header */
	BitmapInfoHeader bih;
	bih.size = sizeof(BitmapInfoHeader);
	bih.width = width;
	bih.height = height;
	bih.planes = 1;
	bih.bitcount = 8;
	bih.compression = 0;
	bih.sizeimage = 0;
	bih.xpels = 0;
	bih.ypels = 0;
	bih.clrused = 0;
	bih.clrimp = 0;

	/* Write file header and info header */
	f.write(reinterpret_cast<char *>(&bfh), sizeof(BitmapFileHeader));
	if (!f.good()) {
		return false;
	}

	f.write(reinterpret_cast<char *>(&bih), sizeof(BitmapInfoHeader));
	if (!f.good()) {
		return false;
	}

	enum {
		RED,
		GREEN,
		BLUE,
	};

	/* Convert the palette to the windows format */
	RgbQuad rq[256];
	for (uint32_t i = 0; i < 256; i++) {
		rq[i].reserved = 0;
		rq[i].blue  = bmp_palette[i][BLUE];
		rq[i].green = bmp_palette[i][GREEN];
		rq[i].red   = bmp_palette[i][RED];
	}
	/* Write the palette */
	f.write(reinterpret_cast<char *>(&rq), sizeof(rq));
	if (!f.good()) {
		return false;
	}
	/* Start at the bottom, since bitmaps are stored bottom up */
	for (uint32_t n = height; n != 0; n--) {
		/* Write to file */
		f.write(reinterpret_cast<const char *>(&shape.rows.at(n - 1).pixels.at(0)), width);
		if (bytewidth != width) {
			char *zeros = new char[bytewidth - width]();
			f.write(zeros, bytewidth - width);
			delete[] zeros;
		}
		if (!f.good()) {
			return false;
		}
	}

	f.close();
	return true;
}
