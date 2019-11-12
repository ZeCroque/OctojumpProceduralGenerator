#include "pch.h"
#include "IHM.h"
#include "IO.h"
#include "MapGenerator.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		debug();
	}
	else
	{
		if (argc == 3 || argc==4)
		{
			int* iSize=nullptr;
			int* iRandomFillPercent=nullptr;
			MapGenerator* mg = nullptr;

			stringToInt(string(argv[1]), &iSize);
			if (iSize == nullptr)
			{
				return 1;
			}
			stringToInt(string(argv[2]), &iRandomFillPercent);
			if (iRandomFillPercent == nullptr)
			{
				return 1;
			}

			if (argc == 3)
			{
				mg = new MapGenerator(*iSize, *iRandomFillPercent);
			}
			else
			{
				mg = new MapGenerator(*iSize, *iRandomFillPercent, string(argv[3]));
			}
			
			mg->generateMap();
			askForPathAndSave(mg->getMap(), *iSize);
			
			if (mg != nullptr)
			{
				delete mg;
			}
		}
		else
		{
			cerr << "Error! Wrong number of arguments provided!" << endl;
			return -1;
		}
	}
	

	return 0;
}


