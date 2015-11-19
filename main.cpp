#include <iostream>
#include <sstream>
#include "HashIndex.h"
#include "XMLParser.h"
#include "QueryHandler.h"
#include <algorithm>
#include <string>
#include <vector>
#include "AVLIndex.h"

using namespace std;

IndexInterface* ind;
QueryHandler query;

int main(int argc, char* argv[])
{

	//Build the parser using default xml file
   XMLParser parser("Input.xml");



   /*
   The following is a switch-statement based UI which allows the user to
   type in a number to indicate which function he would like to call.
   Its lower-level functionality is considered to be self-evident
   */
   int selection = 0;
   int typeSelection = 0;
   string fileName;
   string queryString;
    vector<string> words;
   stringstream queryStream;
   bool exit = false;
   while(true){
   		cout << "(1): Build an Index Object\n" << 
   				"(2): Index a new xml file\n" << 
   				"(3): Load an index from an existing file\n" <<
   				"(4): Delete the index\n" <<
   				"(5): Make a search\n" <<
   				"(6): Print Stats\n" <<
   				"(7): Save the index to a file\n"
   				"(8): Exit \n";
   		if(!(cin >> selection)){
   			cout << "Invalid input. Please try again.\n";
   			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   			continue;
   		}
   		switch(selection){
   			case 1 : 
   				cout << "(1): HashTable\n(2): AVL Tree\n";
   				cin >> typeSelection;
   				switch(typeSelection){
   					case 1 : 
   						ind = new HashIndex();
   						break;
   					case 2 : 
   						ind = new AVLIndex();
   						break;
   					default : 
   						cout << "Damn son you crazy, that number aint cool\n";
   				}
   				break;
   			case 2 :
   				cout << "What is the name of the xml file to index?\n";
   				cin >> fileName;
   				parser.readFileToIndex(fileName, ind);
   				break;
   			case 3 :
   				cout << "What is the name of the input file to load?\n";
   				cin >> fileName;
   				ind->readFromFile(fileName);
   				break;
   			case 4 :
   				delete ind;
   				break;
   			case 5 :
   				words.clear();
   				cout << "Please input your query.\n";
   				cin.get();
   				getline(cin, queryString);
   				queryStream.clear();
   				queryStream.str(queryString);
   				while(!queryStream.eof()){
   					string word;
   					queryStream >> word;
   					words.push_back(word);
   				}
   				query.handleQuery(words, ind);
   				break;
   			case 6 : 
   				cout << "Number of words indexed: " << ind->numWords() << endl;
   				cout << "Number of documents indexed: " << ind->numDocs() << endl;
   				cout << "TOP 50 Words not implemented\n";
   				break;
   			case 7:
   				cout << "What is the name of the file you'd like to write to?\n";
   				cin >> fileName;
   				ind->writeToFile(fileName);
   				break;
   			case 8 :
   				exit = true;
   				break;
   			default : 
   				cout << "Invalid input. Please try again.\n";
   		}
   		if(exit) break;
   }
}

