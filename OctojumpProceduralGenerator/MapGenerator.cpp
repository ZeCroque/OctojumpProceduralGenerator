#include "pch.h"
#include "MapGenerator.h"
#include <time.h>
#include <string>
#include <algorithm>
#include <iostream>
#include "IHM.h"

using namespace std;

MapGenerator::MapGenerator(int iSize, int iRandomFillPercent) : _iSize(iSize), _iRandomFillPercent(iRandomFillPercent), _sSeed(string()), _bUseRandomSeed(true)
{
	this->_iMap = new int* [_iSize];

	for (int i=0; i<_iSize; ++i)
	{
		this->_iMap[i] = new int[_iSize];
		for (int j = 0; j < _iSize; ++j)
		{
			this->_iMap[i][j] = 1;
		}
	}
}

MapGenerator::MapGenerator(int iSize, int iRandomFillPercent, std::string sSeed) : MapGenerator(iSize, iRandomFillPercent) 
{
	this->_bUseRandomSeed = false;
	this->_sSeed = sSeed;
}

MapGenerator::~MapGenerator()
{
	for (int i = 0; i < this->_iSize; ++i)
	{
		delete[] this->_iMap[i];
	}
	delete[] this->_iMap;
}

MapGenerator& MapGenerator::operator = (const MapGenerator& mg)
{
	this->_bUseRandomSeed = mg._bUseRandomSeed;
	*(this->_iMap) = *(mg._iMap);
	this->_iSize = mg._iSize;
	this->_iSize = mg._iSize;
	this->_iRandomFillPercent = mg._iRandomFillPercent;
	this->_sSeed = mg._sSeed;
	return *this;
}

bool MapGenerator::rightRoadExistNearby(int x, int y, int iEndLine)
{
	int yMin = max(y - this->_iSpaceBetweenRoads, 0);
	int yMax = min(y + this->_iSpaceBetweenRoads, this->_iSize);

	for (int j = yMin; j < yMax; ++j)
	{
		int last = -1;
		for (int i = x; i < iEndLine; ++i)
		{
			if (!this->_iMap[i][j] && !last)
			{
				return true;
			}
			last = this->_iMap[i][j];
		}
	}
	return false;
}

bool MapGenerator::downRoadExistNearby(int x, int y, int iEndLine)
{
	int xMin = max(x - this->_iSpaceBetweenRoads, 0);
	int xMax = min(x + this->_iSpaceBetweenRoads, this->_iSize);

	for (int i = xMin; i < xMax; ++i)
	{
		int last = -1;
		for (int j = y; j < iEndLine; ++j)
		{
			if (!this->_iMap[i][j] && !last)
			{
				return true;
			}
			last = this->_iMap[i][j];
		}
	}
	return false;
}

bool MapGenerator::leftRoadExistNearby(int x, int y, int iEndLine)
{
	int yMin = max(y - this->_iSpaceBetweenRoads, 0);
	int yMax = min(y + this->_iSpaceBetweenRoads, this->_iSize);

	for (int j = yMin; j < yMax; ++j)
	{
		int last = -1;
		for (int i = x; i > iEndLine; --i)
		{
			if (!this->_iMap[i][j] && !last)
			{
				return true;
			}
			last = this->_iMap[i][j];
		}
	}
	return false;
}

bool MapGenerator::upRoadExistNearby(int x, int y, int iEndLine)
{
	int xMin = max(x - this->_iSpaceBetweenRoads, 0);
	int xMax = min(x + this->_iSpaceBetweenRoads, this->_iSize);

	for (int i = xMin; i < xMax; ++i)
	{
		int last = -1;
		for (int j = y; j > iEndLine; --j)
		{
			if (!this->_iMap[i][j] && !last)
			{
				return true;
			}
			last = this->_iMap[i][j];
		}
	}
	return false;
}


int MapGenerator::getRandomPercentage()
{
	return rand() / ((RAND_MAX + 1) / 100);
}

int randInt(int min, int max)
{
	return min + rand() % (max - min + 1);
}

void MapGenerator::printMap()
{
	clearConsole();

	for (int j = 0; j < this->_iSize; ++j)
	{
		for (int i = 0; i < this->_iSize; ++i)
		{
			cout << this->_iMap[i][j] << " ";
		}
		cout << endl;
	}
}

void MapGenerator::generateMap()
{
	if (this->_bUseRandomSeed)
	{
		char tmp[80] = { 0 };
		time_t rawtime = time(nullptr);
		tm timeinfo;
		localtime_s(&timeinfo, &rawtime);
		strftime(tmp, sizeof(tmp), "%c", &timeinfo);
		this->_sSeed = string(tmp);
	}

	hash<string> hHash;
	size_t stringHash = hHash(this->_sSeed);
	srand((unsigned int)stringHash);
	int randomValue = 0;

	this->_iRandomFillPercent = 25;
	this->_iSpaceBetweenRoads = 5;
	this->_iMinSpaceFromBorder = (int)(this->_iSize * 0.1);
	this->_iMinRoadSize = (int)(this->_iSize * 0.6);
	this->_iRoadExtensionRange = (int)(this->_iSize * 0.3);

	for (float i = 0; i < (int)(0.1 * this->_iSize); i += this->newCrossline());//TODO make setting
}

float MapGenerator::newCrossline()
{
	int x = randInt(this->_iMinSpaceFromBorder, (this->_iSize - this->_iMinSpaceFromBorder));
	int y = randInt(this->_iMinSpaceFromBorder, (this->_iSize - this->_iMinSpaceFromBorder));

	float iCrossroadCounts = 0;

	int iEndLine = min(x + this->_iMinRoadSize + randInt(0, this->_iRoadExtensionRange), this->_iSize);	
	if (!this->rightRoadExistNearby(x, y, iEndLine))
	{
		iCrossroadCounts += 0.25;
		for (int i = x ; i < iEndLine; ++i)
		{
			this->_iMap[i][y] = 0;
		}
	}

	iEndLine = min(y + this->_iMinRoadSize + randInt(0, this->_iRoadExtensionRange), this->_iSize);
	if (!this->downRoadExistNearby(x, y, iEndLine))
	{
		iCrossroadCounts += 0.25;
		for (int i = y; i < iEndLine; ++i)
		{
			this->_iMap[x][i] = 0;
		}
	}

	iEndLine = max(x - this->_iMinRoadSize - randInt(0, this->_iRoadExtensionRange), 0);
	if (!this->leftRoadExistNearby(x, y, iEndLine))
	{
		iCrossroadCounts += 0.25;
		for (int i = x; i > iEndLine; --i)
		{
			this->_iMap[i][y] = 0;
		}
	}

	iEndLine = max(y - this->_iMinRoadSize - randInt(0, this->_iRoadExtensionRange), 0);
	if (!this->upRoadExistNearby(x, y, iEndLine))
	{
		iCrossroadCounts += 0.25;
		for (int i = y; i > iEndLine; --i)
		{
			this->_iMap[x][i] = 0;
		}
	}

	return iCrossroadCounts;
}
	
int** MapGenerator::getMap() const
{
	return this->_iMap;
}

int MapGenerator::getSize() const
{
	return this->_iSize;
}