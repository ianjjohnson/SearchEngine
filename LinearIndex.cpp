#include "LinearIndex.h"
#include "porter2_stemmer.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

LinearIndex::~LinearIndex() {

}

bool LinearIndex::addDocument(string name, vector<string> relevantWords){return true;}
	/*int indexOfNewDoc = documentTitles.size();

	pair<string, int> doc;
	doc.first = name;
	doc.second = relevantWords.size();

	//Add document title to list of titles
	documentTitles.push_back(doc);

	for(int i = 0; i < relevantWords.size(); i++){

		string word = relevantWords.at(i);

		if(word == "@@@") continue;

		unordered_map< string, vector< pair<int,int> > >::iterator item = index.find(word);

		//If this word hasn't been indexed yet
		if(item == index.end()){

			vector< pair<int, int > > nums;
			pair <int, int> defaultEntry;
			defaultEntry.first = indexOfNewDoc;
			defaultEntry.second = count(relevantWords.begin(), relevantWords.end(), word);
			nums.push_back(defaultEntry);
			pair< string, vector< pair<int, int> > > newItem (word, nums);
			index.insert(newItem);

		} 
		//If this word has already been indexed
		else {

			pair <int, int> defaultEntry;
			defaultEntry.first = indexOfNewDoc;
			defaultEntry.second = count(relevantWords.begin(), relevantWords.end(), word);
			item->second.push_back(defaultEntry);
		}

		//Mark all duplicate words so they're not added again
		for(int j = 0; j < relevantWords.size(); j++)
			if(relevantWords.at(j) == word) relevantWords.at(j) = "@@@";

	}

	return true;
}*/

/*
Add a document to the index by document name and returns it's index in the docTitles list
@param name - the name of the document
@return - the index of the document in the documentTitles vector
*/
int LinearIndex::addDocument(string name){
	int indexOfNewDoc = documentTitles.size();

	pair<string, int> doc;
	doc.first = name;
	doc.second = 1;

	//Add document title to list of titles
	documentTitles.push_back(doc);

	return documentTitles.size()-1;
}

/*
Adds a word to be indexed for a specific document
@param documentIndex - the index of the document in the documentTitles vector
@param word - the word to be added to the index for this document
*/
bool LinearIndex::addWordForDocument(int documentIndex, string word){
		unordered_map< string, vector< int > >::iterator item = index.find(word);

		documentTitles.at(documentIndex).second = documentTitles.at(documentIndex).second +1;
		//If this word hasn't been indexed yet
		if(item == index.end()){

			vector< int > nums;
			nums.push_back(documentIndex);
			pair< string, vector<int> > newItem (word, nums);
			index.insert(newItem);

		} 
		//If this word has already been indexed
		else {
			item->second.push_back(documentIndex);
		}

		return true;
}

/*
A comparator for sorting two items to be output.
@param first - the index of the first document 
*/
bool LinearIndex::sortComparator(int first, int second, vector<string> wordsInSearchQuery){
	int numMatches1 = 0;
	int numMatches2 = 0;
	for(int i = 0; i < wordsInSearchQuery.size(); i++){
		unordered_map<string, vector< int > >::iterator item = index.find(wordsInSearchQuery.at(i));
		numMatches1	+= count(item->second.begin(), item->second.end(), first);
		numMatches2 += count(item->second.begin(), item->second.end(), second);
	}
	return numMatches1/documentTitles.at(first).second < numMatches2/documentTitles.at(second).second;
}

/*
THIS SHOULD ONLY GET EVERYTHING FOR ONE DOC.
INTERSECTION SHOULD BE DONE IN XML PARSER
*/

vector<string> LinearIndex::getDocumentsForWord(string word){

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

	cout << "About to make title vector for word "<< word <<  "\n";

	vector<string> titleVector;

	for(map<int, string>::iterator it = titles.begin(); it != titles.end(); ++it)
		if(find(titleVector.begin(), titleVector.end(), it->second) == titleVector.end())
			titleVector.push_back(it->second);

	cout << "Made title vector\n";

	return titleVector;
}

int LinearIndex::numWords(){
	return index.size();
}

int LinearIndex::numDocs(){
	return documentTitles.size();
}

/*vector<string> LinearIndex::getDocumentsForQuery(vector<string> inDoc, vector<string> notInDoc, bool both){
	
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
void LinearIndex::print(){
	cout << "Printing!\n";

	for (unordered_map<string, vector< int > >::iterator it = index.begin(); it != index.end(); ++it){
  		vector< int > nums = it->second;
  		cout << it->first << ": ";
  		for(int i = 0; i < nums.size(); i++)
  			cout << documentTitles.at(nums.at(i)).first << "\t";
  		cout << endl;
	}
}

bool LinearIndex::writeToFile(string fileName){
	ofstream os(fileName);
	for(int i = 0; i < documentTitles.size(); i++){
		os << documentTitles.at(i).first << " <1> "  << documentTitles.at(i).second << endl;
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

bool LinearIndex::readFromFile(string fileName){
	ifstream is(fileName);
	string token;
	string name = "";

	while(is.is_open()){
		is >> token;

		if(token != "<1>"){
			name += " " + token;
		} else {
			string num;
			is >> num;
			pair<string, int> item;
			item.first = name;
			item.second = atoi(num.c_str());
			documentTitles.push_back(item);
			name = "";
		}

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