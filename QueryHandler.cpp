#include "QueryHandler.h"
#include "porter2_stemmer.h"
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

void QueryHandler::handleQuery(vector<string> query,  IndexInterface* indInput){
	ind = indInput;
	doSearch(query);
}

/*
Searches the index for a series of search queries in argv
@param argc - the number of command line args
@param argv - the list of 
*/
void QueryHandler::doSearch(vector<string> words){

   	vector<string> allow;
   	vector<string> disallow;

   	bool first = true;

   	int argIndex = 1;

	if(words.at(0) == "AND"){

		while(argIndex < words.size() && words.at(argIndex) != "NOT"){

			cout << words.at(argIndex) << "AND" << endl;

			vector<string> otherSet = ind->getDocumentsForWord(words.at(argIndex));

			if(first){
				first = false;
				allow = otherSet;
				argIndex++;
				continue;
			} else {

				map<int, string> finalSet;
			
				for(int i = 0; i < allow.size(); i++){
					string currentDocName = allow.at(i);
					vector<string>::iterator docInSecondList = find(otherSet.begin(), otherSet.end(), currentDocName);

					if(docInSecondList != otherSet.end()){
						int indexSum = i + (docInSecondList - otherSet.begin());
						pair<int, string> newMapItem;
						newMapItem.first = indexSum;
						newMapItem.second = currentDocName;
						finalSet.insert(newMapItem);
					}
				}

				allow.clear();

				for(map<int, string>::iterator it = finalSet.begin(); it != finalSet.end(); ++it)
						allow.push_back(it->second);

				
			}

			argIndex++;
		}


	} else if(words.at(0) == "OR"){

		while(argIndex < words.size() && words.at(argIndex) != "NOT"){

			cout << words.at(argIndex) << "OR" << endl;

			vector<string> otherSet = ind->getDocumentsForWord(words.at(argIndex));

			if(first){
				first = false;
				allow = otherSet;
				argIndex++;
				continue;
			} else {

				map<int, string> finalSet;
			
				for(int i = 0; i < allow.size(); i++){
					string currentDocName = allow.at(i);

					pair<int, string> newMapItem;
					newMapItem.first = i;
					newMapItem.second = currentDocName;
					finalSet.insert(newMapItem);
					
				}

				for(int i = 0; i < otherSet.size(); i++){
					string currentDocName = otherSet.at(i);

					pair<int, string> newMapItem;
					newMapItem.first = i;
					newMapItem.second = currentDocName;
					finalSet.insert(newMapItem);
					
				}

				allow.clear();

				for(map<int, string>::iterator it = finalSet.begin(); it != finalSet.end(); ++it)
						allow.push_back(it->second);

				
			}

			argIndex++;
		}

	} else {

		allow = ind->getDocumentsForWord(words.at(0));

	}

	argIndex++;

	first = true;

	while(argIndex < words.size()){

		cout << words.at(argIndex) << "NOT" << endl;

		vector<string> otherSet = ind->getDocumentsForWord(words.at(argIndex));

		if(first){
				first = false;
				disallow = otherSet;
				argIndex++;
				continue;
			} else {

				map<int, string> finalSet;
			
				for(int i = 0; i < allow.size(); i++){
					string currentDocName = allow.at(i);

					pair<int, string> newMapItem;
					newMapItem.first = i;
					newMapItem.second = currentDocName;
					finalSet.insert(newMapItem);
					
				}

				for(int i = 0; i < otherSet.size(); i++){
					string currentDocName = otherSet.at(i);

					pair<int, string> newMapItem;
					newMapItem.first = i;
					newMapItem.second = currentDocName;
					finalSet.insert(newMapItem);
					
				}

				disallow.clear();

				for(map<int, string>::iterator it = finalSet.begin(); it != finalSet.end(); ++it)
						disallow.push_back(it->second);

			}

		argIndex++;
	}

	vector<string> outputFromSearch;
	
	for(int i = 0; i < allow.size(); i++){
		string docTitle = allow.at(i);
		if(find(disallow.begin(), disallow.end(), docTitle) == disallow.end()){
			outputFromSearch.push_back(docTitle);
		}
	}


	for(int i = 0; i < outputFromSearch.size(); i++)
         cout << "Document #" << i << " " << outputFromSearch.at(i) << endl;

    
    cout << "If you'd like to see a doc, type in its number, or else type -1\n";
    int pageNum;

    while(true){
    	cin >> pageNum;
    	if(pageNum < 0) break;
    	else {
    		string fileName = "SearchDocs/";
    		string docName = outputFromSearch.at(pageNum);
    		replace(docName.begin(), docName.end(), '/', '.');
    		replace(docName.begin(), docName.end(), ' ', '_');
    		fileName += docName;;
    		cout << fileName << endl;
    		ifstream in(fileName);
    		while(in.good() && !in.eof()){
    			string word;
    			in >> word;
    			cout << word << ' ';
    		}
    	}

    	cout << "Type Another Number!\n";
	}	
}
