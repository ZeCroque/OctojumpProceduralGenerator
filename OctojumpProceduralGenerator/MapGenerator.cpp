#include "pch.h"
#include "MapGenerator.h"
#include <time.h>
#include <string>
#include <algorithm>
#include <iostream>
#include "IHM.h"
#include "Rectangle.h"

using namespace std;


/*======================================
======FORME CANONIQUE DE COPLIEN======
========================================*/
MapGenerator::MapGenerator() : _iSize(0), _iRandomFillPercent(0), _sSeed(std::string()), _bUseRandomSeed(false), _iMap(nullptr){}

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

/*======================================
========CONSTRUCTEURS SURCHARGES========
========================================*/
MapGenerator::MapGenerator(int iSize, int iRandomFillPercent) : _iSize(iSize), _iRandomFillPercent(iRandomFillPercent), _sSeed(string()), _bUseRandomSeed(true)
{
	this->_iMap = new int* [_iSize];

	for (int i = 0; i < _iSize; ++i)
	{
		this->_iMap[i] = new int[_iSize];
		for (int j = 0; j < _iSize; ++j)
		{
			this->_iMap[i][j] = -1;
		}
	}
}

MapGenerator::MapGenerator(int iSize, int iRandomFillPercent, std::string sSeed) : MapGenerator(iSize, iRandomFillPercent)
{
	this->_bUseRandomSeed = false;
	this->_sSeed = sSeed;
}

/*======================================
=============GETTERS/SETTERS===========
========================================*/
int** MapGenerator::getMap() const
{
	return this->_iMap;
}

int MapGenerator::getSize() const
{
	return this->_iSize;
}

/*======================================
=============STATIC UTILITIES===========
========================================*/
int randInt(int min, int max)
{
	return min + rand() % (max - min + 1);
}

/*======================================
=============INITIALIZER===========
========================================*/
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

	//DEBUG
	this->_iRandomFillPercent = 25;
	this->_iSpaceBetweenRoads = 5;
	this->_iMinSpaceFromBorder = (int)(this->_iSize * 0.1);
	this->_iMinRoadSize = (int)(this->_iSize * 0.6);
	this->_iRoadExtensionRange = (int)(this->_iSize * 0.3);
	this->_iBuildingMaxHeight = 9;

	for (float i = 0; i < (int)(0.1 * this->_iSize); i += this->newCrossline());//TODO make setting

	placeBuildings();
}

