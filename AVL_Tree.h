//==================================================================
// Code203_Tree.h
// Demonstration of an AVL tree which keeps the tree nodes in as
//   near perfect balance as possible.
// Author: Dr. Rick Coleman
//==================================================================
#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct AVLTreeNode
{
   string key;

   vector<int> docs;

   AVLTreeNode *left;
   AVLTreeNode *right;
   AVLTreeNode *parent;
   char balanceFactor;
};

class AVL_Tree
{
   private:
      AVLTreeNode   *root;
      

   public:
      AVL_Tree();            // Constructor
      ~AVL_Tree();           // Destructor
      void Insert(AVLTreeNode *n);
      void restoreAVL(AVLTreeNode *ancestor, AVLTreeNode *newNode);
      void adjustBalanceFactors(AVLTreeNode *end, AVLTreeNode *start);
      void rotateLeft(AVLTreeNode *n);
      void rotateRight(AVLTreeNode *n);
      void adjustLeftRight(AVLTreeNode *end, AVLTreeNode *start);
      void adjustRightLeft(AVLTreeNode *end, AVLTreeNode *start);
      void PrintTree();
      vector<int> getDocsForWord(string word);
      void addDocToWord(string word, int docIndex);
      AVLTreeNode* find(string word);
   private:
      void ClearTree(AVLTreeNode *n);
      void Print(AVLTreeNode *n);
};

#endif
