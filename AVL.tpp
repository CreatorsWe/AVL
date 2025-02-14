#include"Node.h"
#include"AVL.h"


template <typename T>
typename AVL<T>::ShareNode Copy(const typename AVL<T>::ShareNode &_node) {
  if(_node == nullptr) return typename AVL<T>::ShareNode();
  //深拷贝当前结点
  typename AVL<T>::ShareNode newnode = make_shared<Node<T>>(_node->key,_node->data);
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
  std::cout<<"("<<_node->key<<","<<_node->balancefactor<<")"<<std::endl;
  preOrder(_node->lptr);
  preOrder(_node->rptr);
}

template <typename T>
inline void AVL<T>::midOrder(const typename AVL<T>::ShareNode &_node) const
{
  if(_node == nullptr) return;
  midOrder(_node->lptr);
  std::cout<<"("<<_node->key<<","<<_node->balancefactor<<")"<<std::endl;
  midOrder(_node->rptr);
}

template <typename T>
inline void AVL<T>::Insert(typename AVL<T>::ShareNode &_node, const int &_key, \
                    const T &_data, bool &_isupdate)
{
  if(_node == nullptr){ //找到插入位置，插入关键字
    _node = make_shared<Node<T>>(_key,_data);
    ++size;
    return;
  }
  if(_key > _node->key) Insert(_node->rptr,_key,_data,_isupdate);
  else Insert(_node->lptr,_key,_data,_isupdate);
  /*检查并处理失衡*/
  if(_isupdate) Handle(_node,1,_isupdate);
  return;
}

