// resource_file_adder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tools.h"
/*
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}
*/
//#include <iostream>
//#include <string>
//#include <sstream>
//#include <vector>
//#include <SOIL.h>
using namespace std;

#include "ResourceFile.h"

char * get_char_from_file(char * resource_filename, char * filename, int * filesize)
{
	if (resource_filename == NULL || filename == NULL) {
		printf("resource filenames are NULL??\n");
		*filesize = 0;
		return (char*)-1;
	}
	GLuint texture_id;
	char * mallocimg;
	int img_height, img_width, comp;
	ResourceFileReader readTest(resource_filename);
	readTest.read();
	mallocimg = (char *)readTest.getFile(filename); //mallocimg = MEMORY
	*filesize = readTest.getEntryFileSize(filename);
	readTest.deleteEntries();
	return mallocimg;
}



/*
int main(int argc, _TCHAR* argv[])
{
	if (argc == 3){
		char * resource_file = (char *)argv[1];
		char * file_name = (char *)argv[2];
		printf("Writing file %s to resource %s\n", file_name, resource_file );
		ResourceFileWriter writeTest(resource_file);
		writeTest.addEntry(file_name, file_name);
		writeTest.write();
	}
	if (argc == 2 || argc == 3) {
		char * resource_file = (char *)argv[1];
		ResourceFileReader readTest(resource_file);
		readTest.read();
		vector<string> entryNamesList = readTest.getEntryNames();

        printf("File Directory in: %s\n", resource_file);
        for(vector<string>::iterator i = entryNamesList.begin(); i != entryNamesList.end(); i++)
        {
            string entryName = (*i);
            cout << entryName << " (" << readTest.getEntryFileSize(entryName) << " bytes)" << endl;
        }
	}
	else
		printf("USAGE:%s <resource_file> <file to add>\n", argv[0]);
	return 0;
}

*/