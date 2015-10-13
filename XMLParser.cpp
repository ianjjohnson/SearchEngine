#include "XMLParser.h"
#include "porter2_stemmer.h"

bool isNotXMLTag(string token){
	return token.find("<") == string::npos
		&& token.find(">") == string::npos
		&& token.find("=") == string::npos
		&& token.find("-") == string::npos
		&& token.find(".") == string::npos
		&& token.find(";") == string::npos
		&& token.find(":") == string::npos
		&& token.find("(") == string::npos
		&& token.find(")") == string::npos
		&& token.find("/") == string::npos
		&& token.find("]") == string::npos
		&& token.find("[") == string::npos
		&& token.find(",") == string::npos
		&& token.find("|") == string::npos
		&& token.find("\'") == string::npos
		&& token.find("}") == string::npos
		&& token.find("{") == string::npos;
}

bool XMLParser::isStopWord(string token){
	for(int i = 0; i < stopWords.size(); i++){
		if(token == stopWords.at(i)) return true;
	}
	return false;
}

XMLParser::XMLParser(string fileName){
	is.open(fileName);

	ifstream getStopWords("stopwords.txt");
	string token = "";

	while(token != "<-1>"){
		getStopWords >> token;
		for(int i = 0; i < token.size(); i++)
			token[i] = tolower(token[i]);
		stopWords.push_back(token);
	}


}

bool XMLParser::skipIntroPage(){

	string token = "";
	while(token != "<page>") is >> token;

	return true;
}

bool XMLParser::writeDocToIndex(IndexInterface* index){

	string token = "";
	while(token != "<page>") is >> token;
	is >> token;

	string docName;
	getline(is, docName);

	docName = token + docName;
	docName = docName.substr(7, docName.size()-15);

	if(docName.substr(0, 4) == "User" || docName.substr(0,4) == "File") return false;

	vector<string> words;

	while(token != "</page>"){
		for(int i = 0; i < token.size(); i++)
			token[i] = tolower(token[i]);
		if(isNotXMLTag(token) && !isStopWord(token)){
			Porter2Stemmer::stem(token);
			words.push_back(token);
		}
		is >> token;
	}

	index->addDocument(docName,words);

	return true;
}