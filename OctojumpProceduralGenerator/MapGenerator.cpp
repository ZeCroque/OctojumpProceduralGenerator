#include "pch.h"
#include "MapGenerator.h"
#include <time.h>
#include <string>
#include <algorithm>

using namespace std;

MapGenerator::MapGenerator(int iSize, int iRandomFillPercent) : _iSize(iSize), _iRandomFillPercent(iRandomFillPercent), _sSeed(string()), _bUseRandomSeed(true)
{
	this->_iMap = new int* [_iSize];

	for (int i=0; i<_iSize; ++i)
	{
		this->_iMap[i] = new int[_iSize];
		for (int j = 0; j < _iSize; ++j)
		{
			this->_iMap[i][j] = 0;
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

bool MapGenerator::horizontalRoadExistNearby(int x, int y)
{
	int yMin = max(y - this->_iSpaceBetweenRoads, 0);
	int yMax = min(y + this->_iSpaceBetweenRoads, this->_iSize);

	for (int j = yMin; j < yMax; ++j)
	{
		int previousValue = -1;
		for (int i = x; i < this->_iSegmentSize && i < this->_iSize; ++i)
		{
			if (!previousValue && !this->_iMap[i][j])
			{
				return true;
			}

			previousValue = this->_iMap[i][j];
		}
	}
	return false;
}

bool MapGenerator::verticalRoadExistNearby(int x, int y)
{
	int xMin = max(x - this->_iSpaceBetweenRoads, 0);
	int xMax = min(x + this->_iSpaceBetweenRoads, this->_iSize);

	for (int j = y; j < this->_iSegmentSize && j < this->_iSize; ++j)
	{
		int previousValue = -1;
		for (int i = xMin; j < xMax; ++i)
		{
			if (!previousValue && !this->_iMap[i][j])
			{
				return true;
			}

			previousValue = this->_iMap[i][j];
		}
	}
	return false;
}


int MapGenerator::getRandomPercentage()
{
	return rand() / ((RAND_MAX + 1) / 100);
}

void MapGenerator::generateMap()
{
	if (this->_bUseRandomSeed)
	{
		char tmp[80] = { 0 };
		time_t rawtime=time(nullptr);
		tm timeinfo;
		localtime_s(&timeinfo, &rawtime);
		strftime(tmp, sizeof(tmp), "%c",&timeinfo);
		this->_sSeed = string(tmp);
	}

	hash<string> hHash;
	size_t stringHash = hHash(this->_sSeed);
	srand((unsigned int) stringHash);
	int randomValue=0;

	for (int x = 0; x < this->_iSize; ++x)
	{
		for (int y = 0; y < this->_iSize; ++y)
		{
			this->_iMap[x][y] = 1;
		}
	}

	this->_iRandomFillPercent = 15;
	this->_iRandomDirectionPercent = 50;
	this->_iRandomNewSegmentPercent = 25;
	this->_iSegmentSize = this->_iSize / 10;
	this->_iSpaceBetweenRoads = 10;

	for (int x = 0; x < this->_iSize; ++x)
	{
		for (int y = 0; y < this->_iSize; ++y)
		{
			randomValue = this->getRandomPercentage();
			if (randomValue < this->_iRandomFillPercent)
			{
				bool bDirection; //0:H, 1:V

				randomValue = this->getRandomPercentage();
				bDirection = true;//randomValue < _iRandomDirectionPercent;
				int xTmp = x;
				do
				{
					if (bDirection && !horizontalRoadExistNearby(xTmp,y))
					{					
						//Horizontal
						for (int i = xTmp; i < xTmp+this->_iSegmentSize && i < this->_iSize; ++i)
						{
							this->_iMap[i][y] = 0;
						}
						xTmp += this->_iSegmentSize;
					}
					/*else if (!bDirection && !verticalRoadExistNearby(x, y))
					{
						//Vertical
						for (int i = y; i < y+this->_iSegmentSize && i < this->_iSize; ++i)
						{
							this->_iMap[x][i] = 0;
						}
					}*/

					randomValue = this->getRandomPercentage();
				} while (randomValue < _iRandomDirectionPercent);
			}
	
		}
	}
}

int** MapGenerator::getMap() const
{
	return this->_iMap;
}

int MapGenerator::getSize() const
{
	return this->_iSize;
}