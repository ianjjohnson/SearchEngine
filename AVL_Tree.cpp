#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

/*
A quick note:
I decided to go with the strategy of every node having a pointer to its parent and a saved
balance value for ease of use.
I know this is not the most efficient approach, but it's how I learned it originally
and I find it to be a little easier to intuitize this way
*/

/*
Templatable, but it's only ever a string in this case
*/
template <class T>
class AVLNode {
public:

	//Key value to use as binary search tree comparator
    T key;

    //Balance factor
    int balance;

    //The list of documents which contain this node/word
    vector<int> docs;


    AVLNode *leftChild;
    AVLNode *rightChild;

    AVLNode *parent;
 
 	//Constructor which takes a parent and a key
    AVLNode(T k, AVLNode *p){
    	leftChild = nullptr;
    	rightChild = nullptr;
    	key = k;
    	balance = 0;
    	parent = p;
    }
 
 	//Destructor automatically recursively calls children destructors. Seems rude, but so it goes
    ~AVLNode() {
        delete leftChild;
        delete rightChild;
    }
};
 
//Also templatable, but always used with a string
template <class T>
class AVLTree {

public:

    AVLTree();
    ~AVLTree();
    AVLNode<T>* insert(T key);

    vector<int> getDocsForWord(string word);
    void addDocToWord(string word, int docIndex);
    AVLNode<T>* search(string word);
    int getNumWords();

    void postorderFileWrite(ofstream*);
 
private:
    AVLNode<T> *root;
 
 	//Note I use the method name 'brotate' not 'rotate'. This is an odd personal preference.
    AVLNode<T>* leftBrotate(AVLNode<T>*);
    AVLNode<T>* rightBrotate(AVLNode<T>*);
    AVLNode<T>* leftRightBrotate(AVLNode<T>*);
    AVLNode<T>* rightLeftBrotate(AVLNode<T>*);

    void rebalance(AVLNode<T>*);
    int heightOfNode(AVLNode<T>*);
    void calculateBalance(AVLNode<T>*);
    void postorder(AVLNode<T> *curr, ofstream* os);

    int numWords;
};
 

//Default constructor, nothing exciting
template <class T>
AVLTree<T>::AVLTree(void){
	root = nullptr;
	numWords = 0;
}

//Destructor, deletes root, which will recursively delete it's children (see ~AVLNode())
template <class T>
AVLTree<T>::~AVLTree(void) {
    delete root;
}
 
/*
Performs a left brotation at node 'curr', and returns the new root of the subtree after rotation
@param curr - the node at which to perform the brotation
@return - the new root of the subtree which got brotated
*/
template <class T>
AVLNode<T>* AVLTree<T>::leftBrotate(AVLNode<T> *curr) {

	//cout << "Rotating Left\n";

	//Save right child to temp
    AVLNode<T> *temp = curr->rightChild;

    //Make temp new root of subtree
    temp->parent = curr->parent;

    //Make curr's right child temp's left child
    curr->rightChild = temp->leftChild;
 
 	//If temp's right child wasn't null, save it's new parent
    if (curr->rightChild != nullptr)
        curr->rightChild->parent = curr;
 
 	//Make curr temp's left child and save the parent relationship in curr
    temp->leftChild = curr;
    curr->parent = temp;
 
 	//If temp isn't the root
    if (temp->parent != nullptr) {

    	//Fix the new subtree's parent's pointers
        if (temp->parent->rightChild == curr) {
            temp->parent->rightChild = temp;
        }
        else {
            temp->parent->leftChild = temp;
        }
    }
 	
 	//Calculate the new balances and return the new root of the subtree
    calculateBalance(curr);
    calculateBalance(temp);
    return temp;
}
 
 /*
Performs a right brotation at node 'curr', and returns the new root of the subtree after rotation
@param curr - the node at which to perform the brotation
@return - the new root of the subtree which got brotated
 */
