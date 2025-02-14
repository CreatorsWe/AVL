#include"AVL.h"
int main(){
  AVL<int> avl;
  avl.insert(4,6);
  avl.insert(3,7);
  avl.insert(2,6);
  avl.remove(4);
  avl.midorder();
  avl.preorder();
  return 0;
}