#include <unordered_map>
#include <random>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include "ScapegoatTree.hpp"
#include "BSTSampling.hpp"
#include "utility.h"
#include "basic_alias.hpp"
#include "linear_search.hpp"
#include "basic_bst.hpp"
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <set>
#include "QuickBucket.hpp"
#include "QuickChunkScapegoatTree.hpp"
#include "AliasChunkScapegoatTree.hpp"
#include "ChunkScapegoatTreeWithAlias.hpp"
Element *element;
using namespace std;

string dataHome = "/home/username/DynamicWeightedSetSampling/dataset/";
string tmpData = "/home/username/DynamicWeightedSetSampling/tmp.data";
string resultHome = "/home/username/DynamicWeightedSetSampling/result";

int chunkAlias = 0;

int getNum(char *num){
    int x = 0;
    int len = strlen(num);
    for(int i=0;i<len;i++)
        x = x*10 + num[i]-'0';
    return x;
}

void genUniform(){
    freopen("/home/username/DynamicWeightedSetSampling/dataset/Uniform.data","w",stdout);
    int N = 100000000;
    int W = 10000000;
    XoshiroCpp::Xoroshiro128Plus rng;
//    exponential_distribution<double> dr(1);
    uniform_int_distribution<int> dr(1,W);
    for(int i = 1;i<=N;i++){
        printf("%d %d %d\n",i,i,dr(rng));
    }
}


void genExponential(){
    freopen("/home/username/DynamicWeightedSetSampling/dataset/Exponential.data","w",stdout);
    int N = 100000000;
    double lambda = 1.0/1000;
    XoshiroCpp::Xoroshiro128Plus rng;
    exponential_distribution<double> dr(lambda);
//    uniform_int_distribution<int> dr(1,W);
    for(int i = 1;i<=N;i++){
        printf("%d %d %.10f\n",i,i,dr(rng));
    }
}

void genExponentialExp(){
    freopen("/home/username/DynamicWeightedSetSampling/dataset/ExponentialExp.data","w",stdout);
    int N = 400000000;
    double lambda = 1.0/1000;
    XoshiroCpp::Xoroshiro128Plus rng;
    exponential_distribution<double> dr(lambda);
//    uniform_int_distribution<int> dr(1,W);
    for(int i = 1;i<=N;i++){
        printf("%d %d %.10f\n",i,i,dr(rng));
    }
}

void WSSProduceQuery(int n,int m,string filename,string tmpfile,int sampleTimes){
    ifstream inFile;
    inFile.open(filename.c_str());

    ofstream tmpf;
    tmpf.open(tmpfile.c_str());

    for(int i = 0;i < n;i++)
    {
        int key,value;
        float weight;
        inFile>>key>>value>>weight;
        element[i] = Element(key,value,weight);
    }

//    sort(element,element+n, cmp_element_weight);

    tmpf<<n<<'\n';
    for(int i = 0;i<n;i++)
    {
        int key=element[i].key,value = element[i].value;
        float weight = element[i].weight;
        tmpf << key << " " << value << " " << weight << "\n";
    }
    tmpf << m << '\n';
    for (int i = 0; i < m; i++)
        tmpf << "0 "<<sampleTimes<<"\n";
}

void WIRSProduceQuery(int n,int m,string filename,string tmpfile,int sampleTimes,int cov){
    ifstream inFile;
    inFile.open(filename.c_str());

    ofstream tmpf;
    tmpf.open(tmpfile.c_str());

    for(int i = 0;i < n;i++)
    {
        int key,value;
        float weight;
        inFile>>key>>value>>weight;
        element[i] = Element(key,value,weight);
    }

//    sort(element,element+n, cmp_element_weight);


    tmpf<<n<<'\n';
    for(int i = 0;i<n;i++)
    {
        int key=element[i].key,value = element[i].value;
        float weight = element[i].weight;
        tmpf << key << " " << value << " " << weight << "\n";
    }
    tmpf << m << '\n';
    int length = (long long)n *cov/100;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> l_random(1, n-1-length);
    for (int i = 0; i < m; i++) {
        int l = l_random(gen);
        int r = l+length-1;
        tmpf << "0 " << l<<' '<<r<<' '<<sampleTimes << "\n";
    }
}

