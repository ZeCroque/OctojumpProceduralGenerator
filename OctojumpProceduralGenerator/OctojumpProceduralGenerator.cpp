#include "pch.h"
#include "IHM.h"
#include "IO.h"
#include "MapGenerator.h"
#include <iostream>
#include <string>

using namespace std;

// 40 10 
// 44 11 // 48 12 // 52 13 // 56 14 // 60 15 // 64 16 // 68 17 // 72 18 // 76 19 // 80 20
// 84 21 // 88 22 // 92 23 // 96 24 // 100 25 // 104 26 // 108 27 // 112 28 // 116 29 // 120 30 
// 124 31  // 128 32 // 132 33 // 136 34 // 140 35 // 144 36 // 148 37 // 152 38 // 156 39 // 160 40

int main(int argc, char *argv[]) {
	if (argc == 1) {
		debug();
	}
	else {
		if (argc == 6 || argc == 7) {
			//Argv 1 : MapSize
			int* iSize = nullptr;
			stringToInt(string(argv[1]), &iSize);
			if (iSize == nullptr) {
				return 1;
			}
			//Argv 2 : RandomFillPercent
			int* iRandomFillPercent = nullptr;
			stringToInt(string(argv[2]), &iRandomFillPercent);
			if (iRandomFillPercent == nullptr) {
				return 1;
			}
			//Argv 3 : Building Max Height
			int* iBuildingMaxHeight = nullptr;
			stringToInt(string(argv[3]), &iBuildingMaxHeight);
			if (iBuildingMaxHeight == nullptr) {
				return 1;
			}
			//Argv 4 : Space Between Road
			int* iSpaceBetweenRoads = nullptr;
			stringToInt(string(argv[4]), &iSpaceBetweenRoads);
			if (iSpaceBetweenRoads == nullptr) {
				return 1;
			}
			//Argv 5 : Level To Save
			wchar_t* buffer;
			int bufferSize;
			bufferSize = MultiByteToWideChar(CP_ACP, 0, argv[5], -1, nullptr, 0);
			buffer = new wchar_t[bufferSize];
			MultiByteToWideChar(CP_ACP, 0, argv[5], -1, (LPWSTR)buffer, bufferSize);
			//Argv 6 : Seed (Optional)
			MapGenerator* mg = nullptr;
			if (argc == 7) {
				mg = new MapGenerator(*iSize, *iRandomFillPercent, *iBuildingMaxHeight, *iSpaceBetweenRoads, string(argv[6]));

			}
			else {
				mg = new MapGenerator(*iSize, *iRandomFillPercent , *iBuildingMaxHeight, *iSpaceBetweenRoads);
			}
			//Generate Map
			mg->generateMap();
			saveFile(buffer, mg->getMap(), *iSize);
			delete mg;
		}
		else {
			cerr << "Error: Wrong number of arguments provided (You have provided " << argc << " arguments)" << endl;
			return -1;
		}
	}
	return 0;
}