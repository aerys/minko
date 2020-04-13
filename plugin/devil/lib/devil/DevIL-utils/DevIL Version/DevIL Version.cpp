// Updates the version numbers in all DevIL files.
//  This is definitely not the "best" or prettiest way to do it, and this is
//  a very linear file.  The problem is that each file that needs to be updated
//  usually has a different way of representing the version number.  For
//  instance, some have the version number as x.x.x, while others have
//  x,x,x.  The resource files even have these mixed.  This program gets the
//  job done, though!  I was missing updates of version numbers in files for
//  each release, so running this should fix it.


#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int CheckLine(string line, fstream *file, int pos);


int main() 
{
	string	oldversion[3];
	string	newversion[3];
	string	OldString, NewString;
	string	OldLine, NewLine;
	fstream	file;
	string	line;
	//int		pos1, pos2;
	streamoff pos1, pos2;
	//fpos_t pos1, pos2;

	//oldversion[0] = '1';
	//oldversion[1] = '7';
	//oldversion[2] = '5';

	//newversion[0] = '1';
	//newversion[1] = '7';
	//newversion[2] = '4';

	file.open("Version Numbers.txt", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open Version Numbers.txt" << endl;
		return 0;
	}
	file >> line;
	oldversion[0] = line[0];
	oldversion[1] = line[1];
	oldversion[2] = line[2];
	file >> line;
	newversion[0] = line[0];
	newversion[1] = line[1];
	newversion[2] = line[2];

	file.close();
	file.clear();


	OldString = oldversion[0] + "." + oldversion[1] + "." + oldversion[2];
	NewString = newversion[0] + "." + newversion[1] + "." + newversion[2];

//
//
// src-IL/msvc9/IL.rc and src-IL/msvc8/IL.rc
//
//
	/*for (int i = 0; i < 2; i++) {
		string ILrc[2] = {"src-IL/msvc8/IL.rc", "src-IL/msvc9/IL.rc"};
		file.open(ILrc[i].c_str(), fstream::in | fstream::out);
		if (!file.is_open()) {
			cout << "Unable to open " << ILrc[i] << endl;
			return 0;
		}
		do {
			pos1 = file.tellg();//.seekpos();
			getline(file, line);
			int linelen = line.length();
			pos2 = file.tellg();//.seekpos();

			OldLine = string(" FILEVERSION 0,") + string(oldversion[0]) + "," + string(oldversion[1]) + "," + string(oldversion[2]);
			if (line == OldLine) {
				file.seekp(pos1, ios_base::beg);
				NewLine = string(" FILEVERSION 0,") + string(newversion[0]) + "," + string(newversion[1]) + "," + string(newversion[2]);
				file << NewLine;
				file.seekg(pos2, ios_base::beg);
				int j = file.tellg();
			}
			OldLine = string(" PRODUCTVERSION 0,") + string(oldversion[0]) + "," + string(oldversion[1]) + "," + string(oldversion[2]);
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = string(" PRODUCTVERSION 0,") + string(newversion[0]) + "," + string(newversion[1]) + "," + string(newversion[2]);
				file << NewLine;
				file.seekg(pos2);
			}
			OldLine = string("            VALUE \"FileVersion\", \"") + OldString + "\"";
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = string("            VALUE \"FileVersion\", \"") + NewString + "\"";
				file << NewLine;
				file.seekg(pos2);
			}
			OldLine = "            VALUE \"ProductVersion\", \"" + OldString + " Ansi\"";
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = "            VALUE \"ProductVersion\", \"" + NewString + " Ansi\"";
				file << NewLine;
				file.seekg(pos2);
			}
			OldLine = "    IDC_OPENIL              \"Developer's Image Library, Version " + OldString + "\"";
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = "    IDC_OPENIL              \"Developer's Image Library, Version " + NewString + "\"";
				file << NewLine;
				file.seekg(pos2);
			}
		} while (!file.eof());
		file.close();
		file.clear();
	}*/



//
//
// src-IL/msvc9/IL Unicode.rc and src-IL/msvc/IL Unicode.rc
//
//
	for (int i = 0; i < 2; i++) {
		string ILrc[2] = {"src-IL/msvc9/IL Unicode.rc", "src-IL/msvc8/IL Unicode.rc"};
		file.open(ILrc[i].c_str(), fstream::in | fstream::out);
		if (!file.is_open()) {
			cout << "Unable to open IL Unicode.rc" << endl;
			return 0;
		}
		do {
			pos1 = file.tellg();
			getline(file, line);
			pos2 = file.tellg();

			OldLine = string(" FILEVERSION 0,") + string(oldversion[0]) + "," + string(oldversion[1]) + "," + string(oldversion[2]);
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = string(" FILEVERSION 0,") + string(newversion[0]) + "," + string(newversion[1]) + "," + string(newversion[2]);
				file << NewLine;
				file.seekg(pos1);
				file.seekp(pos1);
			}
			OldLine = string(" PRODUCTVERSION 0,") + string(oldversion[0]) + "," + string(oldversion[1]) + "," + string(oldversion[2]);
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = string(" PRODUCTVERSION 0,") + string(newversion[0]) + "," + string(newversion[1]) + "," + string(newversion[2]);
				file << NewLine;
				file.seekg(pos2);
			}
			OldLine = string("            VALUE \"FileVersion\", \"") + OldString + "\"";
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = string("            VALUE \"FileVersion\", \"") + NewString + "\"";
				file << NewLine;
				file.seekg(pos2);
			}
			OldLine = "            VALUE \"ProductVersion\", \"" + OldString + " Unicode\"";
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = "            VALUE \"ProductVersion\", \"" + NewString + " Unicode\"";
				file << NewLine;
				file.seekg(pos2);
			}
			OldLine = "    IDC_OPENIL              \"Developer's Image Library, Version " + OldString + "\"";
			if (line == OldLine) {
				file.seekp(pos1);
				NewLine = "    IDC_OPENIL              \"Developer's Image Library, Version " + NewString + "\"";
				file << NewLine;
				file.seekg(pos2);
			}
		} while (!file.eof());
		file.close();
		file.clear();
	}



