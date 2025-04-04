#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);
    void rotateLeft(AVLNode<Key, Value>* current);
    void rotateRight(AVLNode<Key, Value>* current);
    void insertFix(AVLNode<Key, Value>* child, AVLNode<Key, Value>* parent);
    void removeFix(AVLNode<Key, Value>* child, int diff);
    int findBalance(AVLNode<Key, Value>* node) const;

};

// HELPER FUNCTIONS FOR INSERT
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* current)
{
  
  AVLNode<Key, Value>* rightChild = current->getRight();

  // Case where there is not rightChild
  if(rightChild == nullptr){
    return; // No rotation needed
  }

  current->setRight(rightChild->getLeft());
  if(rightChild->getLeft() != nullptr){
    rightChild->getLeft()->setParent(current);
  }

  // Making rightChild the new root
  rightChild->setLeft(current);
  rightChild->setParent(current->getParent());

  // Updating current's parent to rightChild
  if(current->getParent() != nullptr){
    if(current->getParent()->getLeft() == current){
      current->getParent()->setLeft(rightChild);
    }
    else{
      current->getParent()->setRight(rightChild);
    }
  }
  else{
    // Case if current was the root
    this->root_ = rightChild;
  }

  current->setParent(rightChild);

  // Adjusting balances
  int8_t currBalance = current->getBalance();
  int8_t rightBalance = rightChild->getBalance();

  current->setBalance(currBalance - std::max(rightBalance, static_cast<int8_t>(0)) - 1);
  rightChild->setBalance(rightBalance - std::min(current->getBalance(), static_cast<int8_t>(0)) + 1);
  
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* current)
{

  AVLNode<Key, Value>* leftChild = current->getLeft();

  // Case where there is not leftChild
  if(leftChild == nullptr){
    return; // No rotation needed
  }

  current->setLeft(leftChild->getRight());
  if(leftChild->getRight() != nullptr){
    leftChild->getRight()->setParent(current);
  }

  // Making leftChild the new root
  leftChild->setRight(current);
  leftChild->setParent(current->getParent());

  // Updating current's parent to leftChild
  if(current->getParent() != nullptr){
    if(current->getParent()->getLeft() == current){
      current->getParent()->setLeft(leftChild);
    }
    else{
      current->getParent()->setRight(leftChild);
    }
  }
  else{
    // Case if current was the root
    this->root_ = leftChild;
  }

  current->setParent(leftChild);

  // Adjusting balances
  int8_t currBalance = current->getBalance();
  int8_t leftBalance = leftChild->getBalance();

  current->setBalance(currBalance - std::min(leftBalance, static_cast<int8_t>(0)) + 1);
  leftChild->setBalance(leftBalance - std::max(current->getBalance(), static_cast<int8_t>(0)) - 1);

}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* child, AVLNode<Key, Value>* parent)
{

  // Need to traverse the tree and check if ancestors have become unblananced post-insertion
  while(parent != nullptr){

    // If left-heavy
    if(parent->getBalance() == -2){
      // LL case
      if(child->getBalance() == -1){
        rotateRight(parent);
        // Update balances
        parent->setBalance(0);
        child->setBalance(0);
      }
      
      // LR case 
      else if(child->getBalance() == 1){
        AVLNode<Key, Value>* rightChild = child->getRight();
        rotateLeft(child);
        rotateRight(parent);

        // Update balances
        if(rightChild->getBalance() == 1){
          parent->setBalance(0);
          child->setBalance(-1);
        }
        else if(rightChild->getBalance() == 0){
          parent->setBalance(0);
          child->setBalance(0);
        }
        else{
          parent->setBalance(-1);
          child->setBalance(0);
        }

        rightChild->setBalance(0);
      }
      return;
    }

    // If right-heavy
    else if(parent->getBalance() == 2){
      // RR case
      if(child->getBalance() == 1){
        rotateLeft(parent);
        // Update balances
        parent->setBalance(0);
        child->setBalance(0);
      }

      // RL case
      else if(child->getBalance() == -1){
        AVLNode<Key, Value>* leftChild = child->getLeft();
        rotateRight(child);
        rotateLeft(parent);

        // Update balances
        if(leftChild->getBalance() == -1){
          parent->setBalance(0);
          child->setBalance(-1);
        }
        else if(leftChild->getBalance() == 0){
          parent->setBalance(0);
          child->setBalance(0);
        }
        else{
          parent->setBalance(-1);
          child->setBalance(0);
        }
        
        leftChild->setBalance(0);
      }
      return;
    }

    parent = parent->getParent();
    child = parent;
  }
}


/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
  // Using the insert() function from BST
  // Base case: empty tree
  if(this->root_ == nullptr){
    // Create new node and set it as the root
    this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    return;
  }

  // Now we need to traverse the tree, checking how new_item's key compares to each node's key
  AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
  AVLNode<Key, Value>* parent = nullptr;

  while(current != nullptr){
    parent = current;
    // Case where the inserted node is less than current node -> move left
    if(new_item.first < current->getItem().first){
      current = current->getLeft();
    }
    // Case where the inserted node is greater than current node -> move right
    else if(new_item.first > current->getItem().first){
      current = current->getRight();
    }
    // Case where the inserted node is the same as current node -> overwrite value
    else{
      current->setValue(new_item.second);
      return;
    }
  }

  // Now we have reached the spot where we can insert the new node
  AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
  if(new_item.first < parent->getItem().first){
    parent->setLeft(newNode);

  }
  else{
    parent->setRight(newNode);
    // parent->setBalance(parent->getBalance() + 1); // Updating balance

  }

  AVLNode<Key, Value>* child = newNode;
  AVLNode<Key, Value>* node = parent;

  while(node != nullptr){
    if(child == node->getLeft()){
      node->setBalance(node->getBalance() - 1);
    }
    else{
      node->setBalance(node->getBalance() + 1);
    }

    if(node->getBalance() == 0){
      break;
    }
    // Handling the case where new balance is not 0
    else if(node->getBalance() == 2 || node->getBalance() == -2){
      insertFix(child, node);
      break;
    }

    child = node;
    node = node->getParent();
  }

}