void WSSProduceMix(int n,int m,string filename,string tmpfile,int sampleTimes,int percent){
    ifstream inFile;
    inFile.open(filename.c_str());

    ofstream tmpf;
    tmpf.open(tmpfile.c_str());

    for(int i = 0;i < n;i++)
    {
        int key,value;
        float weight;
        inFile>>key>>value>>weight;
        element[i] = Element(key,value,weight);
    }

    sort(element,element+n, cmp_element_weight);

    int insNum = m * percent / 200;
    int delNum = m * percent / 200;
//    insNum = 0;
    int queryNum = m - insNum - delNum;
//    queryNum = 1;
//    insNum = m - 1;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> key_random(1, n-1);

    set<int> selectedKey;

    set<int> insKey,delKey;
    for(int i=1;i<=insNum;i++)
    {
        int now = key_random(gen);
        while(selectedKey.find(now)!=selectedKey.end())
            now = key_random(gen);
        selectedKey.insert(now);
        insKey.insert(now);
    }
    for(int i=1;i<=delNum;i++)
    {
        int now = key_random(gen);
        while(selectedKey.find(now)!=selectedKey.end())
            now = key_random(gen);
        selectedKey.insert(now);
        delKey.insert(now);
    }

    tmpf<<n-insNum<<'\n';
    vector<Element> opt;

    for(int i = 0;i<queryNum;i++)
        opt.push_back(Element(-1,sampleTimes,0));
    for(int i = 0;i<n;i++)
    {
        int key=element[i].key,value = element[i].value;
        float weight = element[i].weight;
        if(insKey.find(key)!=insKey.end()){
            opt.push_back(Element(key,value,weight));
            continue;
        }
        if(delKey.find(key)!=delKey.end()){
            opt.push_back(Element(key,-1,weight));
        }
        tmpf << key << " " << value << " " << weight << "\n";
    }

    tmpf << m << '\n';
    shuffle(opt.begin(),opt.end(), gen);
    for (int i = 0; i < m; i++) {
        if(opt[i].key == -1){
            tmpf << "0 "<<sampleTimes<<"\n";
        }
        else if(opt[i].value == -1)
            tmpf << "2 " << opt[i].key << "\n";
        else
            tmpf << "1 " << opt[i].key << " " << opt[i].value << " " << opt[i].weight << '\n';
    }
}

void WIRSProduceMix(int n,int m,string filename,string tmpfile,int sampleTimes,int percent,int cov){
    ifstream inFile;
    inFile.open(filename.c_str());

    ofstream tmpf;
    tmpf.open(tmpfile.c_str());

    for(int i = 0;i < n;i++)
    {
        int key,value;
        float weight;
        inFile>>key>>value>>weight;
        element[i] = Element(key,value,weight);
    }

    sort(element,element+n, cmp_element_weight);

    int insNum = m * percent / 200;
    int delNum = m * percent / 200;
//    insNum = 0;
    int queryNum = m - insNum - delNum;
//    queryNum = 1;
//    insNum = m - 1;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> key_random(1, n-1);

    set<int> selectedKey;

    set<int> insKey,delKey;
    for(int i=1;i<=insNum;i++)
    {
        int now = key_random(gen);
        while(selectedKey.find(now)!=selectedKey.end())
            now = key_random(gen);
        selectedKey.insert(now);
        insKey.insert(now);
    }
    for(int i=1;i<=delNum;i++)
    {
        int now = key_random(gen);
        while(selectedKey.find(now)!=selectedKey.end())
            now = key_random(gen);
        selectedKey.insert(now);
        delKey.insert(now);
    }

    tmpf<<n-insNum<<'\n';
    vector<Element> opt;


    for(int i = 0;i<queryNum;i++)
        opt.push_back(Element(-1,sampleTimes,0));
    for(int i = 0;i<n;i++)
    {
        int key=element[i].key,value = element[i].value;
        float weight = element[i].weight;
        if(insKey.find(key)!=insKey.end()){
            opt.push_back(Element(key,value,weight));
            continue;
        }
        if(delKey.find(key)!=delKey.end()){
            opt.push_back(Element(key,-1,weight));
        }
        tmpf << key << " " << value << " " << weight << "\n";
    }

    tmpf << m << '\n';
    shuffle(opt.begin(),opt.end(), gen);
    int length = (long long)n *cov/100;
    uniform_int_distribution<int> l_random(1, n-1-length);
    for (int i = 0; i < m; i++) {
        if(opt[i].key == -1){
            int l = l_random(gen);
            int r = l+length-1;
            tmpf << "0 " << l<<' '<<r<<' '<<sampleTimes << "\n";
        }
        else if(opt[i].value == -1)
            tmpf << "2 " << opt[i].key << "\n";
        else
            tmpf << "1 " << opt[i].key << " " << opt[i].value << " " << opt[i].weight << '\n';
    }
}

