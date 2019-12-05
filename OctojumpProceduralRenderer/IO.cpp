#include "IO.h"
#include <iostream>
#include <string>
#include <fstream>
#include <shobjidl_core.h>

#define S_INVALID_INPUT "Invalid input. You must enter only digits."
#define S_OUT_OF_RANGE "The number you entered is out of range."

using namespace std;

void stringToInt(string s, int** i)
{
	//Pointe vers le dernier char lu par stoi
	size_t endptr;

	try
	{
		*i = new int();
		**i = stoi(s, &endptr, 10);
	}
	catch (invalid_argument error)
	{
		cout << S_INVALID_INPUT << endl;
		*i=nullptr;
	}
	catch (out_of_range error)
	{
		cout << S_OUT_OF_RANGE << endl;
		*i=nullptr;
	}

	//Vérifie que l'on est pas déjà rentré dans les erreurs précédentes
	//Et qu'il n'y pas de sous chaîne suivant la position d'arrêt de stoi (qu'il n'y a bien que des chiffres)
	if (*i!=nullptr && std::string(s).substr(endptr).length() > 0)
	{
		cout << S_INVALID_INPUT << endl;
		*i = nullptr;
	}
}

int readIntFromKB()
{
	int* iResult=nullptr;
	string sType;
	string sInput;

	cout << endl << "Please enter an integer" << sType << ":" << endl;
	do
	{
		cin >> sInput;
		stringToInt(sInput, &iResult);

		if (iResult==nullptr)
		{
			cout << endl << "Please retry:";
		}
	} while (iResult == nullptr);

	cout << "Number saved." << endl;
	return *iResult;
}

PWSTR readFilePath()
{
	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileDialog* pfd = nullptr;
		hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			COMDLG_FILTERSPEC fileType = {L"City (*.city)", L"*.city" };
			pfd->SetDefaultExtension(L"city");
			pfd->SetFileTypes(1, &fileType);
			hr = pfd->Show(nullptr);

			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pfd->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						return pszFilePath;
					}
					pItem->Release();
				}
			}
			pfd->Release();
		}
		CoUninitialize();
	}
	return nullptr;
}

void saveFile(PWSTR path, int** iMap, int iSize)
{
	ofstream fileStream(path);
	fileStream << iSize << endl;
	for (int j = 0; j < iSize; ++j)
	{
		for (int i = 0; i < iSize; ++i)
		{
			fileStream << iMap[i][j] << ' ';
		}
		fileStream << endl;
	}

	fileStream.close();
}