#ifndef DYNAMICSETSAMPLING_SCAPEGOATTREE_HPP
#define DYNAMICSETSAMPLING_SCAPEGOATTREE_HPP
#include<cstdio>
#include<iostream>
#include<vector>
#include "basic_alias.hpp"
//#include "utility.h"

template<typename Method>
class ScapegoatTree {
public:
    void init(Element* dataSet, int num) {
        treeSize = num;
        root = buildTree(dataSet, num);
    }
    void init(Element* dataSet, int num, float u) {
        treeSize = num;
        U = u;
        root = buildTree(dataSet, num);
    }
    void insert(Element newData) {
        treeSize++;
        node* newLeafNode = newLeaf(newData);
        vector<Element> tmp;
        if (root == nullptr) {
            root = newLeafNode;
            return;
        }
        if (root->leftSon == nullptr)
        {
            if (root->R <= newData.key)
                root = newNode(root, newLeafNode);
            else
                root = newNode(newLeafNode, root);
            return;
        }
        sta.clear();
        node* nowNode = root, * father = nullptr;
        while (nowNode->leftSon != nullptr)
        {
            father = nowNode;
            nowNode->size += 1;
            nowNode->nodeNum += 1;
            nowNode->method->insert_element(newData);
            nowNode->L = min(nowNode->L, newData.key);
            nowNode->R = max(nowNode->R, newData.key);
            sta.push_back(nowNode);
            if (newData.key < nowNode->rightSon->L)
                nowNode = nowNode->leftSon;
            else nowNode = nowNode->rightSon;
        }
        //        if(nowNode->size==0){
        //            if(father->leftSon == nowNode)
        //                father->leftSon = newLeafNode;
        //            else father->rightSon = newLeafNode;
        //            delete(nowNode);
        //            for(auto x:sta){
        //                x->nodeNum --;
        //            }
        //        }else {
        if (father->leftSon == nowNode) {
            if (nowNode->R <= newData.key)
                father->leftSon = newNode(nowNode, newLeafNode);
            else father->leftSon = newNode(newLeafNode, nowNode);
        }
        else {
            if (nowNode->R <= newData.key)
                father->rightSon = newNode(nowNode, newLeafNode);
            else father->rightSon = newNode(newLeafNode, nowNode);
        }
        //        }
        balance();
    }
    void erase(int position) {
        if (root->leftSon == nullptr)
        {
            delete(root);
            root = nullptr;
            return;
        }
        node* nowNode = root;
        while (nowNode->leftSon != nullptr)
        {
            nowNode->size -= 1;
            nowNode->method->delete_element(position);
            if (position < nowNode->rightSon->L)
                nowNode = nowNode->leftSon;
            else nowNode = nowNode->rightSon;
        }
        nowNode->size -= 1;
        nowNode->method->delete_element(position);
        if (root->nodeNum * alpha > root->size) {
            root = rebuildSubtree(root);
        }
    }
    vector<int> ask(int l, int r, int k) {
        vector<Method*> sampleMethod;
        getInterval(root, l, r, sampleMethod);
        vector<Element> sampleElements;
        for (int id = 0; id < sampleMethod.size(); id++) {
            auto method = sampleMethod[id];
            sampleElements.push_back(Element(1, id, method->tot_weight));
        }

        aliasMethod alias(sampleMethod.size(), sampleElements);

        vector<int>ans;
        int num = sampleMethod.size();
        vector<int>sampleNum;
        sampleNum.resize(num, 0);
        for (int i = 0; i < k; i++)
        {
            int methodId = alias.random_sample_value();
            //            sampleNum[methodId]++;
            ans.push_back(sampleMethod[methodId]->random_sample_value());
        }

        //        for(int i=0;i<num;i++)
        //            for(int j=0;j<sampleNum[i];j++)
        //                ans.push_back(sampleMethod[i]->random_sample_value());

        return ans;
    }

    void printTree()
    {
        printTreeInfo(root);
    }
private:
    struct node {
        Method* method;
        int L, R, size, nodeNum;
        vector<Element> elements;
        node* leftSon, * rightSon;
        ~node() {
            elements.clear();
            delete(method);
        }
    };

    node* root = nullptr;
    std::vector<node*> buildBuf;
    std::vector<node*> sta;
    int treeSize = 0;

    XoshiroCpp::Xoroshiro128Plus rng;
    const double alpha = 0.7;
    float U;

    void merge(vector<Element>& FElements, vector<Element> const& LElements, vector<Element> const& RElements) {
        int LPoint = 0, RPoint = 0;
        int LSize = LElements.size(), RSize = RElements.size();
        while (LPoint < LSize && RPoint < RSize)
        {
            if (LElements[LPoint].weight > RElements[RPoint].weight)
                FElements.push_back(LElements[LPoint++]);
            else FElements.push_back((RElements[RPoint++]));
        }
        while (LPoint < LSize) FElements.push_back(LElements[LPoint++]);
        while (RPoint < RSize) FElements.push_back(RElements[RPoint++]);
    }