void produceIns(int n,int m,string filename,string tmpfile){
    ifstream inFile;
    inFile.open(filename.c_str());

    ofstream tmpf;
    tmpf.open(tmpfile.c_str());

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> key_random(1, n-1);

    set<int> selected_key;
    for(int i=1;i<=m;i++)
    {
        int now = key_random(gen);
        while(selected_key.find(now)!=selected_key.end())
            now = key_random(gen);
        selected_key.insert(now);
    }

    tmpf<<n-m<<'\n';
    double mx = 0;
    vector<Element> opt;
    for(int i = 0;i<n;i++)
    {
        int key,value;
        float weight;
        inFile>>key>>value>>weight;
        if(selected_key.find(key)!=selected_key.end()){
            opt.push_back(Element(key,value,weight));
            continue;
        }
        tmpf << key << " " << value << " " << weight << "\n";
    }
    cout<<mx<<endl;
    tmpf << m << '\n';
    shuffle(opt.begin(),opt.end(), gen);
    for (int i = 0; i < m; i++) {
        tmpf << "1 " << opt[i].key << " " << opt[i].value << " " << opt[i].weight << '\n';
    }
}

void produceDel(int n,int m,string filename,string tmpfile){
    ifstream inFile;
    inFile.open(filename.c_str());

    ofstream tmpf;
    tmpf.open(tmpfile.c_str());

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> key_random(1, n-1);

    set<int> selected_key;
    for(int i=1;i<=m;i++)
    {
        int now = key_random(gen);
        while(selected_key.find(now)!=selected_key.end())
            now = key_random(gen);
        selected_key.insert(now);
    }
    tmpf<<n<<'\n';
    double mx = 0;
    vector<Element> opt;
    for(int i = 0;i<n;i++)
    {
        int key,value;
        float weight;
        inFile>>key>>value>>weight;
        if(selected_key.find(key)!=selected_key.end()){
            opt.push_back(Element(key,value,weight));
        }
        tmpf << key << " " << value << " " << weight << "\n";
    }
    cout<<mx<<endl;
    tmpf << m << '\n';
    shuffle(opt.begin(),opt.end(), gen);
    for (int i = 0; i < m; i++)
        tmpf << "2 " << opt[i].key << "\n";
}


void WSSBstTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream, bool frequency) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;


//    sort(ele_vec, ele_vec+n, cmp_element_weight);
    vector<Element> ele(ele_vec,ele_vec+n);
    cout<<ele_vec[0].key<<endl;
    start = chrono::system_clock::now();
    bstMethod bst = bstMethod(n,ele);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    int tmp=0;
    if(!frequency) resultStream << "basic bst tree build_time:" << elapsed_seconds.count() << "\n";
    start = chrono::system_clock::now();
    if(!frequency) {
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                for(int j=1;j<=i.value;j++)
                    tmp+=bst.random_sample_value();
            }
            if (i.opt_type == 1) {
                Element tmp_ele = Element(i.key, i.value, i.weight);
                bst.insert_element(tmp_ele);
            }
            if (i.opt_type == 2) {
                bst.delete_element(i.key);
            }
        }
    }else{
        chrono::high_resolution_clock::time_point t1,t2;
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                t1 = chrono::high_resolution_clock::now();
                for(int j=1;j<=i.value;j++)
                    tmp+=bst.random_sample_value();
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 1) {
                t1 = chrono::high_resolution_clock::now();
                Element tmp_ele = Element(i.key, i.value, i.weight);
                bst.insert_element(tmp_ele);
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 2) {
                t1 = chrono::high_resolution_clock::now();
                bst.delete_element(i.key);
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
        }
    }
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    if(frequency) return;
    resultStream << "basic bst Operation_time:" << elapsed_seconds.count() << "\n";
    resultStream << "basic average bst Operation_time:" << elapsed_seconds.count()/opt_vec.size()*1e9<< "\n";
    struct rusage rUsage;
    getrusage(RUSAGE_SELF, &rUsage);
    long ms = rUsage.ru_maxrss;
    float gms = ms / 1024 ;
    resultStream <<"bst_memory " << gms << "mb\n";
    resultStream<<"-----------------------------------"<<endl;
}

void sortTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream, bool frequency) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;


    start = chrono::system_clock::now();
    sort(ele_vec, ele_vec+n, cmp_element_key);

    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    int tmp=0;
    if(!frequency) resultStream << "sort_time:" << elapsed_seconds.count() << "\n";

}

void WSSBucketTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream, bool frequency) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;