//
//
// configure.ac
//
//
	file.open("configure.ac", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open configure.ac" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "	" + OldString + ",";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "	" + NewString + ",";
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// IL/il.h
//
//
	file.open("include/IL/il.h", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open include/IL/il.h" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "#define IL_VERSION_" + oldversion[0] + "_" + oldversion[1] + "_" + oldversion[2] + " 1";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "#define IL_VERSION_" + newversion[0] + "_" + newversion[1] + "_" + newversion[2] + " 1";
			file << NewLine;
			file.seekg(pos2);
		}
		OldLine = "#define IL_VERSION       " + oldversion[0] + oldversion[1] + oldversion[2];
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "#define IL_VERSION       " + newversion[0] + newversion[1] + newversion[2];
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// IL/ilu.h
//
//
	file.open("include/IL/ilu.h", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open include/IL/ilu.h" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "#define ILU_VERSION_" + oldversion[0] + "_" + oldversion[1] + "_" + oldversion[2] + " 1";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "#define ILU_VERSION_" + newversion[0] + "_" + newversion[1] + "_" + newversion[2] + " 1";
			file << NewLine;
			file.seekg(pos2);
		}
		OldLine = "#define ILU_VERSION       " + oldversion[0] + oldversion[1] + oldversion[2];
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "#define ILU_VERSION       " + newversion[0] + newversion[1] + newversion[2];
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// IL/ilut.h
//
//
	file.open("include/IL/ilut.h", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open include/IL/ilut.h" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "#define ILUT_VERSION_" + oldversion[0] + "_" + oldversion[1] + "_" + oldversion[2] + " 1";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "#define ILUT_VERSION_" + newversion[0] + "_" + newversion[1] + "_" + newversion[2] + " 1";
			file << NewLine;
			file.seekg(pos2);
		}
		OldLine = "#define ILUT_VERSION       " + oldversion[0] + oldversion[1] + oldversion[2];
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "#define ILUT_VERSION       " + newversion[0] + newversion[1] + newversion[2];
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// README
//
//
	file.open("README", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open README" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "Developer's Image Library version " + OldString + " Readme, Notes and Quick Use";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "Developer's Image Library version " + NewString + " Readme, Notes and Quick Use";
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// include/IL/config.h.win
//
//
	file.open("include/IL/config.h.win", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open include/IL/config.h.win" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "//#define IL_VERSION \"" + OldString + "\"";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "//#define IL_VERSION \"" + NewString + "\"";
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// src-IL/src/il_states.c
//
//
	file.open("src-IL/src/il_states.c", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open src-IL/src/il_states.c" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "ILstring _ilVersion		= IL_TEXT(\"Developer's Image Library (DevIL) " + OldString + "\");";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "ILstring _ilVersion		= IL_TEXT(\"Developer's Image Library (DevIL) " + NewString + "\");";
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// src-ILU/src/ilu_states.c
//
//
	file.open("src-ILU/src/ilu_states.c", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open src-ILU/src/ilu_states.c" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "ILconst_string _iluVersion	= IL_TEXT(\"Developer's Image Library Utilities (ILU) " + OldString + " \" IL_TEXT(__DATE__));";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "ILconst_string _iluVersion	= IL_TEXT(\"Developer's Image Library Utilities (ILU) " + NewString + " \" IL_TEXT(__DATE__));";
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();



//
//
// src-ILUT/src/ilut_states.c
//
//
	file.open("src-ILUT/src/ilut_states.c", fstream::in | fstream::out);
	if (!file.is_open()) {
		cout << "Unable to open src-ILUT/src/ilut_states.c" << endl;
		return 0;
	}
	do {
		pos1 = file.tellg();
		getline(file, line);
		pos2 = file.tellg();

		OldLine = "ILconst_string _ilutVersion	= IL_TEXT(\"Developer's Image Library Utility Toolkit (ILUT) " + OldString + " \");";
		if (line == OldLine) {
			file.seekp(pos1);
			NewLine = "ILconst_string _ilutVersion	= IL_TEXT(\"Developer's Image Library Utility Toolkit (ILUT) " + NewString + " \");";
			file << NewLine;
			file.seekg(pos2);
		}
	} while (!file.eof());
	file.close();
	file.clear();


	cout << "Successfully updated all files!" << endl;

	return 0;
}

