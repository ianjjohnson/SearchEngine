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
	virtual int addDocument(string name);
	virtual bool addWordForDocument(int documentIndex, string word);

	virtual vector<string> getDocumentsForWord(string word);

	virtual bool sortComparator(int, int, vector<string>);

	virtual bool writeToFile(string);
	virtual bool readFromFile(string);

	virtual int numWords();
	virtual int numDocs();

	void print();

private:
	vector< pair<string, int> > documentTitles; //contains each document's title and number of words (for ranking)
	unordered_map<string, vector<int> > index; //The actual index object
	
};

#endif