//    sort(ele_vec, ele_vec+n, cmp_element_weight);
    vector<Element> ele(ele_vec,ele_vec+n);
    start = chrono::system_clock::now();
    BucketMethod bucket(n,ele);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "basic bucket build_time:" << elapsed_seconds.count() << "\n";
    int tmp = 0;
    start = chrono::system_clock::now();
    if(!frequency) {
        for (auto i: opt_vec) {
//            cout<<i.opt_type<<' '<<i.key<<' '<<i.value<<' '<<i.weight<<endl;
            if (i.opt_type == 0) {
                for(int j=1;j<=i.value;j++)
                    tmp+=bucket.random_sample_value();
            }
            if (i.opt_type == 1) {
                Element tmp_ele = Element(i.key, i.value, i.weight);
                bucket.insert_element(tmp_ele);
            }
            if (i.opt_type == 2) {
                bucket.delete_element(i.key);
            }
        }
    }else{
        chrono::high_resolution_clock::time_point t1,t2;
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                t1 = chrono::high_resolution_clock::now();
                for(int j=1;j<=i.value;j++)
                    tmp+=bucket.random_sample_value();
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 1) {
                t1 = chrono::high_resolution_clock::now();
                Element tmp_ele = Element(i.key, i.value, i.weight);
                bucket.insert_element(tmp_ele);
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 2) {
                t1 = chrono::high_resolution_clock::now();
                bucket.delete_element(i.key);
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
        }
    }
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    if(frequency) return;
    resultStream << "basic bucket Operation_time:" << elapsed_seconds.count() << "\n";
    resultStream << "basic average bucket Operation_time:" << elapsed_seconds.count()/opt_vec.size()*1e9<< "\n";
    struct rusage rUsage;
    getrusage(RUSAGE_SELF, &rUsage);
    long ms = rUsage.ru_maxrss;
    float gms = ms / 1024 ;
    resultStream <<"bucket_memory " << gms << "mb\n";
    resultStream<<"-----------------------------------"<<endl;
}

void WSSAliasTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream, bool frequency) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;


//    sort(ele_vec, ele_vec+n, cmp_element_weight);
    vector<Element> ele(ele_vec,ele_vec+n);
    start = chrono::system_clock::now();
    aliasMethod alias(n,ele);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "basic bucket build_time:" << elapsed_seconds.count() << "\n";
    int tmp = 0;
    start = chrono::system_clock::now();
    if(!frequency) {
        for (auto i: opt_vec) {
//            cout<<i.opt_type<<' '<<i.key<<' '<<i.value<<' '<<i.weight<<endl;
            if (i.opt_type == 0) {
                for(int j=1;j<=i.value;j++)
                    tmp+=alias.random_sample_value();
            }
            if (i.opt_type == 1) {
                Element tmp_ele = Element(i.key, i.value, i.weight);
                alias.insert_element(tmp_ele);
            }
            if (i.opt_type == 2) {
                alias.delete_element(i.key);
            }
        }
    }else{
        chrono::high_resolution_clock::time_point t1,t2;
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                t1 = chrono::high_resolution_clock::now();
                for(int j=1;j<=i.value;j++)
                    tmp+=alias.random_sample_value();
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 1) {
                t1 = chrono::high_resolution_clock::now();
                Element tmp_ele = Element(i.key, i.value, i.weight);
                alias.insert_element(tmp_ele);
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 2) {
                t1 = chrono::high_resolution_clock::now();
                alias.delete_element(i.key);
                t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
        }
    }
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    if(frequency) return;
    resultStream << "basic alias Operation_time:" << elapsed_seconds.count() << "\n";
    resultStream << "basic average alias Operation_time:" << elapsed_seconds.count()/opt_vec.size()*1e9<< "\n";
    struct rusage rUsage;
    getrusage(RUSAGE_SELF, &rUsage);
    long ms = rUsage.ru_maxrss;
    float gms = ms / 1024 ;
    resultStream <<"alias_memory " << gms << "mb\n";
    resultStream<<"-----------------------------------"<<endl;
}

