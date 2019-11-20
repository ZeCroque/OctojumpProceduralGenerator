#ifndef MAP_GENERATOR__H
#define MAP_GENERATOR__H

#include <string>
#include <algorithm>
#include "Rectangle.h"
#include <random>

class MapGenerator {
private:
    int _iSize;
    int **_iMap;
    std::string _sSeed;
    int _iRandomFillPercent;
    int _iMinRoadSize{};
    int _iRoadExtensionRange{};
    int _iSpaceBetweenRoads{};
    int _iMinSpaceFromBorder{};
    int _iBuildingMaxHeight{};
    std::mt19937 *_generator{};
    bool _bUseRandomSeed;

public:
    //FORME CANONIQUE DE COPLIEN
    MapGenerator();

    virtual ~MapGenerator();

    MapGenerator &operator=(const MapGenerator &mg);

    //CONSTRUCTEURS SURCHARGES
    MapGenerator(int iSize, int iRandomFillPercent);

    MapGenerator(int iSize, int iRandomFillPercent, std::string sSeed);

    //GETTERS/SETTERS
    int **getMap() const;

    int getSize() const;

    //INITIALIZER
    void generateMap();

    //ROADS GENERATION
    float newCrossline();

    bool rightRoadExistNearby(int x, int y, int iEndLine);

    bool leftRoadExistNearby(int x, int y, int iEndLine);

    bool upRoadExistNearby(int x, int y, int iEndLine);

    bool downRoadExistNearby(int x, int y, int iEndLine);

    //BUILDINGS GENERATION
    void placeBuildings();

    Rectangle findSquare(int x, int y);

    void fillRectangle(const Rectangle &rect);

    //DEBUG
    void printMap(int x, int y, int xEnd, int yEnd);
    int randInt(int min, int max);
};


#endif