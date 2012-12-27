/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SLP_H
#define SLP_H

#include <iostream>
#include <sys/types.h>

using namespace std;

struct SLP_Header
{
	string version;
	uint num_shapes;
	string comment;
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

struct SLP_Pixel
{
	bool empty;
	int r;
	int g;
	int b;
};

struct SLP_Row
{
	SLP_Pixel* pixel;
	ushort left;
	ushort right;
	uint datastart; // start of the data for the row
};

struct SLP_Shape
{
	SLP_Info info;
	SLP_Row* row;
};

struct SLP_File
{
	SLP_Header header;
	SLP_Shape* shape;
};

enum SLP_Commands
{
	CMD_Colour_List        = 0x00, // also 4, 8, 0x0c
	CMD_Skip               = 0x01, // also 5, 9, 0x0d, although they do not appear in AoE2 files
	CMD_Big_Colour_List    = 0x02,
	CMD_Big_Skip           = 0x03,

	CMD_Player_Colour_List = 0x06,
	CMD_Fill               = 0x07,

	CMD_Player_Colour_Fill = 0x0A,
	CMD_Shadow_Transparent = 0x0B,

	CMD_Shadow_Player      = 0x0E,
	CMD_End_Row            = 0x0F,

	CMD_Outline            = 0x4E,

	CMD_Outline_Span       = 0x5E,
};

void ExtractSLPFile(string filename);

#endif /* SLP_H */
