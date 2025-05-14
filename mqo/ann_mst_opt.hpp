#pragma once

#include "ann/build_mst_from_ann.hpp"
#include "ann/build_ann.hpp"
#include "ann/build_ann_base_only.hpp"

#include "ann/hnswlib/hnswalg.h"   // Your "hnswalg.h" file
#include "../utils/time.hpp"

#include <vector>
#include <mutex>       // for std::lock_guard
#include <utility>     // for std::pair
#include <queue>
#include <unordered_map>
#include <limits>
#include <bits/stdc++.h>

using namespace std;



/* ####################################### */

pair<vector<pair<int, int>>, pair<double, double>> optimize_trajectory_ann_mst(float* query, int n_query, int dim, map<string, int> & params)
{

    auto start = timing::now();

    int M = params["M"];
    int ef_construction = params["ef"];
    int use_base_only = params["base_only"];

    std::tuple<mqohnswlib::HierarchicalNSW<float>*,mqohnswlib::L2Space*,map<pair<int, int>, float>> graph;
    if(use_base_only)graph = build_ann_base_only(query, n_query, dim, M, ef_construction);
    else graph  = build_ann_full_l2(query, n_query, dim, M, ef_construction);

    mqohnswlib::HierarchicalNSW<float> * qann = get<0>(graph);
    map<pair<int, int>, float> distinfo = move(get<2>(graph));
    pair< vector<pair<int, int>>, double> res = approximateMSTUsingHNSW(qann, distinfo, n_query, dim);
    double duration = timing::get_duration(start);

    return {move(res.first), {duration, res.second}};
}