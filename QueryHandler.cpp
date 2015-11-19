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

/*
Public interface method for query handling
@param query - the list of strings in the user's query input (includes AND, OR, NOT)
@param indInput - a pointer to the index to search
*/
void QueryHandler::handleQuery(vector<string> query,  IndexInterface* indInput){
	ind = indInput;
	doSearch(query);
}

/*
Searches the index for a series of words in 'words'
@param words - the list of words input by the user in the query (includes AND, OR, NOT)
*/
void QueryHandler::doSearch(vector<string> words){

	//Initialize temp vars for documents which are 'allowed' and 'disallowed'
   	vector<string> allow;
   	vector<string> disallow;


   	//Boolean to check if current token is first token
   	bool first = true;

   	//Index of the words vector which is currently being checked
   	int argIndex = 1;

	if(words.at(0) == "AND")
	//Case 1: AND operator
	{
		//For each operand which is not 'NOT'
		while(argIndex < words.size() && words.at(argIndex) != "NOT"){

			//Save all the documents which contain the new word being checked
			vector<string> otherSet = ind->getDocumentsForWord(words.at(argIndex));

			//If this is the first argument, don't perform set intersection because first set is empty
			if(first){
				first = false;
				allow = otherSet;
				argIndex++;
				continue;
			} else {

				//Map of key-value pairs keyed by sum of indeces in the original vectors returned by the index in TF/IDF order
				//This allows for equal treatment of both parts of the AND clause
				map<int, string> finalSet;	
			

				//Perform a set intersection, allowing only documents which contain both words
				for(int i = 0; i < allow.size(); i++){
					string currentDocName = allow.at(i);
					vector<string>::iterator docInSecondList = find(otherSet.begin(), otherSet.end(), currentDocName);

					if(docInSecondList != otherSet.end()){

						//Key for the map is the sum of the two doc's indeces in their vectors
						int indexSum = i + (docInSecondList - otherSet.begin());
						pair<int, string> newMapItem;
						newMapItem.first = indexSum;
						newMapItem.second = currentDocName;
						finalSet.insert(newMapItem);
					}
				}

				//Empty the allow vector
				allow.clear();


				//Re-fill the allow vector by ascending order of sum of original vector indeces, so the
				//documents with the highest average TF/IDF value for the two sets come first
				for(map<int, string>::iterator it = finalSet.begin(); it != finalSet.end(); ++it)
						allow.push_back(it->second);

				
			}

			argIndex++;
		}


	} else if(words.at(0) == "OR")
	//Case 2: OR operator
	{

		//For each operand which is not 'NOT'
		while(argIndex < words.size() && words.at(argIndex) != "NOT"){

			//Save all the documents which contain the new word being checked
			vector<string> otherSet = ind->getDocumentsForWord(words.at(argIndex));

			//If this is the first argument, don't perform set union because first set is empty
			if(first){
				first = false;
				allow = otherSet;
				argIndex++;
				continue;
			} else {

				//Map of key-value pairs keyed by sum of indeces in the original vectors returned by the index in TF/IDF order
				//This allows for equal treatment of both parts of the OR clause
				map<int, string> finalSet;
			
				//Perform a set union, allowing only documents which contain both words
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

				//Empty the allow vector
				allow.clear();

				//Re-fill the allow vector by ascending order of sum of original vector indeces, so the
				//documents with the highest average TF/IDF value for the two sets come first
				for(map<int, string>::iterator it = finalSet.begin(); it != finalSet.end(); ++it)
						allow.push_back(it->second);

				
			}

			argIndex++;
		}

	} else
	//Case 3: No operator
	{

		//Allow any document which matches the first word given
		allow = ind->getDocumentsForWord(words.at(0));

	}

	argIndex++;

	first = true;


	//For the remaining items, perform the 'NOT' (set difference) operation
	while(argIndex < words.size()){

		//Find all the documents which contain the disallowed word
		vector<string> otherSet = ind->getDocumentsForWord(words.at(argIndex));

		//If first, don't do set union because set 1 is empty
		if(first){
				first = false;
				disallow = otherSet;
				argIndex++;
				continue;
			} else {

				//THE REASON I'M BOTHERING TO RANK THE DISALLOWED ARTICLES IS THAT MAYBE IN SOME FORM, THE SEARCH
				//ENGINE COULD WANT TO AVOID THESE WORDS IF POSSIBLE WITHOUT ENTIRELLY RULING THEM OUT OF THE SEARCH

				//Map of key-value pairs keyed by sum of indeces in the original vectors returned by the index in TF/IDF order
				//This allows for equal treatment of both parts of the OR clause
				map<int, string> finalSet;
			

				//Perform the set union
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


				//Empty the disallow vector
				disallow.clear();

				//Re-fill the allow vector by ascending order of sum of original vector indeces, so the
				//documents with the highest average TF/IDF value for the two sets come first
				for(map<int, string>::iterator it = finalSet.begin(); it != finalSet.end(); ++it)
						disallow.push_back(it->second);

			}

		argIndex++;
	}

	//The output from the search
	vector<string> outputFromSearch;
	
	//Take all of the allowed documents which are not in disallowed documents and put them in the output
	for(int i = 0; i < allow.size(); i++){
		string docTitle = allow.at(i);
		if(find(disallow.begin(), disallow.end(), docTitle) == disallow.end()){
			outputFromSearch.push_back(docTitle);
		}
	}


	//For the first 15 documents to be output
	for(int i = 0; i < 15 && i < outputFromSearch.size(); i++){

		//Output document number, title, author and timestamp info for the doc
         cout << "Document #" << i << " " << outputFromSearch.at(i) << endl;
         pair<string, string> authTime = ind->getAuthorAndTimeForDocNamed(outputFromSearch.at(i));
         cout << "Author: " << authTime.first << ", Timestamp: " << authTime.second << "\n";
    }

    
    cout << "If you'd like to see a doc, type in its number, or else type -1\n";
   
    int pageNum;


    //Loop until user says to break by typing -1
    while(true){

    	//Catch invalid entries
    	if(!(cin >> pageNum)){
   			cout << "Invalid input. Please try again.\n";
   			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   			continue;
   		}

   		//Break if the user says so
    	if(pageNum < 0) break;

    	//Print out all of the document
    	else {

    		//Calculate the document's file name
    		string fileName = "SearchDocs/";
    		string docName = outputFromSearch.at(pageNum);
    		replace(docName.begin(), docName.end(), '/', '.');
    		replace(docName.begin(), docName.end(), ' ', '_');
    		fileName += docName;;
    		cout << fileName << endl;

    		//Open an IFStream for the document and print it
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
