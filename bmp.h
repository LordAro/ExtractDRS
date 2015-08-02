/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BMP_H
#define BMP_H

#include "slp.h"
#include "extractdrs.h"

bool CreateBMP(const std::string &filename, const SLPShape &shape);

/** BMP File Header (stored in little endian) */
struct BitmapFileHeader {
	uint16_t type;
	uint32_t size;
	uint32_t reserved;
	uint32_t off_bits;
} __attribute__((packed));

/** BMP Info Header (stored in little endian) */
struct BitmapInfoHeader {
	uint32_t size;
	uint32_t width;
	uint32_t height;

	uint16_t planes;
	uint16_t bitcount;

	uint32_t compression;
	uint32_t sizeimage;
	uint32_t xpels;
	uint32_t ypels;
	uint32_t clrused;
	uint32_t clrimp;
};

/** Format of palette data in BMP header */
struct RgbQuad {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t reserved;
};

#endif /* BMP_H */
