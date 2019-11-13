#ifndef MAP_GENERATOR__H
#define MAP_GENERATOR__H

#include <string>

class MapGenerator
{
	private:
		int _iSize;
		int** _iMap;
		std::string _sSeed;
		int _iRandomFillPercent;
		int _iRandomDirectionPercent;
		int _iRandomNewSegmentPercent;
		int _iSegmentSize;
		int _iSpaceBetweenRoads;
		bool _bUseRandomSeed;

	public:
		MapGenerator() : _iSize(0), _iRandomFillPercent(0), _sSeed(std::string()), _bUseRandomSeed(false), _iMap(nullptr){}
		MapGenerator(int iSize, int iRandomFillPercent);
		MapGenerator(int iSize, int iRandomFillPercent, std::string sSeed);
		virtual ~MapGenerator();
		MapGenerator& operator = (const MapGenerator& mg);
		void printMap();
		int getRandomPercentage();
		bool horizontalRoadExistNearby(int x, int y);
		bool verticalRoadExistNearby(int x, int y);
		void generateMap();
		int** getMap() const;
		int getSize() const;
};


#endif