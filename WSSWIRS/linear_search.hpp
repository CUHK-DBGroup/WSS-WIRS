#pragma once

#include "utility.h"
#include <queue>
#include <random>
#include <iostream>
#include <unordered_map>
using namespace std;

class linearMethod {
public:
    //create the prob_bias and alias table
    linearMethod() : gen(rd()) {}
    linearMethod(int num, vector<Element>& all_ele) : gen(rd()) {
        for (int i = 0; i < num; i++) {
            elements.push_back(all_ele[i]);
            position_map[all_ele[i].key] = i;
        }
        ele_size = num;
    }
    //randomly generate
    double calc_proportion_sum() {
        double tot_weight = 0;
        double sum = 0;
        for (int i = 0; i < ele_size; i++)
            tot_weight += elements[i].weight;
        for (auto i : elements) {
            sum += i.value * (i.weight / tot_weight);
        }
        //cerr << " " << tot_weight << "\n";
        return sum;
    }


    void delete_element(int del_key) {
        int del_pos = position_map[del_key];
        int tail_key = elements[ele_size - 1].key;
        elements[del_pos] = elements[ele_size - 1];
        position_map[tail_key] = del_pos;
        position_map.erase(del_key);
        elements.pop_back();
        ele_size--;
    }


    void insert_element(Element ins_ele) {
        if (position_map.find(ins_ele.key) != position_map.end()) {
            cout << "insert an existed element!!!!!!!!!\n";
        }
        position_map[ins_ele.key] = ele_size;
        elements.push_back(ins_ele);
        ele_size++;
    }

    bool find_key(int& tmp_key) {
        return (position_map.find(tmp_key) != position_map.end());
    }

    int random_choose() {
        pos_rand = uniform_int_distribution<int>(0, ele_size - 1);
        int choose_pos = pos_rand(gen);
        //choose_pos = ele_size - 1;
        return elements[choose_pos].key;
    }
private:
    int ele_size;
    random_device rd;
    mt19937 gen;
    uniform_int_distribution<int> pos_rand;
    uniform_real_distribution<double> dr;
    //int* alias;
    vector<int> alias;
    vector<float> prob_bias;
    vector<Element> elements;
    unordered_map<int, int> position_map;
    //double* prob_bias;

    
};
