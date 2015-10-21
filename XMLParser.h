#include <string>
#include "IndexInterface.h"
#include <iostream>
#include <fstream>

using namespace std;


class XMLParser{
public:
	XMLParser(string);
	bool writeDocToIndex(IndexInterface*);
	bool skipIntroPage();
	bool readFile(IndexInterface*);
	bool readFileToIndex(string fileName, IndexInterface* index);
	bool isNotXMLTag(string token);
private:
	ifstream is;
	vector<string> stopWords;
	bool isStopWord(string);
};