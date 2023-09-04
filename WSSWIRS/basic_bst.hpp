#pragma once
#pragma once

#include "utility.h"
#include <queue>
#include <assert.h>
#include <random>
#include <iostream>
#include <unordered_map>
using namespace std;

class bstMethod {
public:
	struct TreeNode {
		//TreeNode() {}
		//~TreeNode() {}
		Element ele;
		int height;
		double sub_weight;
		TreeNode* lchild;
		TreeNode* rchild;
	};

	int exact_delete_key;
	TreeNode* root;
	double tot_weight;

	double node_weight(TreeNode* tmp_node) {
		return (tmp_node == NULL) ? 0 : tmp_node->sub_weight;
	}

	void update_weight(TreeNode* tmp_node) {
		if (tmp_node == NULL)
			return;
		tmp_node->sub_weight = tmp_node->ele.weight + node_weight(tmp_node->lchild) + node_weight(tmp_node->rchild);
		return;
	}

    bstMethod(int num, vector<Element>& all_ele) {
//        tot_weight = 0;
//        root = build(0,num-1,all_ele);
//        tot_weight = root->sub_weight;
        root = NULL;
        int k = 0;
        for (auto i : all_ele) {
            insert_element(i);
//            if(k%1000000 == 0){
//                end = chrono::system_clock::now();
//                elapsed_seconds = end - start;
//                cout << "basic bst tree build_time:" << elapsed_seconds.count() << "\n";
//                cout<<i.key<<' '<<i.value<<' '<<i.weight<<endl;
//                start=end;
//            }
//            cerr << k <<" "<<root->height << "\n";
            k++;
        }
		cerr <<"BST_tot_weight" << tot_weight << "\n";
    }

    TreeNode* build(int L,int R, vector<Element>& all_ele){
        if(L==R)
            return New_Node(all_ele[L],NULL,NULL);
        if(L>R)
            return NULL;
        int mid=(L+R)/2;
        TreeNode *LNode = build(L,mid-1,all_ele);
        TreeNode *RNode = build(mid+1,R,all_ele);

        int height = max(getHeight(LNode), getHeight(RNode)) + 1;
        auto node =  New_Node(all_ele[mid],LNode,RNode,height);
        update_weight(node);
        return node;
    }

	TreeNode* New_Node(Element tmp_ele, TreeNode* lchild, TreeNode* rchild, int height = 0)
	{
		TreeNode* p_TreeNode = (TreeNode*)malloc(sizeof(TreeNode));
		p_TreeNode->sub_weight = tmp_ele.weight;
		p_TreeNode->ele = tmp_ele;
		p_TreeNode->lchild = lchild;
		p_TreeNode->rchild = rchild;
		p_TreeNode->height = height;
		return p_TreeNode;
	}

	inline int getHeight(TreeNode* node)
	{
		return (node == NULL) ? -1 : node->height;
	}

	TreeNode* RR_Rotate(TreeNode* k2)
	{
		TreeNode* k1 = k2->lchild;
		k2->lchild = k1->rchild;
		k1->rchild = k2;
		k2->height = max(getHeight(k2->lchild), getHeight(k2->rchild)) + 1;
		k1->height = max(getHeight(k1->lchild), k2->height) + 1; 
		update_weight(k2);
		update_weight(k1);
		return k1;
	}

	/* LL(Y rotates to the left):
			k2                       k1
		   /  \                     /  \
		  X    k1         ==>      k2   Z
			  /  \                /  \
			 Y    Z              X    Y
	 */
	TreeNode* LL_Rotate(TreeNode* k2)
	{
		TreeNode* k1 = k2->rchild;
		k2->rchild = k1->lchild;
		k1->lchild = k2;
		k2->height = max(getHeight(k2->lchild), getHeight(k2->rchild)) + 1;
		k1->height = max(getHeight(k1->rchild), k2->height) + 1;
		update_weight(k2);
		update_weight(k1);

		return k1;
	}



	/* LR(B rotates to the left, then C rotates to the right):
		  k3                         k3                       k2
		 /  \                       /  \                     /  \
		k1   D                     k2   D                   k1   k3
	   /  \         ==>           /  \        ==>          / \   / \
	  A    k2                    k1   C                   A  B  C   D
		  /  \                  /  \
		 B    C                A    B
	*/
	/*
	 Return which the root pointer should point to
	 */
	TreeNode* LR_Rotate(TreeNode* k3)
	{
		k3->lchild = LL_Rotate(k3->lchild);
		return RR_Rotate(k3);
	}


	/* RL(D rotates to the right, then C rotates to the left):
		   k3                         k3                          k2
		  /  \                       /  \                        /  \
		 A    k1                    A    k2                     k3   k1
			 /  \       ==>             /  \         ==>       /  \  / \
			k2   B                     C    k1                A   C D   B
		   /  \                            /  \
		  C    D                          D    B
	 */
	TreeNode* RL_Rotate(TreeNode* k3)
	{
		k3->rchild = RR_Rotate(k3->rchild);
		return LL_Rotate(k3);
	}