template <class T>
AVLNode<T>* AVLTree<T>::rightBrotate(AVLNode<T> *curr) {

	//cout << "Rotating Right\n";

	//Save left child to temp
    AVLNode<T> *temp = curr->leftChild;

    //Make temp new root of subtree
    temp->parent = curr->parent;

    //Make curr's right child temp's left child
    curr->leftChild = temp->rightChild;
 
 	//If temp's left child wasn't null, save it's new parent
    if (curr->leftChild != nullptr)
        curr->leftChild->parent = curr;
 

    temp->rightChild = curr;
    curr->parent = temp;
 	
 	//If temp isn't the root
    if (temp->parent != nullptr) {

    	//Fix the new subtree's parent's pointers
        if (temp->parent->rightChild == curr) {
            temp->parent->rightChild = temp;
        }
        else {
            temp->parent->leftChild = temp;
        }
    }
 
 	//Calculate the new balances and return the new root of the subtree
    calculateBalance(curr);
    calculateBalance(temp);
    return temp;
}
 
/*
Performs a left-right brotation at node 'curr', and returns the new root of the subtree after rotation
@param curr - the node at which to perform the brotation
@return - the new root of the subtree which got brotated
 */
template <class T>
AVLNode<T>* AVLTree<T>::leftRightBrotate(AVLNode<T> *curr) {

	//cout << "Rotating Left-Right\n";

    curr->leftChild = leftBrotate(curr->leftChild);
    return rightBrotate(curr);
}
 
/*
Performs a right-left brotation at node 'curr', and returns the new root of the subtree after rotation
@param curr - the node at which to perform the brotation
@return - the new root of the subtree which got brotated
 */
template <class T>
AVLNode<T>* AVLTree<T>::rightLeftBrotate(AVLNode<T> *curr) {

	//cout << "Rotating Right-Left\n";

    curr->rightChild = rightBrotate(curr->rightChild);
    return leftBrotate(curr);
}
 
/*
Recursivelly calculates the height of node curr by descending down the tree
@param curr - the node to calculate the height of
@return - the height of the node, where leaves are height 0
*/
template <class T>
int AVLTree<T>::heightOfNode(AVLNode<T> *curr) {

	//base case!
    if (curr == nullptr)
        return -1;

    //Recursively find max of left and right subtrees
    return 1 + std::max(heightOfNode(curr->leftChild), heightOfNode(curr->rightChild));
}

/*
Calculate and saves the balance of node curr by calculating the difference between the heights of its children
@param curr - the node whose balance needs to be calculated and saved
*/
template <class T>
void AVLTree<T>::calculateBalance(AVLNode<T> *curr) {
    curr->balance = heightOfNode(curr->rightChild) - heightOfNode(curr->leftChild);
}
 
template <class T>
void AVLTree<T>::rebalance(AVLNode<T> *curr) {

	//calculate the balance for this current node
    calculateBalance(curr);
 
 	//Left case
    if (curr->balance == -2) {

    	//Left left case
        if (heightOfNode(curr->leftChild->leftChild) >= heightOfNode(curr->leftChild->rightChild))
            curr = rightBrotate(curr);
       
        //Left right case
        else
            curr = leftRightBrotate(curr);
    
    }

    //Right case
    else if (curr->balance == 2) {

    	//Right right case
        if (heightOfNode(curr->rightChild->rightChild) >= heightOfNode(curr->rightChild->leftChild))
            curr = leftBrotate(curr);
        
        //Right left case
        else
            curr = rightLeftBrotate(curr);
    }
 
 	//If root hasn't been found
    if (curr->parent != nullptr) {
    	//Recursively call up the tree
        rebalance(curr->parent);
    }
    else {
    	//Set the new root (if it's been changed)
        root = curr;
    }
}

