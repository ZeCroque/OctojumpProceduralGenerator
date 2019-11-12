#ifndef IHM__H
#define IHM__H

#include "MapGenerator.h"

int askWhatToDoNext();
int askSeedMode();
int askSize();
void askForPathAndSave(int** iMap, int iSize);
void clearConsole(char fill = ' ');
void debug();

#endif
