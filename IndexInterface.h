#include <string>
#include <vector>

using namespace std;

class IndexInterface{
public:
	virtual ~IndexInterface() {}

	virtual bool addDocument(string name, vector<string> relevantWords) = 0;

	virtual vector<string> getDocumentsForQuery(vector<string> inDoc, vector<string> notInDoc) = 0;


	virtual bool writeToFile(string) = 0;
	virtual bool readFromFile(string) = 0;

private:
	vector<pair<string, int> > documentTitles;

};