/*======================================
=============ROAD GENERATION============
========================================*/
float MapGenerator::newCrossline()
{
	int x = randInt(this->_iMinSpaceFromBorder, (this->_iSize - this->_iMinSpaceFromBorder));
	int y = randInt(this->_iMinSpaceFromBorder, (this->_iSize - this->_iMinSpaceFromBorder));

	float iCrossroadCounts = 0;

	int iEndLine = min(x + this->_iMinRoadSize + randInt(0, this->_iRoadExtensionRange), this->_iSize);
	if (!this->rightRoadExistNearby(x, y, iEndLine))
	{
		iCrossroadCounts += 0.25;
		for (int i = x; i < iEndLine; ++i)
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

	iEndLine = max(x - this->_iMinRoadSize - randInt(0, this->_iRoadExtensionRange), -1);
	if (!this->leftRoadExistNearby(x, y, iEndLine))
	{
		iCrossroadCounts += 0.25;
		for (int i = x; i > iEndLine; --i)
		{
			this->_iMap[i][y] = 0;
		}
	}

	iEndLine = max(y - this->_iMinRoadSize - randInt(0, this->_iRoadExtensionRange), -1);
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

bool MapGenerator::rightRoadExistNearby(int x, int y, int iEndLine)
{
	int yMin = max(y - this->_iSpaceBetweenRoads, 0);
	int iRowYMax = min(y + this->_iSpaceBetweenRoads, this->_iSize);

	for (int j = yMin; j < iRowYMax; ++j)
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
	int iRowYMax = min(y + this->_iSpaceBetweenRoads, this->_iSize);

	for (int j = yMin; j < iRowYMax; ++j)
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


/*======================================
===========BUILDING GENERATION==========
========================================*/
void MapGenerator::placeBuildings()
{
	bool bSquareFound;

	do
	{
		bSquareFound = false;

		//Sets up rectangle detection origin
		for (int y = 0; y < this->_iSize && !bSquareFound; ++y)
		{
			for (int x = 0; x < this->_iSize; ++x)
			{
				if (this->_iMap[x][y] == -1)
				{
					Rectangle currentRect = findSquare(x, y);
					fillRectangle(currentRect);
					bSquareFound = true;
					break;
				}
			}
		}
	} while (bSquareFound);
	printMap(0,0,0,0);
}

Rectangle MapGenerator::findSquare(int x, int y)
{
	int xSquareEnd;
	int ySquareEnd;

	for (xSquareEnd = x; xSquareEnd < this->_iSize && this->_iMap[xSquareEnd][y] == -1; ++xSquareEnd);
	for (ySquareEnd = y; ySquareEnd < this->_iSize && this->_iMap[x][ySquareEnd] == -1; ++ySquareEnd)
	{
		int k;
		for (k = x; k < this->_iSize && this->_iMap[k][ySquareEnd] == -1; ++k);
		if (k != xSquareEnd)
		{
			break;
		}
	}

	return Rectangle(x, y, xSquareEnd, ySquareEnd);
}

void MapGenerator::fillRectangle(Rectangle rect)
{
	int iWidth = rect._xEnd - rect._xOrigin + 2;
	int iHeight = rect._yEnd - rect._yOrigin + 2;

	int** rectangleWFCMatrix = new int* [iHeight];
	for (int i = 0; i < iHeight; ++i)
	{
		rectangleWFCMatrix[i] = new int[iWidth];
		for (int j = 0; j < iWidth; ++j)
		{
			if (j == 0 || j==iWidth-1 || i == 0 || i == iHeight - 1)
			{
				rectangleWFCMatrix[i][j] = 0;
			}
			else
			{
				rectangleWFCMatrix[i][j] = -1;
			}
		}
	}


	for (int j = 1; j < iHeight - 1; ++j)
	{
		for (int i = 1; i < iWidth - 1; ++i)
		{
			int iRoadSurroundingCount = 0;
			if (rectangleWFCMatrix[j - 1][i] == 0)
			{
				++iRoadSurroundingCount;
			}
			if (rectangleWFCMatrix[j + 1][i] == 0)
			{
				++iRoadSurroundingCount;
			}
			if (rectangleWFCMatrix[j][i-1] == 0)
			{
				++iRoadSurroundingCount;
			}
			if (rectangleWFCMatrix[j][i+1] == 0)
			{
				++iRoadSurroundingCount;
			}
			if (iRoadSurroundingCount > 2)
			{
				rectangleWFCMatrix[j][i] = 1;
			}
			else
			{
				int value = -2;
				if (randInt(0, 1))
				{
					value=randInt(1, this->_iBuildingMaxHeight);
				}
				rectangleWFCMatrix[j][i] = value;
			}

		}
		cout << endl;
	}

	for (int j = rect._yOrigin, l=1; j < rect._yEnd; ++j, ++l)
	{
		for (int i = rect._xOrigin, k=1; i < rect._xEnd; ++i, k++)
		{
			this->_iMap[i][j] = rectangleWFCMatrix[l][k];
		}
	}

	for (int j = 0; j < iHeight; ++j)
	{
		delete[] rectangleWFCMatrix[j];
	}
	delete[] rectangleWFCMatrix;


}

/*======================================
==================DEBUG=================
========================================*/
void MapGenerator::printMap(int x, int y, int xEnd, int yEnd)
{
	clearConsole();

	for (int j = 0; j < this->_iSize; ++j)
	{
		if (j == 0)
		{
			for (int i = 0; i < this->_iSize; ++i)
			{
				if (i == 0)
				{
					cout << "   ";
				}
				if (i < 10)
				{
					cout << "+" << i << " ";
				}
				else
				{
					cout << i << " ";
				}
			}
			cout << endl;
		}
		for (int i = 0; i < this->_iSize; ++i)
		{
			if (i == 0)
			{
				if (j < 10)
				{
					cout << "+" << j << " ";
				}
				else
				{
					cout << j << " ";
				}
			}
			if (i >= x && i < xEnd && j >= y && j < yEnd)
			{
				setConsoleColor(4);
			}
			else
			{
				setConsoleColor(15);
			}

			if (this->_iMap[i][j] == 0)
			{
				setConsoleColor(1);
			}
			else if (this->_iMap[i][j] == -2)
			{
				setConsoleColor(3);
			}
			else if (this->_iMap[i][j] > 0)
			{
				setConsoleColor(4);
			}

			if (this->_iMap[i][j] >= 0 && this->_iMap[i][j] < 10)
			{
				cout << "+" << this->_iMap[i][j] << " ";
			}
			else
			{
				cout << this->_iMap[i][j] << " ";
			}

		}
		setConsoleColor(15);
		cout << endl;
	}
}

