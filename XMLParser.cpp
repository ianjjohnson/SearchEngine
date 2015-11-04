#include "XMLParser.h"
#include "porter2_stemmer.h"
#include "rapidxml/rapidxml.hpp"
#include <sstream>
#include <algorithm>

XMLParser::XMLParser(string fileName){
	is.open(fileName);

	cout << "File opened!\n";
}

bool XMLParser::isXMLTag(string token){
   return token.find("<") != string::npos
      || token.find(">") != string::npos
      || token.find("=") != string::npos
      || token.find("-") != string::npos
      || token.find(".") != string::npos
      || token.find(";") != string::npos
      || token.find(":") != string::npos
      || token.find("(") != string::npos
      || token.find(")") != string::npos
      || token.find("/") != string::npos
      || token.find("]") != string::npos
      || token.find("[") != string::npos
      || token.find(",") != string::npos
      || token.find("|") != string::npos
      || token.find("\'") != string::npos
      || token.find("}") != string::npos
      || token.find("{") != string::npos;
}

/*
Reads a .xml file based on the fileName associated with this XMLParser into an IndexInterface
@param index - the IndexInterface to load the xml data into
@return - true if the method succeeded
*/
bool XMLParser::readFile(IndexInterface* index){
   //The first page of the dump is trash, this skips over it
	skipIntroPage();

   //count the number of files and print it to the screen
	int num = 0;

   //Write a doc to the index while we're not at the end of a file
	while(!is.eof()){
		num++;
		cout << num << endl;
		writeDocToIndex(index);
	}
	return true;
}


/*
Skips one page of the current xmlfile
@return - true if the method succeeded
*/
bool XMLParser::skipIntroPage(){

	string token = "";
	while(token != "<page>") is >> token;

	return true;
}

/*
Reads a file called fileName into an IndexInterface using rapidxml
@param fileName - the name of the file to index
@param index - the index to populate with information from the file
*/
bool XMLParser::readFileToIndex(string fileName, IndexInterface* index){

   /*
   This is a block of setup code for rapidxml so that I can parse through the xml file efficiently
   */

	rapidxml::xml_document<> doc;
   rapidxml::xml_node<> * root_node;
   ifstream theFile (fileName);

   cout << "loaded the file\n";

   vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
   buffer.push_back('\0');
   // Parse the buffer using the xml file parsing library into doc 
   doc.parse<0>(&buffer[0]);
   // Find our root node
   root_node = doc.first_node("mediawiki");

   cout << "found root node\n";

   //Save the number of documents that have been indexed
   int num = 0;
   for (rapidxml::xml_node<> * document_node = root_node->first_node("page"); document_node; document_node = document_node->next_sibling()){

      //The text element of the current document node
      rapidxml::xml_node<> * text;

      //Grab the text and document name of the current document
      text = document_node->first_node("revision")->first_node("text");
      string docName = document_node->first_node("title")->value();
      
      //Ignore documents called user or file. They're garbage
      if(docName.substr(0, 4) == "User" || docName.substr(0,4) == "File") continue;

      string fileName = "SearchDocs/";
      string docNameCopy = docName;
      replace(docNameCopy.begin(), docNameCopy.end(), '/', '.');
      replace(docNameCopy.begin(), docNameCopy.end(), ' ', '_');
      fileName = fileName + docNameCopy;
      ofstream writeDocFile(fileName);


      //Add this document to the index and save it's index in the index number-name registry
      int indexOfDoc = index->addDocument(docName);


      //If we've successfully found a text node for this document
      if(text){

      	 docNameCopy = docName;
      	 replace(docNameCopy.begin(), docNameCopy.end(), '/', ' ');
      	 replace(docNameCopy.begin(), docNameCopy.end(), ':', '.');
      	 stringstream titleStream(docNameCopy);


      	 while(!titleStream.eof()){

            //save the current word and add it to the index for the current document number
         	string word;
         	titleStream >> word;
         	writeDocFile << word << " ";
            //if(/*!isStopWord(word) */!isXMLTag(word)){
				word[0] = tolower(word[0]);
            	Porter2Stemmer::stem(word);
            	index->addWordForDocument(indexOfDoc, word);
          }

         //Make a stringstream of the text element of the document node
         stringstream ss(text->value());
         
         //Parse through the whole stringstream
         while(!ss.eof()){

            //save the current word and add it to the index for the current document number
         	string word;
         	ss >> word;
         	writeDocFile << word << " ";
            //if(/*!isStopWord(word) */!isXMLTag(word)){
				word[0] = tolower(word[0]);
            	Porter2Stemmer::stem(word);
            	index->addWordForDocument(indexOfDoc, word);
           	}


       }


       writeDocFile.close();
       cout << num++ << docName << endl;
   }

   return true;
}


/*
Write a single document from the xml file into the given index using my own ghetto xml parsing
@param index - the IndexInterface object to write to
@return - true if the method ran to completion and filled a document into the index
*/
bool XMLParser::writeDocToIndex(IndexInterface* index){


   //Token for tokenizing the entire file
	string token = "";

   //Move forward past the initial junk
	while(token != "<page>" && !is.eof()) is >> token;
	
   //If there's no pages, return false to indicate we haven't loaded anything
	if(is.eof()) return false;


   //skip a token
	is >> token;

   //Save the name of the doc
	string docName;
	getline(is, docName);

   //Remove xml crap from the doc name
	docName = token + docName;
	docName = docName.substr(7, docName.size()-15);

   //Skip user and file documents -- they're junk
	if(docName.substr(0, 4) == "User" || docName.substr(0,4) == "File") return false;

   //Save all the words in the file into this
	vector<string> words;

   //For each word in the section, stem the word, put it to lower case and copy it into the words arr
	while(token != "</page>"){
		for(int i = 0; i < token.size(); i++)
			token[i] = tolower(token[i]);
		//if(isNotXMLTag(token) && !isStopWord(token)){
		Porter2Stemmer::stem(token);
		words.push_back(token);
		//}
		is >> token;
	}

   //Add the document to the index
	index->addDocument(docName, words);

	return true;
}