void WSSTest(int status, bool frequency, string resultFile) {
    ifstream tmpf;
    int n,m;
    vector<Opt> opt_vec;
    tmpf.open(tmpData.c_str());
    int _k, _v,_times; float _w;
    tmpf >> n;
    for (int i = 0; i < n; i++) {
        tmpf >> _k >> _v >> _w;
        element[i] = Element(_k, _v, _w);
    }

    int opt_type;
    tmpf >> m;
//    cout<<m<<endl;
//    cerr<<n<<' '<<m<<endl;
    for (int i = 0; i < m; i++) {
        tmpf >> opt_type;
        if (opt_type == 0) {
            int sampleTimes;
            tmpf >>sampleTimes;
            opt_vec.push_back(Opt(0, 0, sampleTimes, 0));
        }
        else if (opt_type == 1) {
            tmpf >> _k >> _v >> _w;
            opt_vec.push_back(Opt(1, _k, _v, _w));
        }
        else if (opt_type == 2) {
            tmpf >> _k;
            opt_vec.push_back(Opt(2, _k, 0, 0));
        }
    }

    ofstream resultStream;
    resultStream.open(resultFile.c_str());
//    random_shuffle(element,element+n);
    if(!status){
//        sortTest(n, element, opt_vec, resultStream, frequency);
        WSSAliasTest(n, element, opt_vec, resultStream, frequency);
        return;
    }
//    cerr<<"begin"<<endl;
    if(status&1) WSSBstTest(n, element, opt_vec, resultStream, frequency);

    if(status&2) WSSBucketTest(n, element, opt_vec, resultStream, frequency);
}

void WIRSBstTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream, bool frequency) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

//    sort(ele_vec, ele_vec+n, cmp_element_key);
    vector<Element> ele(ele_vec,ele_vec+n);
    BSTSampling bst;

    bst.init(ele_vec,n);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    if(!frequency)resultStream << "basic bst tree build_time:" << elapsed_seconds.count() << "\n";
    start = chrono::system_clock::now();
    if(!frequency) {
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                bst.ask(i.key,i.value,i.weight);
            }
            if (i.opt_type == 1) {
                Element tmp_ele = Element(i.key, i.value, i.weight);
                bst.insert(tmp_ele);
            }
            if (i.opt_type == 2) {
                bst.erase(i.key);
            }
        }
    }else{
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
                bst.ask(i.key,i.value,i.weight);
                chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 1) {
                chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
                Element tmp_ele = Element(i.key, i.value, i.weight);

                bst.insert(tmp_ele);
                chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 2) {
                chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
                bst.erase(i.key);
                chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
        }
    }
    if(frequency) return;
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "bst Operation_time:" << elapsed_seconds.count() << "\n";
    resultStream << "bst average bst Operation_time:" << elapsed_seconds.count()/opt_vec.size()*1e9<< "\n";
    struct rusage rUsage;
    getrusage(RUSAGE_SELF, &rUsage);
    long ms = rUsage.ru_maxrss;
    float gms = ms / 1024 ;
    resultStream <<"bst_memory " << gms << "mb\n";
    resultStream<<"-----------------------------------"<<endl;
    bst.clear();
}

void WIRSBucketTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream, bool frequency,int chunkSize) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

//    sort(ele_vec, ele_vec+n, cmp_element_key);
//    vector<Element> ele(ele_vec,ele_vec+n);
//    ChunkScapegoatTree<BucketMethod> chunkBucketTree;
    QuickChunkScapegoatTree chunkBucketTree;
    chunkBucketTree.setChunkSize(chunkSize);
    chunkBucketTree.init(ele_vec,n);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;

    if(!frequency) resultStream << "basic bucket build_time:" << elapsed_seconds.count() << "\n";
    start = chrono::system_clock::now();
    if(!frequency) {
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                chunkBucketTree.ask(i.key,i.value,i.weight);
            }
            if (i.opt_type == 1) {
//                cout<<i.key<<' '<<i.value<<' '<<i.weight<<endl;
                Element tmp_ele = Element(i.key, i.value, i.weight);
                chunkBucketTree.insert(tmp_ele);
            }
            if (i.opt_type == 2) {
                chunkBucketTree.erase(i.key);
            }
        }
    }else{
        for (auto i: opt_vec) {
            if (i.opt_type == 0) {
                chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

                chunkBucketTree.ask(i.key,i.value,i.weight);
                chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 1) {
                chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
                Element tmp_ele = Element(i.key, i.value, i.weight);

                chunkBucketTree.insert(tmp_ele);
                chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
            if (i.opt_type == 2) {
                chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

                chunkBucketTree.erase(i.key);
                chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
                resultStream<<chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()<<endl;
            }
        }
    }
    if(frequency) return;
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "chunk bucket Operation_time:" << elapsed_seconds.count() << "\n";
    resultStream << "bucket average bucket Operation_time:" << elapsed_seconds.count()/opt_vec.size()*1e9<< "\n";
    struct rusage rUsage;
    getrusage(RUSAGE_SELF, &rUsage);
    long ms = rUsage.ru_maxrss;
    float gms = ms / 1024 ;
    resultStream <<"bucket_memory " << gms << "mb\n";
    resultStream<<"-----------------------------------"<<endl;
    chunkBucketTree.clear();
}

void WIRSAliasTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream,int chunkSize) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

