#include "pch.h"
#include "MapGenerator.h"
#include <time.h>
#include <string>

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
			randomValue= rand() / ((RAND_MAX + 1) / 100);
			this->_iMap[x][y] = (randomValue < this->_iRandomFillPercent);
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