#ifndef LINEAR_INDEX_H
#define LINEAR_INDEX_H

#include "IndexInterface.h"
#include <vector>
#include <string>
#include <unordered_map>

class LinearIndex : public IndexInterface{

public:
	virtual ~LinearIndex();

	virtual bool addDocument(string name, vector<string> relevantWords);

	virtual vector<string> getDocumentsForQuery(vector<string> inDoc, vector<string> notInDoc, bool both);


	virtual bool writeToFile(string);
	virtual bool readFromFile(string);

	void print();

private:
	vector< pair<string, int> > documentTitles;
	unordered_map<string, vector< pair<int, int> > > index;
	
};

#endif