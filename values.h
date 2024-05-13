#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

#ifndef VALUES_H
#define VALUES_H

struct node {

  node *next;
  node *prev;

  int value;
  std::string name;

};

void insert(node **_head, std::string _name, int _value) {

  node *temp = new node();

  temp->name = _name;
  temp->value = _value;

  if (*_head == nullptr) {

    temp->next = nullptr;
    temp->prev = nullptr;
    *_head = temp;

    return;

  } else {

    node *temp_head = *_head;

    while (temp_head->next != nullptr) {

      temp_head = temp_head->next;
      
    }

    temp_head->next = temp;
    temp->prev = temp_head;
    temp->next = nullptr;

    return;
    
  }
  
}

void print_all(node *_head, std::ofstream &ofs) {

  node *temp = _head;

  if (temp == nullptr) {

    return;
  }
  while (temp->next != nullptr) {

    ofs << temp->name << " = " << temp->value << std::endl;
    temp = temp->next;
  }

  ofs << temp->name << " = " << temp->value << std::endl;

  return;
  
}

void print_reverse(node *_head) {

  node *temp = _head;

  if (temp == nullptr) {

    return;
  }
  while (temp->next != nullptr) {

    temp = temp->next;
  }

  while (temp->prev != nullptr) {

    temp = temp->prev;

  }

  return;
  
}

node* find_value(node** _head, std::string _name) {

  node* temp = *_head;

  while (temp != nullptr) {
    
    if (_name  == temp -> name) {

      return temp;
      
    } else {

      temp = temp -> next;
      
    }
    
  }

  return nullptr;
  
}

#endif
