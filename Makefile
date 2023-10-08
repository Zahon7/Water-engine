CXX=/bin/g++
MINGW-CXX=/bin/x86_64-w64-mingw32-g++

override CXXFLAGS+=-lraylib -Iinclude -lGL -DDEBUG
override MINGW-CXXFLAGS+=-lopengl32 -lraylib -lwinmm -Iinclude -lstdc++fs -DDEBUG

all: water run

release: water mingw-water

water: source/water.cpp
	$(CXX) source/water.cpp $(CXXFLAGS) -o binary/water

mingw-water: source/water.cpp
	$(MINGW-CXX) source/water.cpp -mwindows -static $(MINGW-CXXFLAGS) -o binary/water.exe

run: binary/water
	binary/water


# ------- GITHUB ---------

upload:
	git add -A

	git commit -F DONE.txt

	git push

	# CLEANUP
	rm DONE.txt
	touch DONE.txt
