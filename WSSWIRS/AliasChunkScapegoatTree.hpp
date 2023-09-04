#include<cstdio>
#include<iostream>
#include<vector>
#include<bitset>
#include<algorithm>
#include "basic_alias.hpp"
#ifndef DYNAMICSETSAMPLING_ALIASCHUNKSCAPEGOATTREE_HPP
#define DYNAMICSETSAMPLING_ALIASCHUNKSCAPEGOATTREE_HPP

class AliasChunkScapegoatTree {
    struct chunk{
        std::vector<Element> elements;
        int size;
        double totWeight;
        float U;
        int L,R;

        chunk(std::vector<Element> &_elements){
            elements = _elements;
            size = elements.size();
            L = R = elements[0].key;
            totWeight = 0;
            for(auto e:elements)
            {
                totWeight +=e.weight;
                L = min(L,e.key);
                R = max(R,e.key);
            }
            sort(elements.begin(),elements.end(), cmp_element_key);
        }

        chunk(std::vector<Element> &_elements,float _U){
            elements = _elements;
            size = elements.size();
            U = _U;
            L = R = elements[0].key;
            totWeight = 0;
            for(auto e:elements)
            {
                totWeight +=e.weight;
                L = min(L,e.key);
                R = max(R,e.key);
            }
            sort(elements.begin(),elements.end(), cmp_element_key);
        }

        ~chunk(){
            elements.clear();
        }

        int randomSampleValue(mt19937 &rng){
//
            uniform_real_distribution<float> db(0,totWeight);
            float sampl = db(rng);
            for(int i=0;i<size;i++)
                if(elements[i].weight>=sampl)return elements[i].value;
                else sampl-=elements[i].weight;
            return elements[size-1].value;
        }

        void getElements(int l,int r,vector<Element> &retElements){
            for(const Element& element:elements)
                if(l<=element.key&&element.key<=r)
                    retElements.push_back(element);
        }
    };

    struct node{
        aliasMethod *method;
        int L,R,size;
        vector<Element> elements;
        node *leftSon,*rightSon;
        ~node(){
            elements.clear();
            delete(method);
        }
    };
public:
    void setChunkSize(int _cSize){
        chunkSize = _cSize;
    }
    void init(Element *dataSet,int num){
        root = buildTree(dataSet,num);
    }
    void init(Element *dataSet,int num,float u){
        U=u;
        root = buildTree(dataSet,num);
    }

    vector<int> ask(int l,int r,int k){
        vector<aliasMethod *> sampleMethod;
        getInterval(root,l,r,sampleMethod);
        vector<Element> sampleElements;
        for(int id = 0;id<sampleMethod.size();id++) {
            auto method = sampleMethod[id];
            sampleElements.push_back(Element(-1, id,method->tot_weight));
        }
        int methodNum= sampleElements.size();

        chunk *chunk1 = getChunk(root, l);
        chunk *chunk2 = getChunk(root, r);
        if(chunk1==chunk2&&chunk1!= nullptr) chunk1->getElements(l,r,sampleElements);
        else {
            if (chunk1 != nullptr) chunk1->getElements(l, r, sampleElements);
            if (chunk2 != nullptr) chunk2->getElements(l, r, sampleElements);
        }
        for(int i=methodNum;i<sampleElements.size();i++)
            sampleElements[i].value += methodNum;
        aliasMethod alias(sampleElements.size(),sampleElements);

        vector<int>ans;

        for(int i = 0;i<k;i++)
        {
            int sampleRes = alias.random_sample_value();
            if(sampleRes>=methodNum) ans.push_back(sampleRes-methodNum);
            else{
                int chunkId = sampleMethod[sampleRes]->random_sample_value();
                auto chunkk = chunks[chunkId];
//                if(i==27070) {
//                    cout<<chunkId<<' '<<chunkk->randomSampleValue(rng)<<endl;
//                }
                int x = chunkk->randomSampleValue(rng);
                ans.push_back(x);
            }
        }

        return ans;
    }

    void printTree()
    {
        printTreeInfo(root);
    }
private:

    node *root = nullptr;
    std::vector<node *> buildBuf;
    std::vector<node *> sta;
    std::vector<chunk *> chunks;

    mt19937 rng;
    const double alpha =0.7;
    int chunkSize;
    float U = -1;

    Element chunkToElement(chunk * ch,int id){
        return Element(ch->L,id,ch->totWeight);
    }


