#ifndef INDEX_INTERFACE_H
#define INDEX_INTERFACE_H

#include <string>
#include <vector>

using namespace std;

class IndexInterface{
public:
	virtual ~IndexInterface() {}

	virtual bool addDocument(string name, vector<string> relevantWords) = 0;

	virtual vector<string> getDocumentsForQuery(vector<string> inDoc, vector<string> notInDoc, bool both) = 0;


	virtual bool writeToFile(string) = 0;
	virtual bool readFromFile(string) = 0;

private:
	vector<pair<string, int> > documentTitles;

};

#endif