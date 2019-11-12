#include "pch.h"
#include "IHM.h"
#include "IO.h"
#include "MapGenerator.h"
#include <iostream>
#include <string>

using namespace std;

void writeDisclaimer()
{
	cout << "You are currently running Octojump_Procedural test program. Copyright (C) 2019 Gaëtan Croquefer" << endl << endl;
}

void writeTitle(string s)
{
	cout << endl << "==================================================================" << endl << s << endl << "==================================================================" << endl;
	writeDisclaimer();
}

void clearConsole(char fill)
{
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
}

int askSize()
{
	int iInput;
	bool bInputInRange;

	writeTitle("Octojump Procedural Test");
	cout << "Enter the size of the map to be generated :" << endl;
	do
	{
		iInput = readIntFromKB();
		bInputInRange = !(iInput < 0);
		if (!bInputInRange)
		{
			cout << iInput << "Size cannot be null or negative. Please try again." << endl;
		}
	} while (!bInputInRange);

	return iInput;
}

int askSeedMode()
{
	int iInput;
	bool bInputInRange;

	clearConsole();
	writeTitle("Octojump Procedural Test");
	cout << "Would you like the seed to be :\n[0]Randomly generated (time based)\n[1]User defined" << endl;
	do
	{
		iInput = readIntFromKB();
		bInputInRange = !(iInput < 0 || iInput>1);
		if (!bInputInRange)
		{
			cout << iInput << " is not a valid option. Please try again." << endl;
		}
	} while (!bInputInRange);

	return iInput;
}

int askWhatToDoNext()
{
	int iInput;
	bool bInputInRange;

	clearConsole();
	writeTitle("Octojump Procedural Test");
	cout << "Would you like to :\n[0]Generate again\n[1]Save generated city to file\n[2]Quit" << endl;
	do
	{
		iInput = readIntFromKB();
		bInputInRange = !(iInput<0 || iInput>2);
		if (!bInputInRange)
		{
			cout << iInput << " is not a valid option. Please try again." << endl;
		}
	} while (!bInputInRange);
	return iInput;
}

void askForPathAndSave(int** iMap, int iSize)
{
	PWSTR path;
	do
	{
		path = readFilePath();
		if (path == nullptr)
		{
			cout << "Couldn't retrieve path. Please try again." << endl;
		}
	} while (path == nullptr);
	saveFile(path, iMap, iSize);
}

void debug()
{
	MapGenerator* mg=nullptr;
	int iUsrChoice;
	string sUsrSeed;
	int iSize = askSize();

	do
	{
		iUsrChoice = askSeedMode();
		switch (iUsrChoice)
		{
		case 0:
			mg = new MapGenerator(iSize, 50);
			break;
		case 1:
			cout << "Please enter your seed:" << endl;
			cin >> sUsrSeed;
			mg = new MapGenerator(iSize, 50, sUsrSeed);
			break;
		}
		mg->generateMap();
		iUsrChoice = askWhatToDoNext();
	} while (iUsrChoice == 0);

	if (iUsrChoice == 1)
	{
		askForPathAndSave(mg->getMap(), iSize);
	}

	if (mg != nullptr)
	{
		delete mg;
	}
}
