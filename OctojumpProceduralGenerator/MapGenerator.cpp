#include "pch.h"
#include "MapGenerator.h"
#include <ctime>
#include <string>
#include <algorithm>
#include <iostream>
#include <utility>
#include "IHM.h"
#include "Rectangle.h"
#include <random>

using namespace std;


/*======================================
======FORME CANONIQUE DE COPLIEN======
========================================*/
MapGenerator::MapGenerator() : _iSize(0), _iRandomFillPercent(0), _sSeed(std::string()), _bUseRandomSeed(false),
                               _iMap(nullptr) {}

MapGenerator::~MapGenerator() {
    for (int i = 0; i < this->_iSize; ++i) {
        delete[] this->_iMap[i];
    }
    delete[] this->_iMap;
}

MapGenerator &MapGenerator::operator=(const MapGenerator &mg) {
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
MapGenerator::MapGenerator(int iSize, int iRandomFillPercent) : _iSize(iSize), _iRandomFillPercent(iRandomFillPercent),
                                                                _sSeed(string()), _bUseRandomSeed(true) {
    this->_iMap = new int *[_iSize];

    for (int i = 0; i < _iSize; ++i) {
        this->_iMap[i] = new int[_iSize];
        for (int j = 0; j < _iSize; ++j) {
            this->_iMap[i][j] = -1;
        }
    }
}

MapGenerator::MapGenerator(int iSize, int iRandomFillPercent, std::string sSeed) : MapGenerator(iSize,
                                                                                                iRandomFillPercent) {
    this->_bUseRandomSeed = false;
    this->_sSeed = std::move(sSeed);
}

/*======================================
=============GETTERS/SETTERS===========
========================================*/
int **MapGenerator::getMap() const {
    return this->_iMap;
}

int MapGenerator::getSize() const {
    return this->_iSize;
}

/*======================================
=============UTILITIES===========
========================================*/
int MapGenerator::randInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(*(this->_generator));
}

void getShape(){
    
}

/*======================================
=============INITIALIZER===========
========================================*/
void MapGenerator::generateMap() {
    if (this->_bUseRandomSeed) {
        char tmp[80] = {0};
        time_t rawtime = time(nullptr);
        tm timeinfo;
        localtime_s(&timeinfo, &rawtime);
        strftime(tmp, sizeof(tmp), "%c", &timeinfo);
        this->_sSeed = string(tmp);
    }

    std::seed_seq seed(this->_sSeed.begin(), this->_sSeed.end());
    this->_generator = new std::mt19937(seed);

    //DEBUG
    this->_iRandomFillPercent = 25;
    this->_iSpaceBetweenRoads = 5;
    this->_iMinSpaceFromBorder = (int) (this->_iSize * 0.1);
    this->_fMinRoadSize = 0.60;
    this->_fMaxRoadSize = 0.90;
    this->_iBuildingMaxHeight = 9;

    for (float i = 0; i < (0.2 * this->_iSize);) {
        i += this->newCrossline();
    }//TODO make setting

    for (int x = 0; x < this->_iSize; x++) {
        for (int y = 0; y < this->_iSize; ++y) {
            if (this->_iMap[x][y] == -2) {
                this->_iMap[x][y] = 0;
            }
        }
    }
    placeBuildings();
}

/*======================================
=============ROAD GENERATION============
========================================*/
bool MapGenerator::roadExistNearby(int a, int b, int iEndLine, bool vertical) {
    if (iEndLine < a) {
        int tmp = iEndLine;
        iEndLine = a;
        a = tmp;
    }
    int bMin = max(b - this->_iSpaceBetweenRoads, 0);
    int bMax = min(b + this->_iSpaceBetweenRoads, this->_iSize);

    for (int j = bMin; j < bMax; ++j) {
        int last = -1;
        for (int i = a; i < iEndLine; ++i) {
            if (!vertical) {
                if (!this->_iMap[i][j] && !last) {
                    return true;
                }
                last = this->_iMap[i][j];
            } else {
                if (!this->_iMap[j][i] && !last) {
                    return true;
                }
                last = this->_iMap[j][i];
            }
        }
    }
    return false;
}

float MapGenerator::newRoad(int x, int y, int road_width, int direction) {
    int a, b;
    int iEndLine;
    int vertical;
    if (direction == 1 || direction == 3) {
        a = x;
        b = y;
        vertical = false;
    } else {
        a = y;
        b = x;
        vertical = true;
    }
    if (direction == 1 || direction == 2) {
        iEndLine = randInt(a + (int) (float(this->_iSize - a) * this->_fMinRoadSize),
                           a + (int) (float(this->_iSize - a) * this->_fMaxRoadSize));
    } else {
        iEndLine = a - randInt((int) ((float) a * this->_fMinRoadSize), (int) ((float) a * this->_fMaxRoadSize));
    }

    int road_value;
    if (!this->roadExistNearby(a, b, iEndLine, vertical)) {
        int new_b = max(b - road_width, 0);
        int bMax = min(b + road_width, this->_iSize);
        if (a > iEndLine) {
            int tmp = iEndLine;
            iEndLine = a;
            a = tmp;
        }

        do {
            if (new_b == b) {
                road_value = 0;
            } else {
                road_value = -2;
            }
            if (!vertical) {
                for (int i = a; i < iEndLine; ++i) {
                    this->_iMap[i][new_b] = road_value;
                }
            } else {
                for (int i = a; i < iEndLine; ++i) {
                    this->_iMap[new_b][i] = road_value;
                }
            }
            ++new_b;
        } while (new_b < bMax);
        return 0.25;
    }
    return 0;
}

