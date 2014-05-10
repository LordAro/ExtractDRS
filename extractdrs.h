/*
 * This file is part of ExtractDRS.
 * ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXTRACTDRS_H
#define EXTRACTDRS_H

#include <string>

static const std::string PATHSEP = "/";
static const std::string EXTRACT_DIR = "extracted" + PATHSEP;

typedef unsigned char  uint8;
typedef unsigned short uint16;

void GenCreateDirectory(const std::string &name);

#endif /* EXTRACTDRS_H */
