# This file is part of ExtractDRS.
# ExtractDRS is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
# ExtractDRS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with ExtractDRS. If not, see <http://www.gnu.org/licenses/>.

OBJDIR=obj

CXX=clang++
CXXFLAGS=-Wall -Wextra -pedantic -g -std=c++11

CXXFILES=bmp.cpp drs.cpp extractdrs.cpp filereader.cpp slp.cpp

OBJFILES=$(addprefix $(OBJDIR)/,$(CXXFILES:.cpp=.o))
TARGET=extractdrs

all: $(TARGET)

$(TARGET): create $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJFILES)

run: $(TARGET)
	./$(TARGET) data/

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

create:
	mkdir -p $(OBJDIR)

clean:
	for i in $(OBJFILES) $(DEPEND) $(TARGET);\
	do if test -f $$i; then rm $$i; fi;\
	done

.PHONY: run clean all
