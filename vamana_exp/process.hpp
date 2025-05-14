#pragma once

#include "DiskANN/include/index.h"
#include "../utils/time.hpp"
#include <vector>
#include <iostream>
using namespace std;



std::pair<double, double> get_recall(
    diskann::AbstractIndex& index,
    const float *queries,
    const std::vector<std::vector<int>> &answers,
    int n_query,
    int dim,
    int topk,
    int Lsearch)
{

    std::vector<uint32_t> ids(topk);
    std::vector<float>    dists(topk);

    int correct_count = 0;
    auto start = timing::now();

    for (int i = 0; i < n_query; i++) {
        index.search(queries + (size_t)i * dim, topk, Lsearch,ids.data(), dists.data());

        // Compare top result to answers
        if (!answers[i].empty() && (int)ids[0] == answers[i][0]) {
            correct_count++;
        }
    }

    double timetook = timing::get_duration(start);
    double recall = (double)correct_count / n_query;

    return {recall, timetook};
}



std::pair<double, double> get_recall_optimized(
    diskann::AbstractIndex& index,
    const float *queries,
    const std::vector<std::vector<int>> &answers,
    vector<pair<int, int>> & trajectory, 
    int n_query,
    int dim,
    int topk,
    int Lsearch)
{
    // result 
    std::vector<std::vector<unsigned> > res(n_query);
    for (unsigned i = 0; i < n_query; i++) res[i].resize(topk);
    // distance ( for vamana search )
    std::vector<float> dists(topk);

    int correct_count = 0;

    auto start = timing::now();

    for (auto x : trajectory)
    {
        int curr = x.first;
        int parent = x.second;

        if (parent!=-1)
        {
            int entry_point = res[parent][0];
            index.search(queries + (size_t)curr * dim, topk, Lsearch, res[curr].data(), dists.data(), entry_point);
        }
        else index.search(queries + (size_t)curr * dim, topk, Lsearch, res[curr].data(), dists.data());

        if ( (int)res[curr][0] == answers[curr][0])correct_count++;

    }

    double timetook = timing::get_duration(start);
    double recall = (double)correct_count / n_query;

    return {recall, timetook};
}

