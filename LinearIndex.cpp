#include "LinearIndex.h"
#include <iostream>

LinearIndex::~LinearIndex() {

}

bool LinearIndex::addDocument(string name, vector<string> relevantWords){
	int indexOfNewDoc = documentTitles.size();

	pair<string, int> doc;
	doc.first = name;
	doc.second = relevantWords.size();

	documentTitles.push_back(name);

	for(int i = 0; i < relevantWords.size(); i++){

		string word = relevantWords.at(i);

		if(word == "@@@") continue;

		unordered_map< string, vector< pair<int,int> > >::iterator item = index.find(word);

		if(item == index.end()){

			vector< pair<int, int > > nums;
			pair <int, int> defaultEntry;
			defaultEntry.first = indexOfNewDoc;
			defaultEntry.second = 1;
			nums.push_back(defaultEntry);
			pair< string, vector< pair<int, int> > > newItem (word, nums);
			index.insert(newItem);

		} else {

			pair <int, int> defaultEntry;
			defaultEntry.first = indexOfNewDoc;
			defaultEntry.second = count(relevantWords.begin(), relevantWords.end(), word);
			item->second.push_back(defaultEntry);


			for(int j = 0; j < relevantWords.size(); j++)
				if(relevantWords.at(j) == word) relevantWords.at(j) = "@@@";
		}

	}

	return true;
}

vector<string> LinearIndex::getDocumentsForQuery(vector<string> inDoc, vector<string> notInDoc){
	
	vector< pair <int, int> > docs;

	for (unordered_map<string, vector< pair<int, int> > >::iterator it = index.begin(); it != index.end(); ++it){

		for(int i = 0; i < inDoc.size(); i++){
			if(it->first == inDoc.at(i))
				for(int j = 0; j < it->second.size(); j++)
					docs.push_back(it->second.at(j));
		}

		//Sort by relevance and remove duplicates
	}

	for(int i = 0; i < notInDoc.size(); i++){
		unordered_map<string, vector< pair<int, int> > >::iterator item = index.find(notInDoc.at(i));
		if(item != index.end()){
			for(int j = 0; j < item->second.size(); j++){
				for(int k = 0; k < docs.size(); k++){
					if(item->second.at(j).first == docs.at(k).first){
						docs.at(k).first = -1;
					}
				}
			}
		}
	}

	vector<string> titles;

	for(int i = 0; i < docs.size(); i++){
		if(docs.at(i).first != -1)
			titles.push_back(documentTitles.at(docs.at(i).first));
	}


	return titles;
}

void LinearIndex::print(){
	cout << "Printing!\n";

	for (unordered_map<string, vector< pair<int, int> > >::iterator it = index.begin(); it != index.end(); ++it){
  		vector< pair<int, int> > nums = it->second;
  		cout << it->first << ": ";
  		for(int i = 0; i < nums.size(); i++)
  			cout << documentTitles.at(nums.at(i).first) << ", " << nums.at(i).second << "\t";
  		cout << endl;
	}
}


bool LinearIndex::writeToFile(string fileName){
	return true;
}

bool LinearIndex::readFromFile(string fileName){
	return true;
}