float MapGenerator::newCrossline() {
    int x = this->randInt(this->_iMinSpaceFromBorder, (this->_iSize - this->_iMinSpaceFromBorder));
    int y = this->randInt(this->_iMinSpaceFromBorder, (this->_iSize - this->_iMinSpaceFromBorder));
    int road_width = randInt(0, 1);

    float iCrossroadCounts = 0;

    int stack[4] = {1, 2, 3, 4};
    int trash[4];
    int rnd;
    bool tmp;

    for (int i = 0; i < 4;) {
        rnd = stack[randInt(0, 3)];
        tmp = false;
        for (int j : trash) {
            if (rnd == j) {
                tmp = true;
            }
        }
        if (!tmp) {
            iCrossroadCounts += this->newRoad(x, y, road_width, rnd);
            trash[i] = rnd;
            i++;
        }
    }

    return iCrossroadCounts;
}


/*======================================
===========BUILDING GENERATION==========
========================================*/
void MapGenerator::placeBuildings() {
    bool bSquareFound;

    do {
        bSquareFound = false;

        //Sets up rectangle detection origin
        for (int y = 0; y < this->_iSize && !bSquareFound; ++y) {
            for (int x = 0; x < this->_iSize; ++x) {
                if (this->_iMap[x][y] == -1) {
                    Rectangle currentRect = findSquare(x, y);
                    fillRectangle(currentRect);
                    bSquareFound = true;
                    break;
                }
            }
        }
    } while (bSquareFound);
    printMap(0, 0, 0, 0);
}

Rectangle MapGenerator::findSquare(int x, int y) {
    int xSquareEnd;
    int ySquareEnd;

    for (xSquareEnd = x; xSquareEnd < this->_iSize && this->_iMap[xSquareEnd][y] == -1; ++xSquareEnd);
    for (ySquareEnd = y; ySquareEnd < this->_iSize && this->_iMap[x][ySquareEnd] == -1; ++ySquareEnd) {
        int k;
        for (k = x; k < this->_iSize && this->_iMap[k][ySquareEnd] == -1; ++k);
        if (k != xSquareEnd) {
            break;
        }
    }

    return Rectangle(x, y, xSquareEnd, ySquareEnd);
}

void MapGenerator::fillRectangle(const Rectangle &rect) {
    int iWidth = rect._xEnd - rect._xOrigin + 2;
    int iHeight = rect._yEnd - rect._yOrigin + 2;

    int **rectangleWFCMatrix = new int *[iHeight];
    for (int i = 0; i < iHeight; ++i) {
        rectangleWFCMatrix[i] = new int[iWidth];
        for (int j = 0; j < iWidth; ++j) {
            if (j == 0 || j == iWidth - 1 || i == 0 || i == iHeight - 1) {
                rectangleWFCMatrix[i][j] = 0;
            } else {
                rectangleWFCMatrix[i][j] = -1;
            }
        }
    }


    for (int j = 1; j < iHeight - 1; ++j) {
        for (int i = 1; i < iWidth - 1; ++i) {
            int iRoadSurroundingCount = 0;
            if (rectangleWFCMatrix[j - 1][i] == 0) {
                ++iRoadSurroundingCount;
            }
            if (rectangleWFCMatrix[j + 1][i] == 0) {
                ++iRoadSurroundingCount;
            }
            if (rectangleWFCMatrix[j][i - 1] == 0) {
                ++iRoadSurroundingCount;
            }
            if (rectangleWFCMatrix[j][i + 1] == 0) {
                ++iRoadSurroundingCount;
            }
            if (iRoadSurroundingCount > 2) {
                rectangleWFCMatrix[j][i] = 1;
            } else {
                int value = -2;
                if (this->randInt(0, 1)) {
                    value = this->randInt(1, this->_iBuildingMaxHeight);
                }
                rectangleWFCMatrix[j][i] = value;
            }

        }
        cout << endl;
    }

    for (int j = rect._yOrigin, l = 1; j < rect._yEnd; ++j, ++l) {
        for (int i = rect._xOrigin, k = 1; i < rect._xEnd; ++i, k++) {
            this->_iMap[i][j] = rectangleWFCMatrix[l][k];
        }
    }

    for (int j = 0; j < iHeight; ++j) {
        delete[] rectangleWFCMatrix[j];
    }
    delete[] rectangleWFCMatrix;


}

/*======================================
==================DEBUG=================
========================================*/
void MapGenerator::printMap(int x, int y, int xEnd, int yEnd) {
    clearConsole();

    for (int j = 0; j < this->_iSize; ++j) {
        if (j == 0) {
            for (int i = 0; i < this->_iSize; ++i) {
                if (i == 0) {
                    cout << "   ";
                }
                if (i < 10) {
                    cout << "+" << i << " ";
                } else {
                    cout << i << " ";
                }
            }
            cout << endl;
        }
        for (int i = 0; i < this->_iSize; ++i) {
            if (i == 0) {
                if (j < 10) {
                    cout << "+" << j << " ";
                } else {
                    cout << j << " ";
                }
            }
            if (i >= x && i < xEnd && j >= y && j < yEnd) {
                setConsoleColor(4);
            } else {
                setConsoleColor(15);
            }

            if (this->_iMap[i][j] == 0) {
                setConsoleColor(1);
            } else if (this->_iMap[i][j] == -2) {
                setConsoleColor(3);
            } else if (this->_iMap[i][j] > 0) {
                setConsoleColor(4);
            }

            if (this->_iMap[i][j] >= 0 && this->_iMap[i][j] < 10) {
                cout << "+" << this->_iMap[i][j] << " ";
            } else {
                cout << this->_iMap[i][j] << " ";
            }

        }
        setConsoleColor(15);
        cout << endl;
    }
}

