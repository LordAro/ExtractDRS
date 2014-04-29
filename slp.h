/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SLP_H
#define SLP_H

#include <iostream>

#include "extractdrs.h"

struct SLP_Header
{
	std::string version;
	uint num_shapes;
	std::string comment;
};

struct SLP_Info
{
	uint data_offset;
	uint outline_offset;
	uint palette_offset;
	uint properties;
	int width;
	int height;
	int hotspot_x;
	int hotspot_y;
};

struct SLP_Row
{
	uint8* pixel; // 8bpp
	uint16 left;
	uint16 right;
	uint datastart; // start of the data for the row
};

struct SLP_Shape
{
	SLP_Info info;
	SLP_Row* row;
};

struct SLP_File
{
	int id;
	SLP_Header header;
	SLP_Shape* shape;
};

enum SLP_Commands
{
	CMD_Lesser_Block_Copy  = 0x00, // also 4, 8, 0x0c
	CMD_Lesser_Skip        = 0x01, // also 5, 9, 0x0d
	CMD_Greater_Block_Copy = 0x02,
	CMD_Greater_Skip       = 0x03,

	CMD_Copy_Transform     = 0x06,
	CMD_Fill               = 0x07,

	CMD_Transform          = 0x0A,
	CMD_Shadow             = 0x0B,

	CMD_Extended_Command   = 0x0E,
	CMD_End_Row            = 0x0F,
};

void ExtractSLPFile(std::string filename);

#endif /* SLP_H */
