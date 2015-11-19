#include "HashIndex.h"
#include "porter2_stemmer.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

HashIndex::~HashIndex() {
	//std::unordered_map deals with its own memory, yo'
}

/*
Add a document to the index by document name and returns it's index in the docTitles list
@param name - the name of the document
@return - the index of the document in the documentTitles vector
*/
int HashIndex::addDocument(string name, string author, string date){
	int indexOfNewDoc = documentTitles.size();

	pair<string, int> doc;
	doc.first = name;
	doc.second = 1;

	//Add document title to list of titles
	documentTitles.push_back(doc);

	pair<string, string> authorDate;
	authorDate.first = author;
	authorDate.second = date;

	pair<string, pair<string, string> > mapItem;
	mapItem.first = name;
	mapItem.second = authorDate;

	authorsAndDates.insert(mapItem);

	return documentTitles.size()-1;
}

/*
Adds a word to be indexed for a specific document
@param documentIndex - the index of the document in the documentTitles vector
@param word - the word to be added to the index for this document
*/
bool HashIndex::addWordForDocument(int documentIndex, string word){
		//Search for this word in the index
		unordered_map< string, vector< int > >::iterator item = index.find(word);

		//Register that the number of words in this document has increased
		documentTitles.at(documentIndex).second = documentTitles.at(documentIndex).second +1;
		
		//If this word hasn't been indexed yet
		if(item == index.end()){

			//Build a new object for this word and put it in the index
			vector< int > nums;
			nums.push_back(documentIndex);
			pair< string, vector<int> > newItem (word, nums);
			index.insert(newItem);

		} 
		//If this word has already been indexed
		else {
			//Add this document index to the word's index list
			item->second.push_back(documentIndex);
		}

		return true;
}

/*
A comparator for sorting two items to be output.
@param first - the index of the first document 

bool HashIndex::sortComparator(int first, int second, vector<string> wordsInSearchQuery){
	int numMatches1 = 0;
	int numMatches2 = 0;
	for(int i = 0; i < wordsInSearchQuery.size(); i++){
		unordered_map<string, vector< int > >::iterator item = index.find(wordsInSearchQuery.at(i));
		numMatches1	+= count(item->second.begin(), item->second.end(), first);
		numMatches2 += count(item->second.begin(), item->second.end(), second);
	}
	return numMatches1/documentTitles.at(first).second < numMatches2/documentTitles.at(second).second;
}*/

/*
Gets all of the documents which contain word, and returns them in a vector ordered by relevance
@param word - the word to search for
@return - a list of documents containing that word, ranked by TF/IDF
*/
vector<string> HashIndex::getDocumentsForWord(string word){

	Porter2Stemmer::stem(word);
	word[0] = tolower(word[0]);

	unordered_map<string, vector< int > >::iterator wordInIndex = index.find(word);

	if(wordInIndex == index.end()){
		vector<string> empty;
		return empty;
	}

	vector<int> docs = wordInIndex->second;

	map<int, string> titles;

	for(int i = 0; i < docs.size(); i++){
		if(docs.at(i) != -1){
			int numberOfAppearances = count(docs.begin(), docs.end(), docs.at(i));
			pair<int, string> newPair;
			newPair.second = documentTitles.at(docs.at(i)).first;
			newPair.first = (int)(1000*numberOfAppearances/documentTitles.at(docs.at(i)).second);
			titles.insert(newPair);
			docs.erase(remove(docs.begin(), docs.end(), docs.at(i)));
		}
	}

	vector<string> titleVector;

	for(map<int, string>::iterator it = titles.begin(); it != titles.end(); ++it)
		if(find(titleVector.begin(), titleVector.end(), it->second) == titleVector.end())
			titleVector.push_back(it->second);

	return titleVector;
}

/*
Getter for the number of words indexed by this Index object
@return the number of words indexed by this object
*/
int HashIndex::numWords(){
	return index.size();
}

/*
Getter for the number of documents indexed by this Index object
@return the number of docs indexed by this object
*/
int HashIndex::numDocs(){
	return documentTitles.size();
}

