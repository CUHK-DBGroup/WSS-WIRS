#ifndef DYNAMICSETSAMPLING_BSTSAMPLING_HPP
#define DYNAMICSETSAMPLING_BSTSAMPLING_HPP
#include "utility.h"
#include "basic_alias.hpp"
#include<cstdio>
#include<iostream>
#include <vector>
#include <random>

class BSTSampling {
    struct node{
        node *leftSon, *rightSon;
        int height;
        Element element;
        double weight;
        int L,R;
    };
public:

    void insert(Element element){
        root = insertElement(root,element);
    }

    void erase(int key){
        root = eraseElement(root, key);
    }

    vector<int> ask(int l,int r,int k){
        sampleBuf.clear();
        askInterval(root,l,r);
        std::vector<Element> sampleElements;
        float totWeight = 0;
        for(int id = 0 ;id<sampleBuf.size();id++)
        {
            node *nowNode = sampleBuf[id];
            totWeight = nowNode->weight;
            sampleElements.push_back(Element(1,id,nowNode->weight));
        }
        aliasMethod alias(sampleElements.size(),sampleElements);

        vector<int> ans;
        for(int i=0;i<k;i++)
        {
//            uniform_real_distribution<float> idD(0,totWeight);
//            float s = idD(gen);
//            int id = 0;
//            for(int j=0;j< sampleElements.size();j++)
//                if(sampleElements[j].weight>=s){
//                    id = j;
//                    break;
//                }
//                else s-=sampleElements[j].weight;

            int id = alias.random_sample_value();
//            std::uniform_real_distribution<double> db(0,sampleBuf[id]->weight);
//            double sampleWeight = db(gen);
//            ans.push_back(sample(sampleBuf[id],sampleWeight));
            ans.push_back(sample(sampleBuf[id]));
        }
        return ans;
    }

    void init(Element *element,int num)
    {
//        root = buildTree(0,num-1,element);
        root = nullptr;
        for(int i = 0;i<num;i++)
            root = insertElement(root,element[i]);
    }

    void clear(){
        clear(root);
    }

private:
    node *root;
    std::vector<node*> sampleBuf;
    std::random_device rd;
    std::mt19937 gen;

    void clear(node *nowNode){
        if(nowNode->leftSon!= nullptr) clear(nowNode->leftSon);
        if(nowNode->rightSon!= nullptr) clear(nowNode->rightSon);
        delete(nowNode);
    }
    void update(node *nowNode)
    {
        node *ls = nowNode->leftSon,*rs = nowNode->rightSon;
        nowNode->weight = ls->weight+rs->weight;
        nowNode->height = std::max(ls->height,rs->height)+1;
        nowNode->L = ls->L;
        nowNode->R = rs->R;
    }
    node *newNode(node *ls, node *rs)
    {
        node *nowNode = new node();
        nowNode->leftSon = ls;
        nowNode->rightSon = rs;
        update(nowNode);
        return nowNode;
    }
    node *newLeaf(Element element){
        node *nowNode = new node();
        nowNode->leftSon = nowNode->rightSon = nullptr;
        nowNode->height = 0;
        nowNode->element = element;
        nowNode->L = nowNode->R = element.key;
        nowNode->weight = element.weight;
        return nowNode;
    }
    node *RRRotate(node *k2)
    {
        node *k1 = k2->leftSon;
        if(k1->leftSon== nullptr||k1->rightSon== nullptr||k2->leftSon== nullptr||k2->rightSon== nullptr)
            puts("OK");
        k2->leftSon = k1->rightSon;
        k1->rightSon = k2;
        update(k2);
        update(k1);
        return k1;
    }
    node *LLRotate(node *k2){
        node *k1 = k2->rightSon;
        k2->rightSon = k1->leftSon;
        k1->leftSon = k2;
        update(k2);
        update(k1);
        return k1;
    }
    node *LRRotate(node *k3){
        k3->leftSon= LLRotate(k3->leftSon);
        return RRRotate(k3);
    }
    node *RLRotate(node *k3){
        k3->rightSon = RRRotate(k3->rightSon);
        return LLRotate(k3);
    }
    node *insertElement(node *nowNode,Element element){
        if(nowNode == nullptr)
            return newLeaf(element);
        if(nowNode->leftSon == nullptr){
            node *newL = newLeaf(element);
            if(nowNode->R<=element.key)
                return newNode(nowNode,newL);
            else return newNode(newL,nowNode);
        }
        if(element.key<nowNode->rightSon->L)
            nowNode->leftSon = insertElement(nowNode->leftSon,element);
        else nowNode->rightSon = insertElement(nowNode->rightSon,element);

        update(nowNode);

        node *ls = nowNode->leftSon,*rs = nowNode->rightSon;
        if(ls->height-rs->height==2)
        {
            if(element.key<ls->rightSon->L)
                nowNode = RRRotate(nowNode);
            else nowNode = LRRotate(nowNode);
        }
        else if(rs->height-ls->height==2)
        {
            if(element.key<rs->rightSon->L)
                nowNode = RLRotate(nowNode);
            else nowNode = LLRotate(nowNode);
        }
        update(nowNode);
        return nowNode;
    }

