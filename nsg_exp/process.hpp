#pragma once

#include "nsg/include/efanna2e/index_nsg.h"
#include "../utils/time.hpp"

using namespace std;

pair<double, double> get_recall(efanna2e::IndexNSG* nsg, float * query, vector<vector<int>> & ans, int n_query, int dim, int topk, int beamsize)
{
    // set param
    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", beamsize);
    paras.Set<unsigned>("P_search", beamsize);

    // answer vec
    std::vector<std::vector<unsigned> > res(n_query);
    for (unsigned i = 0; i < n_query; i++) res[i].resize(topk);

    auto start = timing::now();

    float correct = 0;

    // search one by one
    for (unsigned i = 0; i < n_query; i++) {
        nsg->SearchWithOptGraph(query + i * dim, topk, paras, res[i].data());
        if (res[i][0] == ans[i][0])correct++;
    }
    float recall = correct / n_query;
    double timetook = timing::get_duration(start);

    return {recall, timetook};
}


pair<double, double> get_recall_optimized(efanna2e::IndexNSG* nsg, 
                                          float * query, 
                                          vector<vector<int>> & ans, 
                                          vector<pair<int, int>> & trajectory, 
                                          int n_query, 
                                          int dim, 
                                          int topk, 
                                          int beamsize)
{
    // set param
    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", beamsize);
    paras.Set<unsigned>("P_search", beamsize);

    // answer vec
    std::vector<std::vector<unsigned> > res(n_query);
    for (unsigned i = 0; i < n_query; i++) res[i].resize(topk);

    auto start = timing::now();
    float correct = 0;

    for (auto x : trajectory)
    {
        int curr = x.first;
        int parent = x.second;

        if (parent!=-1)
        {
            int entry_point = res[parent][0];
            nsg->SearchWithOptGraph_mine(query + curr * dim, topk, paras, res[curr].data(), entry_point);
        }
        else nsg->SearchWithOptGraph_mine(query + curr * dim, topk, paras, res[curr].data());

        if (res[curr][0] == ans[curr][0])correct++;

    }
    float recall = correct / n_query;
    double timetook = timing::get_duration(start);

    return {recall, timetook};
}



pair<double, double> get_recall_ip(efanna2e::IndexNSG* nsg, float * data, float * query, vector<vector<int>> & ans, int n_query, int dim, int topk, int beamsize)
{
    // set param
    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", beamsize);
    paras.Set<unsigned>("P_search", beamsize);

    // answer vec
    std::vector<std::vector<unsigned> > res(n_query);
    for (unsigned i = 0; i < n_query; i++) res[i].resize(topk);

    auto start = timing::now();

    float correct = 0;

    // search one by one
    for (unsigned i = 0; i < n_query; i++) {
        nsg->Search(query + i * dim, data, topk, paras, res[i].data());
        if (res[i][0] == ans[i][0])correct++;
    }
    float recall = correct / n_query;
    double timetook = timing::get_duration(start);

    return {recall, timetook};
}


pair<double, double> get_recall_optimized_ip(efanna2e::IndexNSG* nsg, float *data, 
    float * query, 
    vector<vector<int>> & ans, 
    vector<pair<int, int>> & trajectory, 
    int n_query, 
    int dim, 
    int topk, 
    int beamsize)
{
// set param
efanna2e::Parameters paras;
paras.Set<unsigned>("L_search", beamsize);
paras.Set<unsigned>("P_search", beamsize);

// answer vec
std::vector<std::vector<unsigned> > res(n_query);
for (unsigned i = 0; i < n_query; i++) res[i].resize(topk);

auto start = timing::now();
float correct = 0;

for (auto x : trajectory)
{
int curr = x.first;
int parent = x.second;

if (parent!=-1)
{
int entry_point = res[parent][0];
nsg->Search_mine(query + curr * dim, data, topk, paras, res[curr].data(), entry_point);
}
else nsg->Search_mine(query + curr * dim, data, topk, paras, res[curr].data());

if (res[curr][0] == ans[curr][0])correct++;

}
float recall = correct / n_query;
double timetook = timing::get_duration(start);

return {recall, timetook};
}
