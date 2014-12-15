CC = gcc
CXX = g++ -std=c++11
# Additional include directory
INCLUDES = 

# Compilation options:
# -g for degging infor and -Wall enables all warnings
CFLAGS   = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

# Linking options:
# -g for debugging info
LDFLAGS = -g

# List the libraries you need to link with in LDLIBS
# -lm for the math library
LDLIBS = -L../prob_verify -lpverify

SOURCES=main.cpp merge.cpp front.cpp back.cpp lock.cpp trbp.cpp cruise.cpp driver.cpp sensor.cpp mergechecker.cpp

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=verifymerge

# The 1st target gets build when typing "make"
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) 

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< 

.PHONY: clean
clean:
	rm -f *.o *.a *.gch a.out core verifymerge

.PHONY: all
all: clean $(EXECUTABLE)

