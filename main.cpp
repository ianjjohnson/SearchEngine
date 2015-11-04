#include <iostream>
#include <sstream>
#include "LinearIndex.h"
#include "XMLParser.h"
#include "QueryHandler.h"
#include <algorithm>

using namespace std;

void doSearch(int, char**);


//The stop words to ignore in search
vector<string> stopWords;
LinearIndex ind;
QueryHandler query;

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
  //parser.readFileToIndex("Input.xml", &ind);

   /*LinearIndex ind;

   XMLParser parser("Input.xml");
   parser.readFile(&ind);

   ind.print();*/
   ind.readFromFile("index.txt");

   query.handleQuery(argc, argv, &ind);
}