    void clear(node* node) {
        if (node->leftSon == nullptr) {
            if (node->size != 0)
                buildBuf.push_back(node);
            else {
                treeSize--;
                delete(node);
            }
            return;
        }
        clear(node->leftSon);
        clear(node->rightSon);
        delete(node);
    }


    void update(node* nowNode) {
        node* ls = nowNode->leftSon;
        node* rs = nowNode->rightSon;
        nowNode->size = ls->size + rs->size;
        nowNode->nodeNum = ls->nodeNum + rs->nodeNum;
        vector<Element> tmp;
        if (ls->size == 0)
            merge(nowNode->elements, tmp, rs->elements);
        else if (rs->size == 0) merge(nowNode->elements, ls->elements, tmp);
        else merge(nowNode->elements, ls->elements, rs->elements);
        if (nowNode->method->method_name() == "size_block")
            nowNode->method = new Method(nowNode->size, nowNode->elements);
        else nowNode->method = new Method(nowNode->size, U, nowNode->elements);
        nowNode->L = ls->L;
        nowNode->R = rs->R;
        if (ls->leftSon != nullptr) ls->elements.clear();
        if (rs->rightSon != nullptr) rs->elements.clear();
    }

    node* newNode(node* ls, node* rs) {
        node* nowNode = new node();
        nowNode->leftSon = ls;
        nowNode->rightSon = rs;
        update(nowNode);
        return nowNode;
    }
    node* newLeaf(Element nowData) {
        node* nowNode = new node();
        nowNode->L = nowNode->R = nowData.key;
        nowNode->elements.push_back(nowData);
        if (nowNode->method->method_name() == "size_block")
            nowNode->method = new Method(1, nowNode->elements);
        else nowNode->method = new Method(1, U, nowNode->elements);
        nowNode->size = 1;
        nowNode->nodeNum = 1;
        return nowNode;
    }
    node* build(int l, int r) {
        if (l == r) return buildBuf[l];
        int mid = (l + r) / 2;
        node* ls = build(l, mid);
        node* rs = build(mid + 1, r);
        node* nd = newNode(ls, rs);
        if (ls->leftSon != nullptr) ls->elements.clear();
        if (rs->rightSon != nullptr) rs->elements.clear();
        return nd;
    }
    node* buildTree(Element* dataSet, int num) {
        buildBuf.clear();
        for (int id = 0; id < num; id++) {
            node* nd = newLeaf(dataSet[id]);
            buildBuf.push_back(nd);
        }
        return build(0, num - 1);
        buildBuf.clear();
    }
    node* rebuildSubtree(node* node) {
        buildBuf.clear();
        int num = node->size;
        clear(node);
        return build(0, num - 1);
        buildBuf.clear();
    }
    void balance() {
        node* pre = nullptr;
        int stSize = sta.size();
        for (int i = 0; i < stSize; i++)
        {
            node* pNode = sta[i];
            double nodeNum = pNode->nodeNum;
            int lNum = pNode->leftSon->nodeNum;
            int rNum = pNode->rightSon->nodeNum;
            if (nodeNum * alpha < max(lNum, rNum))
            {
                if (pNode == root)
                    root = rebuildSubtree(pNode);
                else {
                    if (pre->leftSon == pNode)
                        pre->leftSon = rebuildSubtree(pNode);
                    else pre->rightSon = rebuildSubtree(pNode);
                }
                break;
            }
            pre = pNode;
        }
        sta.clear();
    }

    void getInterval(node* nowNode, int l, int r, vector<Method*>& ans) {
        if (l <= nowNode->L && nowNode->R <= r) {
            ans.push_back(nowNode->method);
            return;
        }
        if (l <= nowNode->leftSon->R && r >= nowNode->leftSon->L) getInterval(nowNode->leftSon, l, r, ans);
        if (nowNode->rightSon->L <= r && l <= nowNode->rightSon->R) getInterval(nowNode->rightSon, l, r, ans);
    }

    void printTreeInfo(node* nowNode) {
        if (nowNode->leftSon == nullptr)
        {
            printf("%d %d %.2f\n", nowNode->elements[0].key, nowNode->elements[0].value, nowNode->elements[0].weight);
            return;
        }
        printTreeInfo(nowNode->leftSon);
        printTreeInfo(nowNode->rightSon);
    }

};


#endif //DYNAMICSETSAMPLING_SCAPEGOATTREE_HPP