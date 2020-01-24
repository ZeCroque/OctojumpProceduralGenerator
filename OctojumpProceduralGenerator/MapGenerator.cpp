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
                               _iMap(nullptr), _iHeatMap(nullptr) {}

MapGenerator::~MapGenerator() {
    for (int i = 0; i < this->_iSize; ++i) {
        delete[] this->_iMap[i];
    }
    delete[] this->_iMap;

    for (int i = 0; i < this->_iSize; ++i) {
        delete[] this->_iHeatMap[i];
    }
    delete[] this->_iHeatMap;
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
MapGenerator::MapGenerator(int iSize, float iRandomFillPercent, int iBuildingMaxHeight, int iSpaceBetweenRoads)
        : _iSize(iSize),
          _iRandomFillPercent(iRandomFillPercent), _iBuildingMaxHeight(iBuildingMaxHeight),
          _iSpaceBetweenRoads(iSpaceBetweenRoads), _sSeed(string()), _bUseRandomSeed(true) {
    this->_iMap = new int *[_iSize];
    for (int i = 0; i < _iSize; ++i) {
        this->_iMap[i] = new int[_iSize];
        for (int j = 0; j < _iSize; ++j) {
            this->_iMap[i][j] = -1;
        }
    }

    this->_iHeatMap = new float *[_iSize];
    for (int i = 0; i < _iSize; ++i) {
        this->_iHeatMap[i] = new float[_iSize];
        for (int j = 0; j < _iSize; ++j) {
            this->_iHeatMap[i][j] = -1;
        }
    }
}

MapGenerator::MapGenerator(int iSize, float iRandomFillPercent, int iBuildingMaxHeight, int iSpaceBetweenRoads,
                           std::string sSeed) : MapGenerator(iSize,
                                                             iRandomFillPercent, iBuildingMaxHeight,
                                                             iSpaceBetweenRoads) {
    this->_bUseRandomSeed = false;
    this->_sSeed = std::move(sSeed);
}

/*======================================
=============GETTERS/SETTERS===========
========================================*/
int **MapGenerator::getMap() const {
    return this->_iMap;
}


/*======================================
=============UTILITIES===========
========================================*/
int MapGenerator::randInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(*(this->_generator));
}


/*======================================
=============INITIALIZER===========
========================================*/
void MapGenerator::generateMap() {
    if (this->_bUseRandomSeed) {
        char tmp[80] = {0};
        time_t rawtime = time(nullptr);
        tm timeinfo{};
        localtime_s(&timeinfo, &rawtime);
        strftime(tmp, sizeof(tmp), "%c", &timeinfo);
        this->_sSeed = string(tmp);
    }

    std::seed_seq seed(this->_sSeed.begin(), this->_sSeed.end());
    this->_generator = new std::mt19937(seed);

    /*
    //DEBUG
    this->_iRandomFillPercent = 0.2;
    this->_iBuildingMaxHeight = 35;
    this->_iSpaceBetweenRoads = 4;
    */

    this->_iMinSpaceFromBorder = (int) (this->_iSize * 0.05);
    this->_fMinRoadSize = 0.60;
    this->_fMaxRoadSize = 0.90;


    for (float i = 0; i < (this->_iRandomFillPercent / 100) * (float) this->_iSize;) {
        i += this->newCrossline();
    }

    for (int x = 0; x < this->_iSize; x++) {
        for (int y = 0; y < this->_iSize; ++y) {
            if (this->_iMap[x][y] == -2) {
                this->_iMap[x][y] = 0;
            }
        }
    }


    for (int i = 0; i < this->_iSize; i++) {  //Make border
        for (int y = 0; y < 3; y++) {
            this->_iMap[i][y] = 0;
            this->_iMap[y][i] = 0;
            this->_iMap[i][this->_iSize - 1 - y] = 0;
            this->_iMap[this->_iSize - 1 - y][i] = 0;
        }
    }

    int a = this->_iSize / 2;
    int b = this->_iSize / 2;
    for (int i = 0; i < 1; i++) {
        this->MakeHeatMap(a, b);
        a = this->randInt(int(this->_iSize * 0.15), (this->_iSize - int(this->_iSize * 0.15))) - 1;
        b = this->randInt(int(this->_iSize * 0.15), (this->_iSize - int(this->_iSize * 0.15))) - 1;
    }

    placeBuildings();
}


