#include <string>
#include "IndexInterface.h"
#include <iostream>
#include <fstream>

using namespace std;


class XMLParser{
public:
	XMLParser(string);
	//bool writeDocToIndex(IndexInterface*);
	bool skipIntroPage();
	//bool readFile(IndexInterface*);
	bool readFileToIndex(string fileName, IndexInterface* index);
	bool isXMLTag(string token);
private:
	ifstream is;
	//The stop words to ignore in search
	vector<string> stopWords;
	bool isStopWord(string token);
};