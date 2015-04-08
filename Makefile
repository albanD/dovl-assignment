GCC = g++
LD = $(GCC)

TARGET = run_dovl

SOURCES_LIB = $(wildcard *.cpp)
OBJECTS_LIB = $(SOURCES_LIB:.cpp=.o)

all: CFLAGS = -c -std=c++11 -Wall -g -I.
release: CFLAGS = -c -std=c++11 -Wall -O3 -I.
LDFLAGS = -lstdc++ -pthread
# we can get this with "libpng-config --ldflags" in a shell
PNGLDFLAGS = -L/usr/lib/x86_64-linux-gnu -lpng12

all: $(TARGET)
release: $(TARGET)


%.o: %.cpp
	$(GCC) $(CFLAGS) $< -o $@

$(TARGET): $(OBJECTS_LIB)
	$(LD) -o $@ $^ $(LDFLAGS) $(PNGLDFLAGS)

clean:
	-rm $(TARGET) $(OBJECTS_LIB)

cl:
	-rm $(OBJECTS_LIB)

test:
	-./run_dovl
