#include <iostream>
#include <sstream>
#include "LinearIndex.h"
#include "XMLParser.h"

using namespace std;

void doSearch(int, char**);


//The stop words to ignore in search
vector<string> stopWords;
LinearIndex ind;

/*
Checks if a given token is a stop word
@param token - the word to compare to all the stop words
@return - true if the token is a stop word
*/
bool isStopWord(string token){

   //send token to lower case
   for(int i = 0; i < token.size(); i++){
         token[i] = tolower(token[i]);
   }


   //For every stop word, check if it's a match
   for(int i = 0; i < stopWords.size(); i++){
      if(token == stopWords.at(i)) return true;
   }
   return false;
}

int main(int argc, char* argv[])
{
   
	ifstream getStopWords("stopwords.txt");
	string token = "";

	while(token != "<-1>"){
		getStopWords >> token;
		for(int i = 0; i < token.size(); i++)
			token[i] = tolower(token[i]);
		stopWords.push_back(token);
	}

   XMLParser parser("Input.xml");
  // parser.readFileToIndex("Input.xml", &ind);

   /*LinearIndex ind;

   XMLParser parser("Input.xml");
   parser.readFile(&ind);

   ind.print();*/
   ind.readFromFile("index.txt");


   doSearch(argc, argv);
}

/*
Searches the index for a series of search queries in argv
@param argc - the number of command line args
@param argv - the list of 
*/
void doSearch(int argc, char* argv[]){
   vector<string> allow;
   vector<string> disallow;

   /*
   DO IT THIS WAY!
   vector<string> and;
   vector<string> or;
   vector<string> not;
   */

   bool both = false;

   for(int i = 1; i < argc; i++){
         if(string(argv[i]) == "OR") both = false;
         else if(string(argv[i]) == "AND") both = true;
         else if(string(argv[i]) == "NOT"){
            if(!isStopWord(string(argv[i+1]))){
               disallow.push_back(string(argv[i+1]));
               i++;
            }
         } else {
            if(!isStopWord(string(argv[i])))
               allow.push_back(string(argv[i]));
         }
   }

   vector<string> outputFromSearch = ind.getDocumentsForQuery(allow, disallow, both);

   for(int i = 0; i < outputFromSearch.size(); i++)
         cout << outputFromSearch.at(i) << endl;

      ind.writeToFile("index.txt");
}