/*vector<string> HashIndex::getDocumentsForQuery(vector<string> inDoc, vector<string> notInDoc, bool both){
	
	vector< int > docs;

	for(int i = 0; i < inDoc.size(); i++){
		Porter2Stemmer::stem(inDoc.at(i));
		//for(int j = 0; j < inDoc.at(i).size(); j++)
			//inDoc.at(i)[j] = tolower(inDoc.at(i)[j]);
		inDoc.at(i)[0] = tolower(inDoc.at(i)[0]);
	}

	for(int i = 0; i < notInDoc.size(); i++){
		Porter2Stemmer::stem(notInDoc.at(i));
		//for(int j = 0; j < notInDoc.at(i).size(); j++)
		//	notInDoc.at(i)[j] = tolower(notInDoc.at(i)[j]);
		notInDoc.at(i)[0] = tolower(notInDoc.at(i)[0]);
	}

	for (unordered_map<string, vector< int > >::iterator it = index.begin(); it != index.end(); ++it){

		for(int i = 0; i < inDoc.size(); i++){
			if(it->first == inDoc.at(i)){
				for(int j = 0; j < it->second.size(); j++){
					bool add = false;
					if(both && i+1 < inDoc.size()){
						unordered_map<string, vector< int > >::iterator other = index.find(inDoc.at(i+1));
						if(other != index.end()){
							for(int k = 0; k < other->second.size(); k++){
								if(other->second.at(k) == it->second.at(j)){
									add = true;
									break;
								}
							}
						}
					} else add = true;
					if(add)
						docs.push_back(it->second.at(j));
				}
			}
			if(both) i++;
		}
	}



	for(int i = 0; i < notInDoc.size(); i++){
		unordered_map<string, vector< int > >::iterator item = index.find(notInDoc.at(i));
		if(item != index.end()){
			for(int j = 0; j < item->second.size(); j++){
				for(int k = 0; k < docs.size(); k++){
					if(item->second.at(j) == docs.at(k)){
						docs.at(k) = -1;
					}
				}
			}
		}
	}

	cout << docs.size() << endl;

	map<int, string> titles;

	for(int i = 0; i < docs.size(); i++){
		if(docs.at(i) != -1){
			int numberOfAppearances = count(docs.begin(), docs.end(), docs.at(i));
			pair<int, string> newPair;
			newPair.second = documentTitles.at(docs.at(i)).first;
			newPair.first = (int)(1000*numberOfAppearances/documentTitles.at(docs.at(i)).second);
			titles.insert(newPair);
			docs.erase(remove(docs.begin(), docs.end(), docs.at(i)));
		}
	}

	cout << "About to make title vector\n";

	vector<string> titleVector;

	for(map<int, string>::iterator it = titles.begin(); it != titles.end(); ++it)
		if(find(titleVector.begin(), titleVector.end(), it->second) == titleVector.end())
			titleVector.push_back(it->second);

	cout << "Made title vector\n";

	return titleVector;
}*/

/*
Prints the index out to the console -- only for debugging
*/
void HashIndex::print(){
	cout << "Printing!\n";

	for (unordered_map<string, vector< int > >::iterator it = index.begin(); it != index.end(); ++it){
  		vector< int > nums = it->second;
  		cout << it->first << ": ";
  		for(int i = 0; i < nums.size(); i++)
  			cout << documentTitles.at(nums.at(i)).first << "\t";
  		cout << endl;
	}
}

/*
Writes the index to a file
@param fileName - the file to write to
@return - true if successful
*/
bool HashIndex::writeToFile(string fileName){
	ofstream os(fileName);
	for(int i = 0; i < documentTitles.size(); i++){

		map<string, pair<string, string> >::iterator authorTime = authorsAndDates.find(documentTitles.at(i).first);

		os << documentTitles.at(i).first << " <1> "  << documentTitles.at(i).second << " " << authorTime->second.first << " <1c> " << authorTime->second.second << endl;
	}
	os << "<2>" << endl;
	for (unordered_map<string, vector< int > >::iterator it = index.begin(); it != index.end(); ++it){
		os << it->first;
		for(int i = 0; i < it->second.size(); i++){
			os << " " << it->second.at(i) << " ";  
		}
		os << " <3>\n";
	}

	os << "<4>";
	os.close();
	return true;
}

/*
Reads the index from a file
@param fileName - the file to read from
@return - true if successful
*/
bool HashIndex::readFromFile(string fileName){
	ifstream is(fileName);
	string token;
	string name = "";
	string author = "";
	string date = "";

	while(is.is_open()){

		while(token != "<1>"){
			name += " " + token;
			is >> token;
		} 

		name.erase(0,1);

		string num;
		is >> num;
		pair<string, int> item;
		item.first = name;
		item.second = atoi(num.c_str());
		documentTitles.push_back(item);
		
		is >> token;

		while(token != "<1c>"){
			author += " " + token;
			is >> token;
		} 
		
		is >> date; 

		pair<string, string> authorAndDate;
		authorAndDate.first = author;
		authorAndDate.second = date;

		pair<string, pair<string, string> > mapItem;
		mapItem.first = name;
		mapItem.second = authorAndDate;

		authorsAndDates.insert(mapItem);
		name = "";
		author = "";
		date = "";

		is >> token;
		

		if(token == "<2>") break;
	}

	name = "";
	vector<int> nums;
	while(is.is_open()){
		nums.clear();
		is >> name;
		if(name == "<4>") break;
		while(true){
			string num;
			is >> num;
			if(num == "<3>"){
				break;
			} else {
				nums.push_back(atoi(num.c_str()));
			}
		}

		pair<string, vector< int > > newItem;
		newItem.first = name;

		for(int i = 0; i < nums.size(); i++){
			newItem.second.push_back(nums.at(i));
		}

		index.insert(newItem);
	}
	return true;
}

/*
Gets the author and timestamp for a document by title
@param name - the name of the document to find info for
@return.first - the author of the document's last revision
@return.second - the timestamp of the document's last revision
*/
pair<string, string> HashIndex::getAuthorAndTimeForDocNamed(string name){
	map<string, pair<string, string> >::iterator authorTime = authorsAndDates.find(name);
	return authorTime->second;
}