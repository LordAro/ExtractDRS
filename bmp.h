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

bool CreateBMP(const std::string &filename, SLPShape *shape);

/** BMP File Header (stored in little endian) */
struct BitmapFileHeader {
	uint16 type;
	uint size;
	uint reserved;
	uint off_bits;
} __attribute__((packed));

/** BMP Info Header (stored in little endian) */
struct BitmapInfoHeader {
	uint size;
	uint width, height;
	uint16 planes, bitcount;
	uint compression, sizeimage, xpels, ypels, clrused, clrimp;
};

/** Format of palette data in BMP header */
struct RgbQuad {
	uint8 blue, green, red, reserved;
};

#endif /* BMP_H */
