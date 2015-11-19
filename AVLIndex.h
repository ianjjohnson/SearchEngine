#ifndef AVL_Index_H
#define AVL_Index_H

#include "IndexInterface.h"
#include "AVL_Tree.cpp"

#include <string>
#include <vector>

class AVLIndex : public IndexInterface{

public:
	virtual ~AVLIndex();

	virtual int addDocument(string name, string author, string date);
	virtual bool addWordForDocument(int documentIndex, string word);

	virtual vector<string> getDocumentsForWord(string word);

	//virtual bool sortComparator(int, int, vector<string>);

	virtual bool writeToFile(string);
	virtual bool readFromFile(string);

	virtual int numWords();
	virtual int numDocs();

	virtual pair<string, string> getAuthorAndTimeForDocNamed(string name);

private:
	vector<pair<string, int> > documentTitles;
	map<string, pair<string, string> > authorsAndDates;
	AVLTree<string> index;

};


#endif