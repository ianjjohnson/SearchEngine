#include "AVLIndex.h"

#include "porter2_stemmer.h"

#include <string>
#include <vector>
#include <fstream>

#include <map>

AVLIndex::~AVLIndex(){
}

int AVLIndex::addDocument(string name){

	int indexOfNewDoc = documentTitles.size();

	pair<string, int> doc;
	doc.first = name;
	doc.second = 1;

	//Add document title to list of titles
	documentTitles.push_back(doc);


	return documentTitles.size()-1;
}

bool AVLIndex::addWordForDocument(int documentIndex, string word){

	index.addDocToWord(word, documentIndex);

	documentTitles.at(documentIndex).second++;


}

vector<string> AVLIndex::getDocumentsForWord(string word){

	Porter2Stemmer::stem(word);
	word[0] = tolower(word[0]);

	vector<int> docs = index.getDocsForWord(word);

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

bool AVLIndex::sortComparator(int, int, vector<string>){
	return true;
}

bool AVLIndex::writeToFile(string fileName){
	ofstream os(fileName);
	for(int i = 0; i < documentTitles.size(); i++){
		os << documentTitles.at(i).first << " <1> "  << documentTitles.at(i).second << endl;
	}
	os << "<2>" << endl;

	index.postorderFileWrite(&os);

	os << "<4>";
	os.close();
	return true;
}

bool AVLIndex::readFromFile(string fileName){
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
				cout << "End of doc\n";
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

int AVLIndex::numWords(){
	return index.getNumWords();
}

int AVLIndex::numDocs(){
	return documentTitles.size();
}