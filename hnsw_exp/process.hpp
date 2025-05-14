#pragma once

#include "hnswlib/hnswlib.h"
#include "../utils/time.hpp"

using namespace std;

float get_self_recall(hnswlib::HierarchicalNSW<float>* alg_hnsw, float* data, int n_elements, int dim)
{
    cout<<"calculating recall for self data"<<endl;
    // Query the elements for themselves and measure recall
    float correct = 0;
    for (long long i = 0; i < n_elements; i++) {
        std::priority_queue<std::pair<float, hnswlib::labeltype>> result = alg_hnsw->searchKnn(data + i * dim, 1);
        hnswlib::labeltype label = result.top().second;
        if (label == i) correct++;
    }
    float recall = correct / n_elements;
    return recall;
}


/* ################################################################################################### */

pair<double, double> get_recall(hnswlib::HierarchicalNSW<float>* alg_hnsw, float* querydata, vector<vector<int>> & ans, int n_elements, int dim, int topk, int beamsize)
{
    //cout<<"calculating recall for queries"<<endl;
    alg_hnsw->ef_ = beamsize;

    auto start = timing::now();
    float correct = 0;
    vector<hnswlib::tableint> empty_entry_points;
    for (long long i = 0; i < n_elements; i++) {
        std::priority_queue<std::pair<float, hnswlib::labeltype>> result = alg_hnsw->searchKnn_mine(querydata + i * dim, topk, empty_entry_points);
        //std::priority_queue<std::pair<float, hnswlib::labeltype>> result = alg_hnsw->searchKnn(querydata + i * dim, topk);
        hnswlib::labeltype label = result.top().second;
        if (label == ans[i][0]) correct++;
    }
    float recall = correct / n_elements;
    double timetook = timing::get_duration(start);
    return {recall, timetook};
}


//single entry
pair<double, double> get_recall_optimized(hnswlib::HierarchicalNSW<float>* alg_hnsw, 
                           float* querydata,
                           vector<vector<int>> & ans, 
                           vector<pair<int, int>> & trajectory, 
                           int n_elements, 
                           int dim, 
                           int topk, 
                           int beamsize)
{
    //cout<<"calculating recall for queries"<<endl;
    alg_hnsw->ef_ = beamsize;
    // Query the elements for themselves and measure recall
    vector<int> ann(n_elements);

    auto start = timing::now();
    float correct = 0;
    vector<hnswlib::tableint> empty_entry_points;

    for (auto x : trajectory)
    {
        int curr = x.first;
        int parent = x.second;
        std::priority_queue<std::pair<float, hnswlib::labeltype>> result;
        if(parent!=-1) 
        {
            vector<hnswlib::tableint> entry_points =  { static_cast<hnswlib::tableint>(ann[parent]) };
            result= alg_hnsw->searchKnn_mine(querydata + curr * dim, topk, entry_points);
        }
        else result = alg_hnsw->searchKnn_mine(querydata + curr * dim, topk, empty_entry_points);
        hnswlib::labeltype label = result.top().second;
        ann[curr] = label;
        if (label == ans[curr][0]) correct++;
    }
    float recall = correct / n_elements;
    double timetook = timing::get_duration(start);
    return {recall, timetook};
}


//mutliple entry
pair<double, double> get_recall_optimized2(hnswlib::HierarchicalNSW<float>* alg_hnsw, 
    float* querydata,
    vector<vector<int>> & ans, 
    vector<pair<int, int>> & trajectory, 
    int n_elements, 
    int dim, 
    int topk, 
    int beamsize,
    int n_entry_points = 3)
{
    assert(n_entry_points <= beamsize);
    //cout<<"calculating recall for queries"<<endl;
    alg_hnsw->ef_ = beamsize;
    // Query the elements for themselves and measure recall
    vector<vector<hnswlib::tableint>> partial_res(n_elements);

    auto start = timing::now();
    float correct = 0;
    vector<hnswlib::tableint> empty_entry_points;

    for (auto x : trajectory)
    {
        int curr = x.first;
        int parent = x.second;
        std::priority_queue<std::pair<float, hnswlib::labeltype>> result;
        if(parent!=-1) result= alg_hnsw->searchKnn_mine(querydata + curr * dim, n_entry_points, partial_res[parent]);
        else result = alg_hnsw->searchKnn_mine(querydata + curr * dim, n_entry_points, empty_entry_points);

        hnswlib::labeltype label;

        for(int rep = 0; rep< n_entry_points; rep++)
        {
            if(rep == n_entry_points-1)label = result.top().second;  //weird but its reverse!!!!
            partial_res[curr].push_back(result.top().second);
            result.pop();
        }

        if (label == ans[curr][0]) correct++;
    }
    float recall = correct / n_elements;
    double timetook = timing::get_duration(start);
    return {recall, timetook};
}


/* ################################################################################################### */
