#ifndef _NODE
#define _NODE
#include<memory>
#include<utility>
using namespace std;
/*AVL树的结点类型，成员变量有关键字、数据、平衡因子、智能指针变量指向子树*/
template<typename T>
class Node{
public:
  int key;
  T data;
  int balancefactor;  //绝对值大于等于 2 说明AVL树失衡
  shared_ptr<Node<T>> lptr;
  shared_ptr<Node<T>> rptr;
public:
  Node(const int& _key,const T& _data);
  Node(const pair<int,T>& _pair);
  Node(const Node& _node);  //浅拷贝
  Node(Node&& _node);
  bool isBalance() const;
  void resetkey(const pair<int,T>& _pair);
};

template<typename T>
Node<T>::Node(const int& _key,const T& _data):key(_key),data(_data),\
              balancefactor(0),lptr(nullptr),rptr(nullptr){};

template<typename T>
Node<T>::Node(const pair<int,T>& _pair):balancefactor(0),lptr(nullptr),\
              rptr(nullptr) {
  key = _pair.first;
  data = _pair.second;
}
template<typename T>
Node<T>::Node(const Node& _node){
  key = _node.key;
  data = _node.data;
  balancefactor = _node.balancefactor;
  lptr = _node.lptr;
  rptr = _node.rptr;
}

template<typename T>
Node<T>::Node(Node&& _node){
  key = _node.key;
  data = _node.data;
  balancefactor = _node.balancefactor;
  lptr = _node.lptr;
  rptr = _node.rptr;
}

template <typename T>
bool Node<T>::isBalance() const
{
  if(balancefactor < 2 && balancefactor > -2) return true;
  else return false;
}

template <typename T>
void Node<T>::resetkey(const pair<int, T> &_pair)
{
  key = _pair.first;
  data = _pair.second;
}
#endif