#ifndef QUERY_HANDLER_H
#define QUERY_HANDLER_H

#include "IndexInterface.h"
#include <vector>
#include <string>

using namespace std;

class QueryHandler{

public:
	void handleQuery(vector<string> query, IndexInterface* ind);
private:
	IndexInterface *ind;
	void doSearch(vector<string> words);
};

#endif