CXX = g++
CXXFLAGS = -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
LDFLAGS = -lfuse
LIBS = -L../libWad -lWad
TARGET = wadfs

all: $(TARGET)

$(TARGET): wadfs.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)