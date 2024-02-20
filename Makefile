CXX=/bin/g++
MINGW-CXX=/bin/x86_64-w64-mingw32-g++

override CXXFLAGS+=-lraylib -Iinclude -lGL -DDEBUG
override MINGW-CXXFLAGS+=-lopengl32 -lraylib -lwinmm -Iinclude -lstdc++fs -DDEBUG

all: water

release: water mingw-water

water: source/water.cpp
	$(CXX) source/water.cpp $(CXXFLAGS) -g -o binary/water

mingw-water: source/water.cpp
	$(MINGW-CXX) source/water.cpp -mwindows -static $(MINGW-CXXFLAGS) -o binary/water.exe


# ------- GITHUB ---------

upload:
	git add -A

	git commit -F DONE.txt

	git push

	# CLEANUP
	rm DONE.txt
	touch DONE.txt