//    sort(ele_vec, ele_vec+n, cmp_element_key);
    AliasChunkScapegoatTree chunkBucketTree;
    chunkBucketTree.setChunkSize(chunkSize);
    chunkBucketTree.init(ele_vec,n);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "alias tree build_time:" << elapsed_seconds.count() << "\n";
    start = chrono::system_clock::now();
    for (auto i: opt_vec) {
        if (i.opt_type == 0) {
            chunkBucketTree.ask(i.key,i.value,i.weight);
//            cout<<i.key<<' '<<i.value<<' '<<i.weight<<endl;
        }
    }
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "alias tree Operation_time:" << elapsed_seconds.count() << "\n";
    resultStream << "alias tree average Operation_time:" << elapsed_seconds.count()/opt_vec.size()*1e9<< "\n";
    struct rusage rUsage;
    getrusage(RUSAGE_SELF, &rUsage);
    long ms = rUsage.ru_maxrss;
    float gms = ms / 1024 ;
    resultStream <<"alias tree memory " << gms << "mb\n";
    resultStream<<"-----------------------------------"<<endl;
}

void WIRSChunkWithAliasTest(int n, Element* ele_vec, vector<Opt>& opt_vec , ofstream &resultStream,int chunkSize) {
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

//    sort(ele_vec, ele_vec+n, cmp_element_key);
    ChunkScapegoatTreeWithAlias chunkBucketTree;
    chunkBucketTree.setChunkSize(chunkSize);
    chunkBucketTree.init(ele_vec,n);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "alias build_time:" << elapsed_seconds.count() << "\n";
    start = chrono::system_clock::now();
    for (auto i: opt_vec) {
        if (i.opt_type == 0) {
            chunkBucketTree.ask(i.key,i.value,i.weight);
//            cout<<i.key<<' '<<i.value<<' '<<i.weight<<endl;
        }
    }
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    resultStream << "alias Operation_time:" << elapsed_seconds.count() << "\n";
    resultStream << "alias average Operation_time:" << elapsed_seconds.count()/opt_vec.size()*1e9<< "\n";
    struct rusage rUsage;
    getrusage(RUSAGE_SELF, &rUsage);
    long ms = rUsage.ru_maxrss;
    float gms = ms / 1024 ;
    resultStream <<"alias_memory " << gms << "mb\n";
    resultStream<<"-----------------------------------"<<endl;
}

void WIRSTest(int status, bool frequency, int chunkSize, string resultFile) {
    ifstream tmpf;
    int n,m;
    vector<Opt> opt_vec;
    tmpf.open(tmpData.c_str());
    int _k, _v,_times; float _w;
    tmpf >> n;
    for (int i = 0; i < n; i++) {
        tmpf >> _k >> _v >> _w;
        element[i] = Element(_k, _v, _w);
    }
    sort(element,element+n+1, cmp_element_key);
    int opt_type;
    tmpf >> m;
//    cout<<m<<endl;
//    cerr<<n<<' '<<m<<endl;
    for (int i = 0; i < m; i++) {
        tmpf >> opt_type;
        if (opt_type == 0) {
            tmpf >> _k >> _v >> _w;
            opt_vec.push_back(Opt(0, _k, _v, _w));
        }
        else if (opt_type == 1) {
            tmpf >> _k >> _v >> _w;
            opt_vec.push_back(Opt(1, _k, _v, _w));
        }
        else if (opt_type == 2) {
            tmpf >> _k;
            opt_vec.push_back(Opt(2, _k, 0, 0));
        }
    }
    ofstream resultStream;
    resultStream.open(resultFile.c_str());
    if(status&1) WIRSBstTest(n, element, opt_vec, resultStream, frequency);
    puts("OK");

    if(status&2) WIRSBucketTest(n, element, opt_vec, resultStream, frequency, chunkSize);
    if(chunkAlias) {
        WIRSAliasTest(n, element, opt_vec, resultStream,chunkSize);
        WIRSChunkWithAliasTest(n,element, opt_vec, resultStream,chunkSize);
    }
}

float *f;

