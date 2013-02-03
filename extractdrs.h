/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXTRACTDRS_H
#define EXTRACTDRS_H

#include <vector>

using std::string;
using std::vector;
using std::endl;
using std::cout;


static const string PATHSEP = "/";
static const string EXTRACT_DIR = "extracted" + PATHSEP;

typedef unsigned char byte;

vector<byte> ReadFile(const string &path);

uint vec2uint(const vector<byte> vec, int offset);

#endif /* EXTRACTDRS_H */