/*======================================
=================TOOLS==================
========================================*/
float distPoint(int a, int b, int x, int y) {
    return float(sqrt(pow(a - x, 2) + pow(b - y, 2)));
}

void MapGenerator::MakeHeatMap(int a, int b) {


    float max_distance = min({distPoint(a, b, 0, 0), distPoint(a, b, 0, this->_iSize), distPoint(a, b, this->_iSize, 0),
                              distPoint(a, b, this->_iSize, this->_iSize)});


    for (int x = 0; x < this->_iSize; ++x) {
        for (int y = 0; y < this->_iSize; ++y) {
            auto rand = this->randInt(-int(this->_iBuildingMaxHeight * 0.10), int(this->_iBuildingMaxHeight * 0.10));
            this->_iHeatMap[x][y] = float(this->_iBuildingMaxHeight * 0.10) + rand;
            //cout << float(this->_iBuildingMaxHeight * 0.50) + rand << endl;
        }
    }

    float heat;
    for (int x = 0; x < this->_iSize; ++x) {
        for (int y = 0; y < this->_iSize; ++y) {
            heat = abs((((float) (this->_iBuildingMaxHeight * 0.50) * distPoint(a, b, x, y)) / (max_distance)) -
                       (float) (this->_iBuildingMaxHeight * 0.50));
            //cout << heat << endl;
            if (heat > 0) {
                this->_iHeatMap[x][y] += heat;
            }
        }
    }
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

int MapGenerator::getIEndLine(int iEndLine, int a, int b, bool vertical) {
    int min = 1;
    int max = NULL;
    int end = this->_iSize - 1;

    if (a > iEndLine) {
        end = a;
        a = 1;
    }

    max = end;
    if (!vertical) {
        for (int i = a; i < end; ++i) {
            if (this->_iMap[i][b] == 0 && i < iEndLine) {
                min = i;
            } else if (this->_iMap[i][b] == 0 && i > iEndLine) {
                max = i;
                break;
            }
        }
    } else {
        for (int i = a; i < end; ++i) {
            if (this->_iMap[b][i] == 0 && i < iEndLine) {
                min = i;
            } else if (this->_iMap[b][i] == 0 && i > iEndLine) {
                max = i;
                break;
            }
        }
    }

    int result;
    if (end != 1) {
        if (iEndLine - min <= max - iEndLine) {
            result = min;
        } else {
            result = max;
        }
    } else {
        if (min - iEndLine <= iEndLine - max) {
            result = min;
        } else {
            result = max;
        }
    }
    return result;
}

float MapGenerator::newRoad(int x, int y, int road_width, int direction) {
    int a, b;
    int tmp_iEndLine;
    int vertical;
    float MinRoadSize = this->_fMinRoadSize;
    float MaxRoadSize = this->_fMaxRoadSize;


    if (direction == 1 || direction == 3) {
        a = x;
        b = y;
        vertical = false;
    } else {
        a = y;
        b = x;
        vertical = true;
    }
    if (road_width == 0) {
        MinRoadSize /= 2;
        MaxRoadSize /= 2;
    }

    if (direction == 1 || direction == 2) {
        tmp_iEndLine = randInt(a + (int) (float(this->_iSize - a) * MinRoadSize),
                               a + (int) (float(this->_iSize - a) * MaxRoadSize));
    } else {
        tmp_iEndLine = a - randInt((int) ((float) a * MinRoadSize), (int) ((float) a * MaxRoadSize));
    }
    int iEndLine = this->getIEndLine(tmp_iEndLine, a, b, vertical);
    a = this->getIEndLine(a, tmp_iEndLine, b, vertical);
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
	int trash[4] = { 0 };
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
    float heat = 0;
    float i = 0;

    for (int y = rect._yOrigin; y < rect._yEnd; ++y) {
        for (int x = rect._xOrigin; x < rect._xEnd; ++x) {
            i++;
            heat += this->_iHeatMap[x][y];
        }
    }
    heat = heat / i;

    for (int y = rect._yOrigin; y < rect._yEnd; ++y) {
        for (int x = rect._xOrigin; x < rect._xEnd; ++x) {
            this->_iMap[x][y] = int(this->_iHeatMap[x][y]); //max(1, (int(heat) * 2 +
//                                        int(this->_iHeatMap[x][y]) +
//                                        (int(this->_iHeatMap[x][y]) + randInt(-10, 10))) / 4);
        }
    }
}