template <typename T>
inline bool AVL<T>::Remove(typename AVL<T>::ShareNode &_node, const int &_key, \
                    bool &_isupdate)
{
  if(_node == nullptr) return false;  //未找到
  if(_key == _node->key){ //找到分为三种情况：叶子、一棵子树、两棵子树
    if(_node->lptr == nullptr && _node->rptr == nullptr){
      /*叶子结点直接删除*/
      _node = nullptr;
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
    _node->resetkey(leftMax(_node,_isupdate));
    //2.检查并处理 _node 的平衡，向下查找路径不包括_node，所以_node为检查平衡
    if(_isupdate) Handle(_node,0,_isupdate);
    --size;
    return true;
  }
  bool result; //保存递归返回值
  if(_key > _node->key) result = Remove(_node->rptr,_key,_isupdate);
  else result = Remove(_node->lptr,_key,_isupdate);
  //检查并处理失衡
  if(_isupdate) Handle(_node,0,_isupdate);
  return result;  
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
inline void AVL<T>::Balance(typename AVL<T>::ShareNode &_node, const UnbalanceType &_type)
{
  //平衡或结点不存在或错误，返回
  if(_node->isBalance() || _node == nullptr || _type == UnbalanceType::NOT) return; 
  if(_type == UnbalanceType::LL){ //LL失衡，右旋，将_node的左子树转上来
    rightRotate(_node);
    return;
  }
  else if(_type == UnbalanceType::RR){ //RR失衡，左旋
    leftRotate(_node);
    return;
  }
  /* 先对_node的左孩子左旋变成LL型，再对_node右旋 */
  else if(_type == UnbalanceType::LR){ 
    leftRotate(_node->lptr);
    rightRotate(_node);
    return;
  }
  else if(_type == UnbalanceType::RL){
    rightRotate(_node->rptr);
    leftRotate(_node);
    return;
  }
}

template <typename T>
inline void AVL<T>::leftRotate(typename AVL<T>::ShareNode &_node)
{
  //1.备份node防止智能指针改变造成内存释放
  auto tmp = _node;
  //2.更新 指向 _node 的指针指向 _node 的右孩子
  _node = tmp->rptr;
  /*3.将原来 _node 的右指针指向 原来 _node 的右孩子的左指针
  (现在tmp指向原来_node，_node指向原来_node的右孩子)*/
  tmp->rptr = _node->lptr;
  //4.将原来 _node 的右孩子的左指针指向 原来的_node
  _node->lptr = tmp;
  //5.更新这两个调整点的平衡因子
  tmp->balancefactor = getBalanceFactor(tmp);
  _node->balancefactor = getBalanceFactor(_node);
}

template <typename T>
inline void AVL<T>::rightRotate(typename AVL<T>::ShareNode &_node)
{

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
}

template <typename T>
inline void AVL<T>::Handle(typename AVL<T>::ShareNode &_node, const int& _flag,\
                    bool &_isupdate)
{
  /*判断当前结点因子是否失衡，不是则返回，是则判断失衡类型并调整*/
  int tmp = getBalanceFactor(_node);
  /*插入：如果更新后平衡因子为 0 ，不会对上层平衡因子产生影响，不用向上更新*/
  if(_flag == 1 && _node->balancefactor != 0 && tmp == 0) _isupdate = false;                       
  /* 删除：如果更新前后平衡因子不变，不会对上层平衡因子产生影响，不用向上更新 */
  else if(_flag == 0 && tmp == _node->balancefactor) _isupdate == false;
  /*更新该结点平衡因子*/
  _node->balancefactor = tmp;
  if(_node->isBalance()) return;
  UnbalanceType type = jugleUnbalanceType(_node);
  cout<<static_cast<int>(type)<<endl;
  Balance(_node,type);
  /*插入：处理结点失衡后，node平衡因子为 0 ，不需要向上更新 */
  if(_flag == 1) _isupdate = false;
}

template <typename T>
pair<int,T> AVL<T>::leftMax(typename AVL<T>::ShareNode &_node, bool &_isupdate)
{
  /* 找到左子树关键字最大结点，删除并返回pair类型，并保证查找路径的平衡 */
  if(_node->rptr == nullptr) {
    auto result = make_pair(_node->key,_node->data);
    if(_node->lptr == nullptr){ //最大结点是叶子结点，直接删除
      _node == nullptr;
      return result;
    }
    //最大结点不是叶子结点，即有左子树，将左子树连接到node上
    auto tmp = _node;
    _node = tmp->lptr;
    return result;
  }
  auto result = leftMax(_node->rptr,_isupdate);
  if(_isupdate) Handle(_node,0,_isupdate);
  return result;
}

template <typename T>
inline AVL<T>::AVL():root(nullptr),size(0){};
template <typename T>
inline AVL<T>::AVL(const int &_key, const T &_data):root(new Node<T>(_key,_data)),size(0){};
template <typename T>
inline AVL<T>::AVL(const pair<int, T> &_pair):root(new Node<T>(_pair)),size(0){};
template <typename T>
inline AVL<T>::AVL(const AVL &_avl){
  root = Copy(_avl.root);
  size = _avl.size;
}
template <typename T>
inline AVL<T>::AVL(AVL &&_avl){
  root = _avl.root;
  size = _avl.size;
};

template <typename T>
inline bool AVL<T>::search(const int &_key) const
{
  if(root == nullptr) return false;
  return Search(root,_key);
}

template <typename T>
inline void AVL<T>::insert(const int &_key, const T &_data)
{
  if(root == nullptr){
    /* 根结点为空，插入到根结点 */
    root.reset(new Node<T>(_key,_data));
    ++size;
    return;
  }
  if(search(_key)) return;  //关键字已存在，不插入
  bool isupdate = true;
  Insert(root,_key,_data,isupdate);
}

template <typename T>
inline void AVL<T>::insert(const pair<int, T> &_pair)
{
  insert(_pair.first,_pair.second);
}

template <typename T>
inline bool AVL<T>::remove(const int &_key)
{
  if(root == nullptr) return false;
  bool isupdate = true;
  return Remove(root,_key,isupdate);
}

template <typename T>
inline void AVL<T>::preorder() const
{
  cout<<"preOrder result:"<<endl;
  if(root == nullptr) cout<<"null"<<endl;
  preOrder(root);
}

template <typename T>
inline void AVL<T>::midorder() const
{
  cout<<"midOrder result:"<<endl;
  if(root == nullptr) cout<<"null"<<endl;
  midOrder(root);
}

template <typename T>
inline int AVL<T>::height() const
{
  return Height(root);
}

template <typename T>
inline int AVL<T>::lenght() const
{
  return size;
}

template <typename T>
inline bool AVL<T>::empty() const
{
  if(size == 0) return true;
  else return false;
}
