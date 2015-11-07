#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

/* AVL node */
template <class T>
class AVLnode {
public:
    T key;
    int balance;
    vector<int> docs;
    AVLnode *left, *right, *parent;
 
    AVLnode(T k, AVLnode *p) : key(k), balance(0), parent(p),
                        left(NULL), right(NULL) {}
 
    ~AVLnode() {
        delete left;
        delete right;
    }
};
 
/* AVL tree */
template <class T>
class AVLtree {
public:
    AVLtree(void);
    ~AVLtree(void);
    AVLnode<T>* insert(T key);
    void deleteKey(const T key);
    void printBalance();

    vector<int> getDocsForWord(string word);
    void addDocToWord(string word, int docIndex);
    AVLnode<T>* search(string word);
 
private:
    AVLnode<T> *root;
 
    AVLnode<T>* rotateLeft          ( AVLnode<T> *a );
    AVLnode<T>* rotateRight         ( AVLnode<T> *a );
    AVLnode<T>* rotateLeftThenRight ( AVLnode<T> *n );
    AVLnode<T>* rotateRightThenLeft ( AVLnode<T> *n );
    void rebalance                  ( AVLnode<T> *n );
    int height                      ( AVLnode<T> *n );
    void setBalance                 ( AVLnode<T> *n );
    void printBalance               ( AVLnode<T> *n );
    void clearNode                  ( AVLnode<T> *n );
};
 
/* AVL class definition */
template <class T>
void AVLtree<T>::rebalance(AVLnode<T> *n) {
    setBalance(n);
 
    if (n->balance == -2) {
        if (height(n->left->left) >= height(n->left->right))
            n = rotateRight(n);
        else
            n = rotateLeftThenRight(n);
    }
    else if (n->balance == 2) {
        if (height(n->right->right) >= height(n->right->left))
            n = rotateLeft(n);
        else
            n = rotateRightThenLeft(n);
    }
 
    if (n->parent != NULL) {
        rebalance(n->parent);
    }
    else {
        root = n;
    }
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateLeft(AVLnode<T> *a) {
    AVLnode<T> *b = a->right;
    b->parent = a->parent;
    a->right = b->left;
 
    if (a->right != NULL)
        a->right->parent = a;
 
    b->left = a;
    a->parent = b;
 
    if (b->parent != NULL) {
        if (b->parent->right == a) {
            b->parent->right = b;
        }
        else {
            b->parent->left = b;
        }
    }
 
    setBalance(a);
    setBalance(b);
    return b;
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateRight(AVLnode<T> *a) {
    AVLnode<T> *b = a->left;
    b->parent = a->parent;
    a->left = b->right;
 
    if (a->left != NULL)
        a->left->parent = a;
 
    b->right = a;
    a->parent = b;
 
    if (b->parent != NULL) {
        if (b->parent->right == a) {
            b->parent->right = b;
        }
        else {
            b->parent->left = b;
        }
    }
 
    setBalance(a);
    setBalance(b);
    return b;
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateLeftThenRight(AVLnode<T> *n) {
    n->left = rotateLeft(n->left);
    return rotateRight(n);
}
 
template <class T>
AVLnode<T>* AVLtree<T>::rotateRightThenLeft(AVLnode<T> *n) {
    n->right = rotateRight(n->right);
    return rotateLeft(n);
}
 
template <class T>
int AVLtree<T>::height(AVLnode<T> *n) {
    if (n == NULL)
        return -1;
    return 1 + std::max(height(n->left), height(n->right));
}
 
template <class T>
void AVLtree<T>::setBalance(AVLnode<T> *n) {
    n->balance = height(n->right) - height(n->left);
}
 
template <class T>
void AVLtree<T>::printBalance(AVLnode<T> *n) {
    if (n != NULL) {
        printBalance(n->left);
        std::cout << n->balance << " ";
        printBalance(n->right);
    }
}
 
template <class T>
AVLtree<T>::AVLtree(void) : root(NULL) {}
 
template <class T>
AVLtree<T>::~AVLtree(void) {
    delete root;
}
 
template <class T>
AVLnode<T>* AVLtree<T>::insert(T key) {
    if (root == NULL) {
        root = new AVLnode<T>(key, NULL);
        return root;
    }
    else {
        AVLnode<T>
            *n = root,
            *parent;
 
        while (true) {
            if (n->key == key)
                return nullptr;
 
            parent = n;
 
            bool goLeft = n->key > key;
            n = goLeft ? n->left : n->right;

            AVLnode<T>* toReturn;
 
            if (n == NULL) {
                if (goLeft) {
                    parent->left = new AVLnode<T>(key, parent);
                    toReturn = parent->left;
                }
                else {
                    parent->right = new AVLnode<T>(key, parent);
                    toReturn = parent->right;
                }
 
                rebalance(parent);
                return toReturn;
            }
        }
    }
 
    return nullptr;
}
 
template <class T>
void AVLtree<T>::deleteKey(const T delKey) {
    if (root == NULL)
        return;
 
    AVLnode<T>
        *n       = root,
        *parent  = root,
        *delNode = NULL,
        *child   = root;
 
    while (child != NULL) {
        parent = n;
        n = child;
        child = delKey >= n->key ? n->right : n->left;
        if (delKey == n->key)
            delNode = n;
    }
 
    if (delNode != NULL) {
        delNode->key = n->key;
 
        child = n->left != NULL ? n->left : n->right;
 
        if (root->key == delKey) {
            root = child;
        }
        else {
            if (parent->left == n) {
                parent->left = child;
            }
            else {
                parent->right = child;
            }
 
            rebalance(parent);
        }
    }
}
 
template <class T>
void AVLtree<T>::printBalance() {
    printBalance(root);
    std::cout << std::endl;
}

template <class T>
vector<int> AVLtree<T>::getDocsForWord(string word){
	AVLnode<T>* node = search(word);
	vector<int> docs;
	if(node != nullptr){
		docs = node->docs;
	}
	return docs;
}

template <class T>
void AVLtree<T>::addDocToWord(string word, int docIndex){
	AVLnode<T>* node = search(word);

	if(node != nullptr){

		node->docs.push_back(docIndex);
	} else {

		AVLnode<T>* newNode = insert(word);

		vector<int> docs;
		docs.push_back(docIndex);
		newNode->docs = docs;

	}
}

template <class T>
AVLnode<T>* AVLtree<T>::search(string word){
	AVLnode<T>* curr = root;
	while(curr != nullptr && curr->key != word){
		if(curr->key > word){
			if(curr->left != nullptr){
				curr = curr->left;
				continue;
			} else {
				return nullptr;
			}
		} else if(curr->key < word){
			if(curr->right != nullptr){
				curr = curr->right;
				continue;
			} else {
				return nullptr;
			}
		}
	} 
	return curr;
}