// HELPER FUNCTIONS FOR REMOVE
template<typename Key, typename Value>
int AVLTree<Key, Value>::findBalance(AVLNode<Key, Value>* node) const{

  return findHeight(node->getRight()) - findHeight(node->getLeft());

}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* current, int diff)
{
  if(current == nullptr){
    return;
  }

  AVLNode<Key, Value>* parent = current->getParent();
  int ndiff = 0;

  if(parent != nullptr){
    if(current == parent->getLeft()){
      ndiff = -1;
    }
    else{
      ndiff = 1;
    }
  }

  int spread = current->getBalance() + diff;

  // WHERE DIFF == -1
  if(diff == -1){
    AVLNode<Key, Value>* child = static_cast<AVLNode<Key, Value>*>(current->getLeft());
    int childBalance = 0;

    if(child != nullptr){
      childBalance = child->getBalance();
    }
    // Case 1: balance + diff = -2
    if(spread == -2){
      // Case 1a: balance of child = -1
      if(childBalance == -1){
        rotateRight(current);
        removeFix(parent, ndiff);
      }
      // Case 1b: balance of child = 0
      if(childBalance == 0){
        rotateRight(current);

      }
      // Case 1c: balance of child = 1
      if(childBalance == 1){
        // AVLNode<Key, Value>* grandchild = child->getRight();
        rotateLeft(child);
        rotateRight(child);

        removeFix(parent, ndiff);
      }

    }

    // Case 2: balance + diff = -1
    // Case 3: balance + diff = 0
    else if(spread == 0){
      removeFix(parent, ndiff);
    }

  }

  // WHERE DIFF == 1
  else if(diff == 1){
    AVLNode<Key, Value>* child = static_cast<AVLNode<Key, Value>*>(current->getRight());
    int childBalance = 0;
    
    if(child != nullptr){
      childBalance = child->getBalance();
    }
    // Case 1: balance + diff = 2
    if(spread == 2){
      // Case 1a: balance of child = 1
      if(childBalance == 1){
        rotateLeft(current);
        removeFix(parent, ndiff);
      }
      // Case 1b: balance of child = 0
      if(childBalance == 0){
        rotateLeft(current);

      }
      // Case 1c: balance of child = -1
      if(childBalance == -1){
        // AVLNode<Key, Value>* grandchild = child->getRight();
        rotateRight(child);
        rotateLeft(child);

        removeFix(parent, ndiff);
      }

    }

    // Case 2: balance + diff = -1
    // Case 3: balance + diff = 0
    else if(spread == 0){
      removeFix(parent, ndiff);
    }
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
  // Using my remove() function from BST
  AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
  int diff = 0;

  while(current != nullptr && current->getKey() != key){
    // parent = current; // Moving down the tree
    if(key < current->getKey()){
      current = current->getLeft();
    }
    else{
      current = current->getRight();
    }
  }

  // Case where the key wasn't found
  if(current == nullptr){
    return;
  }

  // Now that we've found the node to remove, we need to actually remove it
  AVLNode<Key, Value>* parent = current->getParent();

  // Case 1: Has no children
  if(current->getLeft() == nullptr && current->getRight() == nullptr){
    // Case where it's the root
    if(current == this->root_){
      this->root_ = nullptr;
    }
    // Handling where the parent points to now that we're deleting this node
    else if(parent->getLeft() == current){
      parent->setLeft(nullptr);
      diff = 1;
    }
    else{
      parent->setRight(nullptr);
      diff = -1;
    }

    delete current;
    removeFix(parent, diff);
  }

  // Case 2: Only one child
  else if(current->getLeft() == nullptr || current->getRight() == nullptr){
    // Finding whether the child is a left or right
    AVLNode<Key, Value>* child;
    if(current->getLeft() == nullptr){
      child = current->getRight();
    }
    else{
      child = current->getLeft();
    }

    // Case where it's the root
    if(this->root_ == current){
      this->root_ = child;
      this->root_->setParent(nullptr);
    }
    else if(current->getParent()->getLeft() == current){
      current->getParent()->setLeft(child);
      child->setParent(current->getParent());
      diff = 1;
    }
    else{
      current->getParent()->setRight(child);
      child->setParent(current->getParent());
      diff = -1;
    }

    delete current;
    current = nullptr;
    removeFix(parent, diff);
  }

  // Case 3: Both children
  else{
    // Will be what we delete at the end
    AVLNode<Key, Value>* nodeToDelete = current;

    AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(current));
    nodeSwap(current, pred);

    // Post-swap, we need to check if pred has children that need to be reassigned
    AVLNode<Key, Value>* child = nullptr;
    if(nodeToDelete->getLeft() != nullptr){
      child = nodeToDelete->getLeft();
    }
    else if(nodeToDelete->getRight() != nullptr){
      child = nodeToDelete->getRight();
    }

    AVLNode<Key, Value>* parent = nodeToDelete->getParent();

    if(child !=nullptr){
      child->setParent(parent);
    }

    // Update the parent's left or right pointer if our nodeToDelete has a parent
    if(parent != nullptr){
      if(parent->getLeft() == nodeToDelete){
        parent->setLeft(child);
        diff = 1;
      }
      else{
        parent->setRight(child);
        diff = -1;
      }
    }
    // Case where our nodeToDelete is the root
    else{
      this->root_ = child;
    }

    delete nodeToDelete;

    removeFix(parent, diff);
  } 
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