    node *eraseElement(node *nowNode,int key){
        if(root == nullptr) return nullptr;
        node *ls = nowNode->leftSon, *rs = nowNode->rightSon;
        if(ls->leftSon== nullptr &&ls->L == key){
            delete(nowNode);
            delete(ls);
            return rs;
        }
        if(rs->leftSon == nullptr && rs->L == key)
        {
            delete(nowNode);
            delete(rs);
            return ls;
        }
        if(key<nowNode->rightSon->L)
            nowNode->leftSon = eraseElement(nowNode->leftSon,key);
        else nowNode->rightSon = eraseElement(nowNode->rightSon,key);
        ls = nowNode->leftSon;
        rs = nowNode->rightSon;
        update(nowNode);
        if(rs->height-ls->height == 2)
        {
            if(rs->rightSon->height >=rs->leftSon->height)
                nowNode = LLRotate(nowNode);
            else nowNode = RLRotate(nowNode);
        }
        else if(ls->height - rs->height == 2){
            if(ls->leftSon->height>=ls->rightSon->height)
                nowNode = RRRotate(nowNode);
            else nowNode = LRRotate(nowNode);
        }
        update(nowNode);
        return nowNode;
    }

    void askInterval(node *nowNode,int l,int r)
    {
        if(l<=nowNode->L&& nowNode->R<=r)
        {
            sampleBuf.push_back(nowNode);
            return;
        }
        if(l<=nowNode->leftSon->R&&r>=nowNode->leftSon->L) askInterval(nowNode->leftSon,l,r);
        if(nowNode->rightSon->L<=r&&l<=nowNode->rightSon->R) askInterval(nowNode->rightSon,l,r);
    }

    int sample(node *nowNode , double w){
        if(nowNode->leftSon == nullptr) {
            if(nowNode->height!=0)puts("NO");
            return nowNode->element.value;
        }
        if(w<=nowNode->leftSon->weight)
            return sample(nowNode->leftSon,w);
        else return sample(nowNode->rightSon,w-nowNode->leftSon->weight);
    }

    int sample(node *nowNode){
        if(nowNode->leftSon == nullptr) return nowNode->element.value;
        std::uniform_real_distribution<double> db(0,nowNode->weight);
        double w = db(gen);
        if(w<=nowNode->leftSon->weight)
            return sample(nowNode->leftSon,w);
        else return sample(nowNode->rightSon,w-nowNode->leftSon->weight);
    }

    node *buildTree(int l,int r,Element *element)
    {
        if(l==r) return newLeaf(element[l]);
        int mid = (l+r)/2;
        node *ls = buildTree(l,mid,element);
        node *rs = buildTree(mid+1,r,element);
        return newNode(ls,rs);
    }
};


#endif //DYNAMICSETSAMPLING_BSTSAMPLING_HPP
