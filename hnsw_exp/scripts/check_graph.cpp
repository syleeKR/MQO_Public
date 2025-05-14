
#include "../hnswlib/hnswlib.h"
#include "../../data_handler/data_handler.hpp"
#include "../graph_handler.hpp"
#include "../process.hpp"

#include <bits/stdc++.h>
using namespace std;

int main(int argc, char * argv[]) 
{
    string dpath = "../../data/deep/data/data10M.fbin";
    string gpath = "../../data/deep/hnsw/10M/hnsw_10M_16_50.bin";


    int dim;
    int n_elements;
    float * data = get_data(dpath, n_elements, dim);
    auto res = read(gpath, dim);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = res.first;
    hnswlib::L2Space * space = res.second;

    cout<<"self recall : "<< get_self_recall(alg_hnsw,  data,  n_elements ,dim) << endl;

}