void test_discrete_distribution(int n,int m,string name){
    ifstream tmpf;
    tmpf.open((dataHome + name + ".data").c_str());
    ofstream fout;
    fout.open( resultHome + "/discrete_" + name );
    f = new float[n+10];
    int _k, _v; float _w;
    for (int i = 0; i < n; i++) {
        tmpf >> _k >> _v >> _w;
        f[i] = _w;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> d(f,f+n);

    int x = 0;
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

    for(int i= 1;i<=m;i++)
        x+=d(gen);
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;

    fout << "discrete Operation_time:" << elapsed_seconds.count() << "\n";
    fout << "discrete average bucket Operation_time:" << elapsed_seconds.count()/m*1e9<< "\n";
    cout<<x<<endl;
}

void test_alias_distribution(int n,int m,string name){
    ifstream tmpf;
    tmpf.open((dataHome + name + ".data").c_str());
    vector<Element> ele;
    int _k, _v; float _w;
    for (int i = 0; i < n; i++) {
        tmpf >> _k >> _v >> _w;
        ele.push_back(Element(_k,_v,_w));
    }
    std::random_device rd;
    aliasMethod alias(n,ele);


    ofstream fout;
    fout.open( resultHome + "/alias_" + name );
    int x = 0;
    chrono::time_point<chrono::system_clock> start, end;
    chrono::duration<double> elapsed_seconds;

    start = chrono::system_clock::now();

    for(int i= 1;i<=m;i++)
        x+=alias.random_sample_value();
    end = chrono::system_clock::now();
    elapsed_seconds = end - start;
    fout << "alias Operation_time:" << elapsed_seconds.count() << "\n";
    fout << "alias average bucket Operation_time:" << elapsed_seconds.count()/m*1e9<< "\n";
    cout<<x<<endl;
}

int main(int argc,char *argv[]) {
    if(argc==4){
        string testName = argv[1];
        int dataNum = getNum(argv[2]);
        int optNum = getNum(argv[3]);
        test_discrete_distribution(dataNum,optNum,testName);
        test_alias_distribution(dataNum,optNum,testName);
        return 0;
    }
    string testName = argv[1];
    cout <<argc<<endl;
//    cerr<<argv[2]<<" "<<argv[3]<<" "<<argv[4]<<endl;
    if(testName == "WSS"){
        string filename = argv[2];
        string op = argv[3];
        int dataNum = getNum(argv[4]);
        element = new Element[dataNum + 100];
        if(op=="build"){
            string method = argv[5];
            int id = getNum(argv[6]);
            WSSProduceQuery(dataNum, 0, dataHome + filename + ".data", tmpData,0);
            string resultFile =
                    resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_" +method+"_"+
                    to_string(id);
            if(method == "bst") WSSTest(1, false, resultFile);
            if(method == "bucket") WSSTest(2, false, resultFile);
            if(method == "alias") WSSTest(0, false, resultFile);
        }
        else if(op=="buildQ"){
            int id = getNum(argv[5]);
            WSSProduceQuery(dataNum, 0, dataHome + filename + ".data", tmpData,0);
            string resultFile =
                    resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) +"_"+
                    to_string(id);
            WSSTest(0, false, resultFile);
        }
        else if(op=="mix"){
            int optNum = getNum(argv[5]);
            int updatePercent = getNum(argv[6]);
            int sampleTimes = getNum(argv[7]);
            WSSProduceMix(dataNum, optNum, dataHome + filename + ".data", tmpData, sampleTimes,updatePercent);
            string resultFile =
                    resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                    to_string(optNum)+"_percent_"+ to_string(updatePercent)+"_sampleTimes_"+ to_string(sampleTimes);
            WSSTest(3, false, resultFile);
        }
        else if(op=="mixA"){
            int optNum = getNum(argv[5]);
            int updatePercent = getNum(argv[6]);
            int sampleTimes = getNum(argv[7]);
            WSSProduceMix(dataNum, optNum, dataHome + filename + ".data", tmpData, sampleTimes,updatePercent);
            string resultFile =
                    resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                    to_string(optNum)+"_percent_"+ to_string(updatePercent)+"_sampleTimes_"+ to_string(sampleTimes);
            WSSTest(0, false, resultFile);
        }
        else {
            int optNum = getNum(argv[5]);
            if (op == "query") WSSProduceQuery(dataNum, optNum, dataHome + filename + ".data", tmpData, 1);
            if (op == "ins") produceIns(dataNum, optNum, dataHome + filename + ".data", tmpData);
            if (op == "del") produceDel(dataNum, optNum, dataHome + filename + ".data", tmpData);
//        cerr<<"produce done"<<endl;
            if (op=="query"||op=="ins"||op=="del") {
                string resultFile =
                        resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                        to_string(optNum);

                cout << resultFile << endl;
//                WSSTest(3, false, resultFile);
                WSSTest(0, false, resultFile);
            }

            if (op == "queryF"){
                WSSProduceQuery(dataNum, optNum, dataHome + filename + ".data", tmpData, 1);
                string resultFile =
                        resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                        to_string(optNum)+"frequency";

                cout<< resultFile <<endl;
                WSSTest(1, true, resultFile+"_bst");
                WSSTest(2, true, resultFile+"_bucket");
            }
            if (op == "insF") {
                produceIns(dataNum, optNum, dataHome + filename + ".data", tmpData);
                string resultFile =
                        resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                        to_string(optNum)+"frequency";

                cout<< resultFile <<endl;
                WSSTest(1, true, resultFile+"_bst");
                WSSTest(2, true, resultFile+"_bucket");
            }
            if (op == "delF") {
                produceDel(dataNum, optNum, dataHome + filename + ".data", tmpData);
                string resultFile =
                        resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                        to_string(optNum)+"frequency";

                cout<< resultFile <<endl;
                WSSTest(1, true, resultFile+"_bst");
                WSSTest(2, true, resultFile+"_bucket");
            }

        }
    }

    if(testName == "WIRS"){
        string filename = argv[2];
        string op = argv[3];
        int dataNum = getNum(argv[4]);
        element = new Element[dataNum + 100];
        int chunkSize = getNum(argv[5]);
        if(op=="build"){
            string method = argv[6];
            int id = getNum(argv[7]);
            WSSProduceQuery(dataNum, 0, dataHome + filename + ".data", tmpData,0);
            string resultFile =
                    resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_" +method+"_csize_"+ to_string(chunkSize)+"_"+
                    to_string(id);
            if(method == "bst") WIRSTest(1, false, chunkSize, resultFile);
            if(method == "bucket") WIRSTest(2, false, chunkSize, resultFile);
        }

        else if(op=="mix"){
            int optNum = getNum(argv[6]);
            int updatePercent = getNum(argv[7]);
            int sampleTimes = getNum(argv[8]);
            int coverage = getNum(argv[9]);
            WIRSProduceMix(dataNum, optNum, dataHome + filename + ".data", tmpData, sampleTimes,updatePercent,coverage);
            string resultFile =
                    resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                    to_string(optNum)+"_percent_"+ to_string(updatePercent)+"_sampleTimes_"+ to_string(sampleTimes);
            WIRSTest(3, false, chunkSize, resultFile);
        }
        else {
            int optNum = getNum(argv[6]);

            if (op == "ins") produceIns(dataNum, optNum, dataHome + filename + ".data", tmpData);
            if (op == "del") produceDel(dataNum, optNum, dataHome + filename + ".data", tmpData);
//        cerr<<"produce done"<<endl;
            string resultFile =
                    resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                    to_string(optNum)+"_csize_"+ to_string(chunkSize);

            if (op == "query") {
                int coverage = getNum(argv[7]);
                int sampleTimes = getNum(argv[8]);
//                puts("OK");
                WIRSProduceQuery(dataNum, optNum, dataHome + filename + ".data", tmpData, sampleTimes, coverage);
                resultFile = resultFile + "_cov_" + to_string(coverage)+"_T_"+ to_string(sampleTimes);
                puts("OK");
            }

            if (op == "queryA") {
                int coverage = getNum(argv[7]);
                int sampleTimes = getNum(argv[8]);
                chunkAlias = 1;
//                puts("OK")
                WIRSProduceQuery(dataNum, optNum, dataHome + filename + ".data", tmpData, sampleTimes, coverage);
                resultFile = resultFile + "_cov_" + to_string(coverage)+"_T_"+ to_string(sampleTimes);
                puts("OK");
            }

            if(op=="query"||op=="ins"||op=="del"||op=="queryA") {
                cout << resultFile << endl;
                WIRSTest(3, false, chunkSize, resultFile);
            }
            else{
                if (op == "insF") produceIns(dataNum, optNum, dataHome + filename + ".data", tmpData);
                if (op == "delF") produceDel(dataNum, optNum, dataHome + filename + ".data", tmpData);
//        cerr<<"produce done"<<endl;
                string resultFile =
                        resultHome + "/" + testName + "/" + op + "/" + filename + "_n_" + to_string(dataNum) + "_m_" +
                        to_string(optNum)+"_csize_"+ to_string(chunkSize);

                if (op == "queryF") {
                    int coverage = getNum(argv[7]);
                    int sampleTimes = getNum(argv[8]);
//                puts("OK");
                    WIRSProduceQuery(dataNum, optNum, dataHome + filename + ".data", tmpData, sampleTimes, coverage);
                    resultFile = resultFile + "_cov_" + to_string(coverage)+"_T_"+ to_string(sampleTimes);
                    puts("OK");
                }
                cout<< resultFile <<endl;
                WIRSTest(1, true, chunkSize,resultFile+"_bst");
                WIRSTest(2, true, chunkSize, resultFile+"_bucket");
            }
        }
    }
    return 0;
}