/*
Inserts a new node into the tree with key 'key'
@param key - they key for the new node
@return - a pointer to the new node after insertion
*/ 
template <class T>
AVLNode<T>* AVLTree<T>::insert(T key) {

	//Empty case
    if (root == nullptr) {
        root = new AVLNode<T>(key, nullptr);
        return root;
    }

    //Not empty case
    else {
        AVLNode<T> *curr = root;
        AVLNode<T> *parent;
 
        while (true) {

        	//If a node has been found with this key, don't do anything and return a nullptr
            if (curr->key == key)
                return nullptr;
 			
 			//Save parent
            parent = curr;
 
            bool direction = curr->key > key; //True if current value is greater than input key value
            curr = direction ? curr->leftChild : curr->rightChild; //Move left or right down the tree based on above decision
 			
            //If we found a leaf, insert!
            if (curr == nullptr) {

            	//Save the new node which is made for later access
            	AVLNode<T>* toReturn;

            	//If we went left
                if (direction) {

                	//Make the parent's left child a new node and save a pointer to it
                    parent->leftChild = new AVLNode<T>(key, parent);
                    toReturn = parent->leftChild;
                }

                //If we went right
                else {

                	//Make the parent's left child a new node and save a pointer to it
                    parent->rightChild = new AVLNode<T>(key, parent);
                    toReturn = parent->rightChild;
                }
 
 				//Rebalance to maintain AVL property
                rebalance(parent);

                //Save an increase in number of words indexed
                numWords++;

                //Return a pointer to the new item
                return toReturn;
            }
        }
    }
 	//Return nullptr by default (this shouldn't ever actually get executed)
    return nullptr;
}

/*
Get a list of the document numbers for docs which contain word 'word'
@param word - the word to look up in the index
@return - a list of doc numbers for docs containing word
*/
template <class T>
vector<int> AVLTree<T>::getDocsForWord(string word){
	//Search for the word
	AVLNode<T>* node = search(word);
	vector<int> docs;

	//If the word was indexed, copy over it's documents
	if(node != nullptr){
		docs = node->docs;
	}
	return docs;
}

/*
Register that a certain document contains a certain word
@param word - the word being indexed
@param docIndex - the index of the document that contains word
*/
template <class T>
void AVLTree<T>::addDocToWord(string word, int docIndex){
	//Search for the word
	AVLNode<T>* node = search(word);

	//If the word was in the index, add the docIndex to its docs vector
	if(node != nullptr){

		node->docs.push_back(docIndex);
	} 
	//If the word is new for the index
	else {

		//Build a new node and put it in the tree
		AVLNode<T>* newNode = insert(word);

		//Build a vector with the doc index and add it to the word node
		vector<int> docs;
		docs.push_back(docIndex);
		newNode->docs = docs;
	}
}

/*
Search the AVL tree using iterative BST search
@param word - the word (key) to search for -- this could be a T (templated) but nah
@return - a pointer to the node whose key value is word if found, or nullptr
*/
template <class T>
AVLNode<T>* AVLTree<T>::search(string word){

	//Start at the root
	AVLNode<T>* curr = root;

	//While in tree and no match
	while(curr != nullptr && curr->key != word){

		//Case go left
		if(curr->key > word){

			//If there's a left child, go left
			if(curr->leftChild != nullptr){
				curr = curr->leftChild;
				continue;
			} 
			//If there's no left child, no match in tree
			else {
				return nullptr;
			}

		} 
		//Case go right
		else if(curr->key < word){

			//If there's a right child, go right
			if(curr->rightChild != nullptr){
				curr = curr->rightChild;
				continue;
			} 
			//If there's no right child, no match in tree
			else {
				return nullptr;
			}
		}
	} 
	//Either found a match or nullptr, return it (if this line is reached, it's a match unless the tree is empty)
	return curr;
}

/*
Getter for the number of words indexed by the AVL tree
@return - the number of nodes in the AVL tree (precalculated)
*/
template<class T>
int AVLTree<T>::getNumWords(){
	return numWords;
}

/*
Public interface method for a postorder file output traversal of the tree
@param os - the ofstream to write to
*/
template<class T>
void AVLTree<T>::postorderFileWrite(ofstream* os){
	//Make a recursive call from the root
	postorder(root, os);
}

/*
Private recursive method for a postorder file output traversal of the tree
@param curr - the current node
@param os - the ofstream to write to
*/
template<class T>
void AVLTree<T>::postorder(AVLNode<T>* curr, ofstream* os){

	//Base case
	if(curr == nullptr) return;

	//Depth-first, fam (recursive call)
	postorder(curr->leftChild, os);
	postorder(curr->rightChild, os);

	//write out the word name
	*os << curr->key;

	//Write out the doc indeces of all docs containing this word
	for(int i = 0; i < curr->docs.size(); i++){
			*os << " " << curr->docs.at(i) << " ";  
	}

	//Write out word delimiter 3
	*os << " <3>\n";
}

