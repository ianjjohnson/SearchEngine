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
 
template <class T>
AVLNode<T>* AVLTree<T>::insert(T key) {
    if (root == nullptr) {
        root = new AVLNode<T>(key, nullptr);
        return root;
    }
    else {
        AVLNode<T>
            *n = root,
            *parent;
 
        while (true) {
            if (n->key == key)
                return nullptr;
 
            parent = n;
 
            bool goLeft = n->key > key;
            n = goLeft ? n->leftChild : n->rightChild;

            AVLNode<T>* toReturn;
 
            if (n == nullptr) {
                if (goLeft) {
                    parent->leftChild = new AVLNode<T>(key, parent);
                    toReturn = parent->leftChild;
                }
                else {
                    parent->rightChild = new AVLNode<T>(key, parent);
                    toReturn = parent->rightChild;
                }
 
                rebalance(parent);
                numWords++;
                return toReturn;
            }
        }
    }
 
    return nullptr;
}

template <class T>
vector<int> AVLTree<T>::getDocsForWord(string word){
	AVLNode<T>* node = search(word);
	vector<int> docs;
	if(node != nullptr){
		docs = node->docs;
	}
	return docs;
}

template <class T>
void AVLTree<T>::addDocToWord(string word, int docIndex){
	AVLNode<T>* node = search(word);

	if(node != nullptr){

		node->docs.push_back(docIndex);
	} else {

		AVLNode<T>* newNode = insert(word);

		vector<int> docs;
		docs.push_back(docIndex);
		newNode->docs = docs;
	}
}

template <class T>
AVLNode<T>* AVLTree<T>::search(string word){
	AVLNode<T>* curr = root;
	while(curr != nullptr && curr->key != word){
		if(curr->key > word){
			if(curr->leftChild != nullptr){
				curr = curr->leftChild;
				continue;
			} else {
				return nullptr;
			}
		} else if(curr->key < word){
			if(curr->rightChild != nullptr){
				curr = curr->rightChild;
				continue;
			} else {
				return nullptr;
			}
		}
	} 
	return curr;
}

template<class T>
int AVLTree<T>::getNumWords(){
	return numWords;
}

template<class T>
void AVLTree<T>::postorderFileWrite(ofstream* os){
	postorder(root, os);
}

template<class T>
void AVLTree<T>::postorder(AVLNode<T>* curr, ofstream* os){
	if(curr == nullptr) return;
	postorder(curr->leftChild, os);
	postorder(curr->rightChild, os);

	*os << curr->key;
	for(int i = 0; i < curr->docs.size(); i++){
			*os << " " << curr->docs.at(i) << " ";  
	}
	*os << " <3>\n";
}

