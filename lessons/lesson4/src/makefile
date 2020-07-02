CXX = g++
CXXFLAGS = -Wall -c -std=c++11 $(SDL_INCLUDE)
SDL_LIB = -L/usr/lib -lSDL2 -lSDL2_image -Wl,-rpath=/usr/lib
SDL_INCLUDE = -I/usr/SDL_INCLUDE -I../../include
BIN_DIR = ../../bin
LDFLAGS = $(SDL_LIB)
EXE = SDL_Lesson4

all: $(EXE)

$(EXE): main.o
	$(CXX) $< $(LDFLAGS) -o $(BIN_DIR)/$@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(BIN_DIR)/$(EXE)
