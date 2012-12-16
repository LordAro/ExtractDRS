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
	uint data_offsets;
	uint outline_offset;
	uint palette_offset;
	uint properties;
	int width;
	int height;
	int hotspot_x;
	int hotspot_y;
};

void ExtractSLPFile(string filename);

#endif /* SLP_H */