    void merge(vector<Element> &FElements,vector<Element> const & LElements,vector<Element> RElements){
        int LPoint = 0,RPoint = 0;
        int LSize = LElements.size(),RSize = RElements.size();
        while(LPoint<LSize&&RPoint<RSize)
        {
            if(LElements[LPoint].weight>RElements[RPoint].weight)
                FElements.push_back(LElements[LPoint++]);
            else FElements.push_back((RElements[RPoint++]));
        }
        while(LPoint<LSize) FElements.push_back(LElements[LPoint++]);
        while(RPoint<RSize) FElements.push_back(RElements[RPoint++]);
    }

    void clear(node *node){
        if(node->leftSon == nullptr) {
            chunk *nowChunk = chunks[node->elements[0].value];
            if (nowChunk->size !=0 ) {
                buildBuf.push_back(node);
            }
            else{
                delete(node);
                delete(nowChunk);
            }
            return;
        }
        clear(node->leftSon);
        clear(node->rightSon);
        delete(node);
    }


    void update(node *nowNode){
        node *ls = nowNode->leftSon;
        node *rs = nowNode->rightSon;
        nowNode->size = ls->size + rs->size;
        vector<Element> tmp;
        if(ls->size==0)
            merge(nowNode->elements, tmp,rs->elements);
        else if(rs->size == 0) merge(nowNode->elements, ls->elements,tmp);
        else merge(nowNode->elements,ls->elements,rs->elements);
        nowNode->method = new aliasMethod(nowNode->size,nowNode->elements);
        nowNode->L = ls->L;
        nowNode->R = rs->R;
    }

    node *newNode(node *ls,node *rs){
        node *nowNode = new node();
        nowNode->leftSon = ls;
        nowNode->rightSon = rs;
        update(nowNode);
        return nowNode;
    }
    node *newLeaf(chunk *nowChunk,int id){
        Element nowData(nowChunk->L,id,nowChunk->totWeight);
        node *nowNode = new node();
        nowNode->L = nowChunk->L;
        nowNode->R = nowChunk->R;
        nowNode->elements.push_back(nowData);
        nowNode->method = new aliasMethod(1,nowNode->elements);
        nowNode->size = 1;
        return nowNode;
    }
    node *build(int l,int r){
        if(l==r) return buildBuf[l];
        int mid = ((long long)l+(long long)r)/2;
        node *ls = build(l,mid);
        node *rs = build(mid+1,r);
        node *nd = newNode(ls,rs);
        if(ls->leftSon != nullptr) vector<Element> ().swap(ls->elements);
        if(rs->rightSon != nullptr) vector<Element> ().swap(rs->elements);
        return nd;
    }
    node *buildTree(Element *dataSet,int num){
        buildBuf.clear();
        int size = 0;
        int index = 0;
        for(int id = 0;id<num;id+=chunkSize) {
            size++;
            vector<Element> elements;
            for(int j = id;j<min(id+chunkSize,num);j++)
                elements.push_back(dataSet[j]);
            chunk * nowChunk;
            if(U==-1) nowChunk = new chunk(elements);
            else nowChunk = new chunk(elements,U);
            node *nd = newLeaf(nowChunk,index++);
            buildBuf.push_back(nd);
            chunks.push_back(nowChunk);
        }
        return build(0,size-1);
        buildBuf.clear();
    }

    void getInterval(node *nowNode,int l,int r,vector<aliasMethod*> &ans){
        if(l<nowNode->L && nowNode->R<r) {
            ans.push_back(nowNode->method);
            return;
        }
        if(nowNode->leftSon == nullptr) return;
        if(l<nowNode->leftSon->R&&r>nowNode->leftSon->L) getInterval(nowNode->leftSon,l,r,ans);
        if(nowNode->rightSon->L<r&&l<nowNode->rightSon->R) getInterval(nowNode->rightSon,l,r,ans);
    }

    chunk *getChunk(node *nowNode, int position){
        if(nowNode->leftSon == nullptr)
        {
            return chunks[nowNode->elements[0].value];
        }
        if(nowNode->leftSon->L<=position&&position<=nowNode->leftSon->R) return getChunk(nowNode->leftSon, position);
        if(nowNode->rightSon->L<=position&&position<=nowNode->rightSon->R) return getChunk(nowNode->rightSon, position);
        return nullptr;
    }

    void printTreeInfo(node *nowNode){
        if(nowNode->leftSon == nullptr)
        {
            printf("%d %d %.2f\n",nowNode->elements[0].key,nowNode->elements[0].value,nowNode->elements[0].weight);
            return;
        }
        printTreeInfo(nowNode->leftSon);
        printTreeInfo(nowNode->rightSon);
    }


};


#endif //DYNAMICSETSAMPLING_CHUNKSCAPEGOATTREE_HPP
