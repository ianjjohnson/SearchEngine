#include "AVLIndex.h"

#include "porter2_stemmer.h"

#include <string>
#include <vector>
#include <fstream>

#include <map>

AVLIndex::~AVLIndex(){
	//AVL_Tree handles it's own deletion
}

int AVLIndex::addDocument(string name, string author, string date){

	//Build the default pair for the docTitles vector
	pair<string, int> doc;
	doc.first = name;
	doc.second = 1;

	//Add document title to list of titles
	documentTitles.push_back(doc);

	//Build the authorDate pair for the authorDate map
	pair<string, string> authorDate;
	authorDate.first = author;
	authorDate.second = date;
	pair<string, pair<string, string> > mapItem;
	mapItem.first = name;
	mapItem.second = authorDate;

	//Insert the authorDate info into the map
	authorsAndDates.insert(mapItem);

	//Return the largest valid index in documentTitles (the most recently input value)
	return documentTitles.size()-1;
}

bool AVLIndex::addWordForDocument(int documentIndex, string word){

	//Add the page number to the index
	index.addDocToWord(word, documentIndex);

	//Register the increase in word count for the document
	documentTitles.at(documentIndex).second++;

	return true;
}

vector<string> AVLIndex::getDocumentsForWord(string word){

	//Stem word and make lower case
	Porter2Stemmer::stem(word);

	for(int i = 0; i < word.size(); i++)
		word[i] = tolower(word[i]);

	//Search the tree for the word
	vector<int> docs = index.getDocsForWord(word);

	//Make a map of the titles keyed by TF/IDF value
	map<int, string> titles;

	//For each document which contains the word
	for(int i = 0; i < docs.size(); i++){

		if(docs.at(i) != -1){
		
			//Count how many times that document contains the keyword
			int numberOfAppearances = count(docs.begin(), docs.end(), docs.at(i));
			
			//Generate a map key-value pair by calculating TF/IDF value for the word
			pair<int, string> newPair;
			newPair.second = documentTitles.at(docs.at(i)).first;
			newPair.first = (int)(1000*numberOfAppearances/documentTitles.at(docs.at(i)).second);
			titles.insert(newPair);
			docs.erase(remove(docs.begin(), docs.end(), docs.at(i)));
		}
	}

	//Output title vector which will be ordered by TF/IDF
	vector<string> titleVector;

	//Iterate through the map, sorted by TF/IDF and push titles onto the titleVector
	for(map<int, string>::iterator it = titles.begin(); it != titles.end(); ++it)
		if(find(titleVector.begin(), titleVector.end(), it->second) == titleVector.end())
			titleVector.push_back(it->second);

	return titleVector;

}

/*bool AVLIndex::sortComparator(int, int, vector<string>){
	return true;
}*/

bool AVLIndex::writeToFile(string fileName){

	//Build an ofstream for the index
	ofstream os(fileName);

	//For every document indexed
	for(int i = 0; i < documentTitles.size(); i++){

		//Look up author/date info for the doc
		map<string, pair<string, string> >::iterator authorTime = authorsAndDates.find(documentTitles.at(i).first);

		//Print the doc's info to the file
		os << documentTitles.at(i).first << " <1> "  << documentTitles.at(i).second << " " << authorTime->second.first << " <1c> " << authorTime->second.second << endl;
	}

	//Delimiter to indicate end of doc info
	os << "<2>" << endl;

	//Perform a postorder traversal of the tree and write out each word's info as it is found
	index.postorderFileWrite(&os);

	//EOF delimiter
	os << "<4>";
	os.close();
	return true;
}

bool AVLIndex::readFromFile(string fileName){
	
	//Setup ifstream, temp vars
	ifstream is(fileName);
	string token;
	string name = "";
	string author = "";
	string date = "";


	//Check that the file was opened properly
	//This loop will be broken when the end of the document info section is found
	while(is.is_open()){


		//Until delimiter one is found, save every string found into the doc title
		while(token != "<1>"){
			name += " " + token;
			is >> token;
		} 


		//Erase the extra space at the beginning
		name.erase(0,1);

		//Find and save the number of words in this document
		string num;
		is >> num;
		pair<string, int> item;
		item.first = name;
		item.second = atoi(num.c_str());
		documentTitles.push_back(item);
		
		is >> token;


		//Save the author's name
		while(token != "<1c>"){
			author += " " + token;
			is >> token;
		} 
		
		//Save the last revision's timestamp
		is >> date; 

		//Build the author and date map key-value pair and insert it into the map
		pair<string, string> authorAndDate;
		authorAndDate.first = author;
		authorAndDate.second = date;

		pair<string, pair<string, string> > mapItem;
		mapItem.first = name;
		mapItem.second = authorAndDate;

		authorsAndDates.insert(mapItem);

		//Reset temp vars
		name = "";
		author = "";
		date = "";
		
		is >> token;

		//After the <2> delimiter, all info will be words, so break the doc info reading
		if(token == "<2>") break;
	}


	//Setup temp vars
	name = "";
	vector<int> nums;

	//For the rest of the file
	while(is.is_open()){

		//Empty the nums vec
		nums.clear();

		//Save the word being indexed
		is >> name;

		//If delimiter 4 is found, EOF
		if(name == "<4>") break;

		//Save numbers to the page's number vector until delimiter 3 is found
		while(true){
			string num;
			is >> num;
			if(num == "<3>"){
				AVLnode<string>* newNode = index.insert(name);
				newNode->docs = nums;
				break;
			} else {
				nums.push_back(atoi(num.c_str()));
			}
		}
	}
	return true;
}

/*
Getter for the number of words indexed by this Index object
@return the number of words indexed by this object
*/
int AVLIndex::numWords(){
	return index.getNumWords();
}

/*
Getter for the number of documents indexed by this Index object
@return the number of docs indexed by this object
*/
int AVLIndex::numDocs(){
	return documentTitles.size();
}

/*
Gets the author and timestamp for a document by title
@param name - the name of the document to find info for
@return.first - the author of the document's last revision
@return.second - the timestamp of the document's last revision
*/
pair<string, string> AVLIndex::getAuthorAndTimeForDocNamed(string name){

	//Search the map for the doc titled 'name'
	map<string, pair<string, string> >::iterator authorTime = authorsAndDates.find(name);

	//Return the author/time fair for that document
	return authorTime->second;
}