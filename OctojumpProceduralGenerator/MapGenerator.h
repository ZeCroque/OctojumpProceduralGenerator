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
    float **_iHeatMap;
    std::string _sSeed;
    float _iRandomFillPercent;
    float _fMinRoadSize{};
    float _fMaxRoadSize{};
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
    MapGenerator(int iSize, float iRandomFillPercent, int iBuildingMaxHeight, int iSpaceBetweenRoads);
    MapGenerator(int iSize, float iRandomFillPercent, int iBuildingMaxHeight, int iSpaceBetweenRoads, std::string sSeed);



    //GETTERS/SETTERS
    int **getMap() const;

    //INITIALIZER
    void generateMap();

    //TOOLS
    void MakeHeatMap();

    //ROADS GENERATION
    float newCrossline();
    int getIEndLine(int iEndLine, int a, int b, bool vertical);
    float newRoad(int x, int y, int road_width, int direction);
    bool roadExistNearby(int a, int b, int iEndLine, bool vertical = false);

    //BUILDINGS GENERATION
    void placeBuildings();
    Rectangle findSquare(int x, int y);
    void fillRectangle(const Rectangle &rect);

    //DEBUG
    // void printMap();
    int randInt(int min, int max);
};


#endif