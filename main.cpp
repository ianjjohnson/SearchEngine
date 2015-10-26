#include <iostream>
#include <sstream>
#include "LinearIndex.h"
#include "XMLParser.h"

using namespace std;

//AVLTreeNode *createNewNode(int key);

int main(int argc, char* argv[])
{
   
   LinearIndex ind;

   XMLParser parser("Input.xml");
   parser.readFileToIndex("Input.xml", &ind);

   /*LinearIndex ind;

   XMLParser parser("Input.xml");
   parser.readFile(&ind);

   vector<string> words;

   words.push_back("Hello");
   words.push_back("World");
   words.push_back("Data");

   ind.addDocument("Document 1", words);

   words.clear();

   words.push_back("Computer");
   words.push_back("Science");
   words.push_back("Data");
   words.push_back("Hello");

   ind.addDocument("Document 2", words);

   words.clear();

   words.push_back("Data");
   words.push_back("Structures");
   words.push_back("Algorithms");
   words.push_back("Data");

   ind.addDocument("Document 3", words);

   ind.print();*/
   //ind.readFromFile("read.txt");


   vector<string> allow;
   vector<string> disallow;

   bool both = false;

   for(int i = 1; i < argc; i++){
   		if(string(argv[i]) == "OR") both = false;
   		else if(string(argv[i]) == "AND") both = true;
   		else if(string(argv[i]) == "NOT"){
   			disallow.push_back(string(argv[i+1]));
   			i++;
   		} else {
   			allow.push_back(string(argv[i]));
   		}
   }

   vector<string> outputFromSearch = ind.getDocumentsForQuery(allow, disallow, both);

   for(int i = 0; i < outputFromSearch.size(); i++)
   		cout << outputFromSearch.at(i) << endl;

   	ind.writeToFile("index.txt");
}

/*---------------------------------------------
// Create a new tree node with the given key
//---------------------------------------------
AVLTreeNode *createNewNode(int key)
{
   AVLTreeNode *temp = new AVLTreeNode();
   temp->key = key;
   temp->left = NULL;
   temp->right = NULL;
   temp->parent = NULL;
   temp->balanceFactor = '=';
   return temp;
} */