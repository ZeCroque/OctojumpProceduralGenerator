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
		bool _bUseRandomSeed;

	public:
		MapGenerator() : _iSize(0), _iRandomFillPercent(0), _sSeed(std::string()), _bUseRandomSeed(false), _iMap(nullptr){}
		MapGenerator(int iSize, int iRandomFillPercent);
		MapGenerator(int iSize, int iRandomFillPercent, std::string sSeed);
		virtual ~MapGenerator();
		MapGenerator& operator = (const MapGenerator& mg);
		void generateMap();
		int** getMap() const;
		int getSize() const;
};



#endif