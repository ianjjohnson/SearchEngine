#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include "IndexInterface.h"
#include <vector>
#include <string>
#include <unordered_map>

class HashIndex : public IndexInterface{

public:
	virtual ~HashIndex();

	virtual int addDocument(string name, string author, string date);
	virtual bool addWordForDocument(int documentIndex, string word);

	virtual vector<string> getDocumentsForWord(string word);

	//virtual bool sortComparator(int, int, vector<string>);

	virtual bool writeToFile(string);
	virtual bool readFromFile(string);

	virtual int numWords();
	virtual int numDocs();

	void print();

	virtual pair<string, string> getAuthorAndTimeForDocNamed(string name);

private:
	vector< pair<string, int> > documentTitles; //contains each document's title and number of words (for ranking)
	map<string, pair<string, string> > authorsAndDates;
	unordered_map<string, vector<int> > index; //The actual index object
	
};

#endif