	/* return which the root pointer(at an outer/higher level) should point to,
	   the root_node of TreeNode tree may change frequently during delete/insert,
	   so the Root pointer should point to the REAL root node.
	 */
	TreeNode* insertNode(TreeNode* root, Element tmp_ele)
	{
		if (root == NULL)
			return (root = New_Node(tmp_ele, NULL, NULL));
		else if (tmp_ele.key < root->ele.key)
			root->lchild = insertNode(root->lchild, tmp_ele);
		else //key >= root->key
			root->rchild = insertNode(root->rchild, tmp_ele);

		root->height = max(getHeight(root->lchild), getHeight(root->rchild)) + 1;
		if (getHeight(root->lchild) - getHeight(root->rchild) == 2)
		{
			if (tmp_ele.key < root->lchild->ele.key)
				root = RR_Rotate(root);
			else
				root = LR_Rotate(root);
		}
		else if (getHeight(root->rchild) - getHeight(root->lchild) == 2)
		{
			if (tmp_ele.key < root->rchild->ele.key)
				root = RL_Rotate(root);
			else
				root = LL_Rotate(root);
		}
		update_weight(root);
		return root;
	}


	/* return which the root pointer(at an outer/higher level) should pointer to,
	   cause the root_node of TreeNode tree may change frequently during delete/insert,
	   so the Root pointer should point to the REAL root node.
	 */
	TreeNode* deleteNode(TreeNode* root, int key)
	{
		if (!root)
			return NULL;
		if (key == root->ele.key)
		{
			if (root->ele.key == exact_delete_key) {
				exact_delete_key = -1;
				tot_weight -= root->ele.weight;
			}
			if (root->rchild == NULL)
			{
				TreeNode* temp = root;
				root = root->lchild;
				delete(temp);
				return root;
			}
			else
			{
				TreeNode* temp = root->rchild;
				while (temp->lchild)
					temp = temp->lchild;
				/* replace the value */
				root->ele = temp->ele;
				/* Delete the node (successor node) that should be really deleted */
				root->rchild = deleteNode(root->rchild, temp->ele.key);
			}
		}
		else if (key < root->ele.key)
			root->lchild = deleteNode(root->lchild, key);
		else
			root->rchild = deleteNode(root->rchild, key);

		root->height = max(getHeight(root->lchild), getHeight(root->rchild)) + 1;
		if (getHeight(root->rchild) - getHeight(root->lchild) == 2)
		{
			if (getHeight(root->rchild->rchild) >= getHeight(root->rchild->lchild))
				root = LL_Rotate(root);
			else
				root = RL_Rotate(root);
		}
		else if (getHeight(root->lchild) - getHeight(root->rchild) == 2)
		{
			if (getHeight(root->lchild->lchild) >= getHeight(root->lchild->rchild))
				root = RR_Rotate(root);
			else
				root = LR_Rotate(root);
		}
		update_weight(root);
		return root;
	}

	void InOrder(TreeNode* root)
	{
		if (root)
		{
			InOrder(root->lchild);
			printf("key: %d height: %d ", root->ele.key, root->height);
			if (root->lchild)
				printf("left child: %d ", root->lchild->ele.key);
			if (root->rchild)
				printf("right child: %d ", root->rchild->ele.key);
			printf("\n");
			InOrder(root->rchild);
		}
	}

    int traverse_weight(TreeNode* cur_node,double weight) {
		//cerr <<cur_node->ele.key<<" " << weight <<" "<<node_weight(cur_node->lchild)<<" "<<cur_node->ele.weight << " " << node_weight(cur_node->rchild) << "\n";
        if (weight <= node_weight(cur_node->lchild) ) {
            return traverse_weight(cur_node->lchild, weight);
        }
        else{
            weight -= node_weight(cur_node->lchild);
            if (weight <= cur_node->ele.weight) {
                return cur_node->ele.value;
            }
            else {
                weight -= cur_node->ele.weight;
                return traverse_weight(cur_node->rchild, weight);
            }
        }
    }

    int random_sample_value() {
        TreeNode* cur_node = root;
        uniform_real_distribution<double> dr(0, tot_weight);
        double random_weight = dr(gen);
		//cerr << random_weight <<" "<<tot_weight << "\n";
        return traverse_weight(root, random_weight);
    }
    void delete_element(int del_key) {
		exact_delete_key = del_key;
		//cerr << tot_weight << "\n";
		root = deleteNode(root, del_key);
		//cerr << tot_weight << "\n";
	}
    void insert_element(Element& ins_ele) {
		tot_weight += ins_ele.weight;
        root = insertNode(root, ins_ele);
    }
    //create the prob and alias table
   
private:

    random_device rd;
    mt19937 gen;
    uniform_int_distribution<int> pos_rand;
    uniform_real_distribution<double> dr;
};
