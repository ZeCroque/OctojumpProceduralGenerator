#include "pch.h"
#include "IHM.h"
#include "IO.h"
#include "MapGenerator.h"
#include <iostream>
#include <string>

using namespace std;

// 4 1 // 8 2 // 12 3 // 16 4 // 20 5 // 24 6 // 28 7 // 32 8 // 36 9 // 40 10 // 44 11 // 48 12 // 52 13 // 56 14 // 60 15 // 64 16 // 68 17 // 72 18 // 76 19 // 80 20
// 84 21 // 88 22 // 92 23 // 96 24 // 100 25 // 104 26 // 108 27 // 112 28 // 116 29 // 120 30 // 124 31  // 128 32 // 132 33 // 136 34 // 140 35 // 144 36 // 148 37 // 152 38 // 156 39 // 160 40

int main(int argc, char *argv[]) {
	if (argc == 1) {
		debug();
	}
	else {
		if (argc == 6 || argc == 7) {
			int *iSize = nullptr;
			int *iRandomFillPercent = nullptr;
			int *iBuildingMaxHeight = nullptr;
			int* iSpaceBetweenRoads = nullptr;

			MapGenerator *mg = nullptr;
			wchar_t *buffer;
			int bufferSize;

			//old (argc 5)
			//0
			//1 iSize
			//2 iRandomFillPercent
			//3 seed -- opt
			//4 levelToSave
			//new (argc 7)
			//0
			//1 iSize
			//2 irandomFillPercent
			//3 iBuildingMaxHeight
			//4 iSpaceBetweenRoads
			//5 levelToSave
			//6 seed -- opt

			stringToInt(string(argv[1]), &iSize);
			if (iSize == nullptr) {
				return 1;
			}
			stringToInt(string(argv[2]), &iRandomFillPercent);
			if (iRandomFillPercent == nullptr) {
				return 1;
			}
			stringToInt(string(argv[3]), &iBuildingMaxHeight);
			if (iBuildingMaxHeight == nullptr) {
				return 1;
			}
			stringToInt(string(argv[4]), &iSpaceBetweenRoads);
			if (iSpaceBetweenRoads == nullptr) {
				return 1;
			}

			if (argc == 7) {
				mg = new MapGenerator(*iSize, *iRandomFillPercent, string(argv[6]));

			}
			else {
				mg = new MapGenerator(*iSize, *iRandomFillPercent);
			}

			mg->generateMap();

			bufferSize = MultiByteToWideChar(CP_ACP, 0, argv[5], -1, nullptr, 0);
			buffer = new wchar_t[bufferSize];
			MultiByteToWideChar(CP_ACP, 0, argv[5], -1, (LPWSTR)buffer, bufferSize);
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