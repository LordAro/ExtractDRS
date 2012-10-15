OBJDIR=objects

CXX=g++
CXXFLAGS=-Wall -g

CXXFILES=extractdrs.cpp drs.cpp

OBJFILES=$(addprefix $(OBJDIR)/,$(CXXFILES:.cpp=.o))
TARGET=extractdrs

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJFILES)

run: $(TARGET)
	./$(TARGET)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	for i in $(OBJFILES) $(DEPEND) $(TARGET);\
	do if test -f $$i; then rm $$i; fi;\
	done

.PHONY: run clean all
