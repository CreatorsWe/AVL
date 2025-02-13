#include<iostream>
#include<memory>
using namespace std;
template<typename T>
class Node;

const enum class UnbalanceType{LL,RR,LR,RL,NOT}; //失衡类型

template<typename T>
class AVL{
private:  //给长类型取别名
  using ShareNode = shared_ptr<Node<T>>;
private:  //私有成员变量
  typename ShareNode root;
  int size;
private:  //一些辅助函数
/*复制构造函数辅助函数*/
  typename ShareNode Copy(const ShareNode& _node);
  /*返回以_node为根结点的树的高度*/
  int Height(const ShareNode& _node) const;
  bool Search(const ShareNode& _node,const int& _key) const;
  void preOrder(const ShareNode& _node) const;
  void midOrder(const ShareNode& _node) const;
  void Insert(const ShareNode& _node,const int& _key,const T& _data,\
             bool& _isupdate);
  bool Remove(const ShareNode& _node,const int& _key,bool& _isupdate);
  /*判断失衡类型*/
  UnbalanceType jugleUnbalanceType(const ShareNode& _node) const;
  /*计算结点的平衡因子*/
  int getBalanceFactor(const ShareNode& _node) const;
  /*处理结点失衡*/
  void Balance(const ShareNode& _node,const UnbalanceType& _type,bool& _isupdate);
  /*由于处理检查并处理失衡语句相同，所以封装为一个函数*/
  void Handle(const ShareNode& _node,bool& _isupdate);
  /*删除非叶子节点时该函数删除并返回左子树最大关键字，并保证该函数查找的路径平衡*/
  T leftMax(const ShareNode& _node,bool& _isupdate);
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

template <typename T>
typename AVL<T>::ShareNode Copy(const typename AVL<T>::ShareNode &_node) {
  if(_node == nullptr) return typename ShareNode();
  //深拷贝当前结点
  typename ShareNode newnode = typename ShareNode(new Node<T>(_node->key,_node->data));
  newnode->lptr = Copy(_node->lptr);
  newnode->rptr = Copy(_node->rptr);
  return newnode;
}

template <typename T>
inline int AVL<T>::Height(const typename AVL<T>::ShareNode &_node) const
{
  if(_node == nullptr) return 0;
  int leftheight = Height(_node->lptr);
  int rightheight = Height(_node->rptr);
  return leftheight > rightheight ? leftheight + 1 : rightheight + 1;
}

template <typename T>
inline bool AVL<T>::Search(const typename AVL<T>::ShareNode &_node, const int &_key) const
{
  if(_node == nullptr) return false;
  if(_key == _node->key) return true;
  if(_key > _node->key) return Search(_node->rptr,_key);
  else return Search(_node->lptr,_key);
}

template <typename T>
inline void AVL<T>::preOrder(const typename AVL<T>::ShareNode &_node) const
{
  if(_node == nullptr) return;
  std::cout<<"("<<_node->key<<","<<_node->data<<","<<_node->balancefactor<<")"<<std::endl;
  preOrder(_node->lptr);
  preOrder(_node->rptr);
}

template <typename T>
inline void AVL<T>::midOrder(const typename AVL<T>::ShareNode &_node) const
{
  if(_node == nullptr) return;
  midOrder(_node->lptr);
  std::cout<<"("<<_node->key<<","<<_node->data<<","<<_node->balancefactor<<")"<<std::endl;
  midOrder(_node->rptr);
}

template <typename T>
inline void AVL<T>::Insert(const typename AVL<T>::ShareNode &_node, const int &_key, \
                    const T &_data, bool &_isupdate)
{
  if(_node == nullptr){ //找到插入位置，插入关键字
    _node.reset(new Node<T>(_key,_data));
    ++size;
    return;
  }
  if(_key > _node->key) Insert(_node->rptr,_key,_data,_isupdate);
  else Insert(_node->lptr,_key,_data,_isupdate);
  /*检查并处理失衡*/
  if(!_isupdate) return;
  else Handle(_node,_isupdate);
}

template <typename T>
inline bool AVL<T>::Remove(const typename AVL<T>::ShareNode &_node, const int &_key, \
                    bool &_isupdate)
{
  if(_node == nullptr) return false;  //未找到
  if(_key == _node->key){ //找到分为三种情况：叶子、一棵子树、两棵子树
    if(_node->lptr == nullptr && _node->rptr == nullptr){
      /*叶子结点直接删除*/
      _node == nullptr;
      --size;
      return true;
    }
    else if(_node->lptr != nullptr && _node->rptr == nullptr){
      /*左子树不为空,右子树为空，将左子树接到父节点指针上*/
      auto tmp = _node; //保存以下，防止改变智能指针造成内存释放
      _node = tmp->lptr;
      --size;
      return true;
    }
    else if(_node->lptr == nullptr && _node->rptr != nullptr){
      /*右子树不为空左子树为空*/
      auto tmp = _node;
      _node = tmp->rptr;
      --size;
      return true;
    }
    /*删除节点有两棵子树*/
    //1.向下查找左子树最大关键字节点，删除并返回，并保证向下查找路径的平衡
    _node->key = leftMax(_node,_isupdate);
    //2.检查并处理 _node 的平衡，向下查找路径不包括_node，所以_node为检查平衡
    if(!_isupdate) return true;
    else Handle(_node,_isupdate);
    --size;
    return true;
  }
  bool result; //保存递归返回值
  if(_key > _node->key) result = Remove(_node->rptr,_key,_isupdate);
  else result = Remove(_node->rptr,_key,_isupdate);
  //检查并处理失衡
  if(!_isupdate) return true;
  else Handle(_node,_isupdate);
}

template <typename T>
inline UnbalanceType AVL<T>::jugleUnbalanceType(const typename AVL<T>::ShareNode &_node) const
{
  if(_node->isBalance() || _node == nullptr) return UnbalanceType::NOT; //结点平衡或不存在，错误调用
  /*第一次判断方向结果，既然失衡，则必然有一棵子树比另一颗高 2 */
  int firstjugle = Height(_node->lptr) > Height(_node->rptr) ? 0 : 1;
  /*第二次判断根结点是第一次判断结果，如果左右子树高度差不同，类型是子树高的那边；
  如果子树高度相同( 删除情况 )，则等于第一次判断结果。*/
  int secondjugle = -1;
  if(firstjugle == 0) secondjugle = Height(_node->rptr) > Height(_node->lptr) ? 1 : 0;
  else secondjugle = Height(_node->lptr) > Height(_node->rptr) ? 0 : 1;
  /*返回结果*/
  if(firstjugle == 0 && secondjugle == 0) return UnbalanceType::LL;
  else if(firstjugle == 0 && secondjugle == 1) return UnbalanceType::LR;
  else if(firstjugle == 1 && secondjugle == 0) return UnbalanceType::RL;
  else if(firstjugle == 1 && secondjugle == 1) return UnbalanceType::RR;
  else return UnbalanceType::NOT;

}

template <typename T>
inline int AVL<T>::getBalanceFactor(const typename AVL<T>::ShareNode &_node) const
{
  return Height(_node->lptr) - Height(_node->rptr);
}

template <typename T>
inline void AVL<T>::Balance(const typename AVL<T>::ShareNode &_node, \
                    const UnbalanceType &_type, bool &_isupdate)
{
  //平衡或结点不存在或错误，返回
  if(_node->isBalance() || _node == nullptr || _type == UnbalanceType::NOT) return; 
  if(_type == UnbalanceType::LL){ //LL失衡，右旋，将_node的左子树转上来
    //1.备份node防止智能指针改变造成内存释放
    auto tmp = _node;
    //2.更新 指向 _node 的指针指向 _node 的左孩子
    _node = tmp->lptr;
    /*3.将原来 _node 的左指针指向 _node 左孩子的右指针
    (现在tmp指向原来_node，_node指向原来_node的左孩子)*/
    tmp->lptr = _node->rptr;
    //4.将原来 _node 的左孩子的右指针指向 原来的_node
    _node->rptr = tmp;
    //5.更新这两个调整点的平衡因子
    tmp->balancefactor = getBalanceFactor(tmp);
    _node->balancefactor = getBalanceFactor(_node);
    //6.失衡平衡后，失衡点的平衡因子必定更新为 0 ，不会影响上层平衡因子
    _isupdate = false;
    return;
  }
  else if(_type == UnbalanceType::RR){ //RR失衡，左旋

  }
}

template <typename T>
inline void AVL<T>::Handle(const ShareNode &_node, bool &_isupdate)
{
  /*判断当前结点因子是否失衡，不是则返回，是则判断失衡类型并调整*/
  int tmp = getBalanceFactor(_node);
  /*如果更新后平衡因子为 0 ，不会对上层平衡因子产生影响，不需要向上处理*/
  if(_node->balancefactor != 0 && tmp == 0) _isupdate = false;
  _node->balancefactor = tmp;
  if(_node->isBalance()) return;
  UnbalanceType type = jugleUnbalanceType(_node);
  Balance(_node,type,_isupdate);
}
