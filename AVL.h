#ifndef _AVL
#define _AVL
#include<iostream>
#include<memory>
using namespace std;

template<typename T>
class Node;

enum class UnbalanceType{LL,RR,LR,RL,NOT}; //失衡类型

template<typename T>
class AVL{
private:
  using ShareNode = shared_ptr<Node<T>>;
private:  //私有成员变量
  ShareNode root;
  int size;
private:  //一些辅助函数
/*复制构造函数辅助函数*/
  ShareNode Copy(const ShareNode& _node);
  /*返回以_node为根结点的树的高度*/
  int Height(const ShareNode& _node) const;
  bool Search(const ShareNode& _node,const int& _key) const;
  void preOrder(const ShareNode& _node) const;
  void midOrder(const ShareNode& _node) const;
  void Insert(ShareNode& _node,const int& _key,const T& _data,\
             bool& _isupdate);
  bool Remove(ShareNode& _node,const int& _key,bool& _isupdate);
  /*判断失衡类型*/
  UnbalanceType jugleUnbalanceType(const ShareNode& _node) const;
  /*计算结点的平衡因子*/
  int getBalanceFactor(const ShareNode& _node) const;
  /*仅调用左旋右旋处理结点失衡*/
  void Balance(ShareNode& _node,const UnbalanceType& _type);
  /*左旋右旋函数*/
  void leftRotate(ShareNode& _node);
  void rightRotate(ShareNode& _node);
  /* Handle函数是封装函数，用于处理失衡，flag区分是处理插入失衡
  还是删除失衡，它们向上更新的策略不同 */
  void Handle(ShareNode& _node,const int& _flag,bool& _isupdate);
  /*删除非叶子节点时该函数删除并返回左子树最大关键字，并保证该函数查找的路径平衡*/
  pair<int,T> leftMax(ShareNode& _node,bool& _isupdate);
public:
  AVL();
  AVL(const int& _key,const T& _data);
  AVL(const pair<int,T>& _pair);
  AVL(const AVL& _avl);
  AVL(AVL&& _avl);
  bool search(const int& _key) const;
  void insert(const int& _key,const T& _data);
  void insert(const pair<int,T>& _pair);
  bool remove(const int& _key);
  void preorder() const;
  void midorder() const;
  int height() const;
  int lenght() const;
  bool empty() const;
};

#include"AVL.tpp"

#endif