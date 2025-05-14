#pragma once

#include <bits/stdc++.h>
#include "basic.hpp"               
#include "../utils/time.hpp"       
#include "lsh_mst_opt.hpp"             
#include "ann/build_ann.hpp"            
#include "ann/build_ann_base_only.hpp"          
#include "ann/build_mst_from_ann.hpp"

using namespace std;

inline pair<vector<pair<int,int>>, pair<double,double>>
optimize_trajectory_mixed_mst(float* query,
                           int   n_query,
                           int   dim,
                           map<string,int>& params)
{
    int B               = params.at("B");
    int M               = params.at("M");
    int ef_construction = params.at("ef");
    bool base_only      = (params.count("base_only") && params["base_only"] != 0);

    float* hyperplane = generateRandomHyperplane(B, /*rep=*/1, dim);

    auto t0 = timing::now();
    auto pq = split(query, hyperplane, n_query, dim, B);

    vector<pair<int,int>> fullTrajectory;
    double sumWeights = 0.0;
    long long sumEdges = 0;

    while (!pq.empty()) {
        GroupInfo gi = pq.top(); pq.pop();
        auto   grp = move(gi.group);
        int    szg = grp.size();
        if (szg <= 1) {
            // singleton → trivial component
            fullTrajectory.emplace_back(grp[0], -1);
            continue;
        }

        float* subQ = new float[szg * dim];
        for (int i = 0; i < szg; i++) {
            memcpy(subQ + i*dim,
                   query + grp[i]*dim,
                   sizeof(float)*dim);
        }

        mqohnswlib::HierarchicalNSW<float>*  index = nullptr;
        mqohnswlib::L2Space*                space = nullptr;
        map<pair<int,int>,float>            distinfo;
        if (base_only) {
            tie(index, space, distinfo)
                = build_ann_base_only(subQ, szg, dim,
                                      M, ef_construction);
        } else {
            tie(index, space, distinfo)
                = build_ann_full_l2(subQ, szg, dim,
                                 M, ef_construction);
        }

        auto [locTraj, locAvgW] =
            approximateMSTUsingHNSW(index, distinfo, szg, dim);

        int localEdgeCount = 0;
        for (auto& e : locTraj) {
            int ni = e.first;
            int pi = e.second;
            int gni = grp[ni];
            int gpi = (pi >= 0 ? grp[pi] : -1);
            fullTrajectory.emplace_back(gni, gpi);
            if (pi >= 0) localEdgeCount++;
        }
        sumWeights += locAvgW * localEdgeCount;
        sumEdges   += localEdgeCount;

        // cleanup
        delete index;
        delete space;
        delete[] subQ;
    }

    double duration = timing::get_duration(t0);


    double weight_for_root = get_weight_for_root(query, n_query, dim);

    double avgWeight = (sumWeights + (n_query - sumEdges)*weight_for_root)/ n_query ;
                        

    return { move(fullTrajectory),
             { duration, avgWeight } };
}




/* #################################################################### */

inline pair<vector<pair<int,int>>, pair<double,double>>
_optimize_trajectory_mixed_modified_mst(float* query,
                           int   n_query,
                           int   dim,
                           bool use_l2,
                           map<string,int>& params)
{
    int B               = params.at("B");
    int M               = params.at("M");
    int ef_construction = (params.find("ef") != params.end()) ? params.at("ef") : M;
    bool base_only      = (params.count("base_only") && params["base_only"] != 0);

    float* hyperplane = generateRandomHyperplane(B, /*rep=*/1, dim);

    auto t0 = timing::now();
    auto pq = split(query, hyperplane, n_query, dim, B);

    vector<pair<int,int>> fullTrajectory;     fullTrajectory.reserve(n_query);
    double sumWeights = 0.0;
    long long sumEdges = 0;

    while (!pq.empty()) {
        GroupInfo gi = pq.top(); pq.pop();
        auto   grp = move(gi.group);
        int    szg = grp.size();
        if (szg <= 1) {
            // singleton → trivial component
            fullTrajectory.emplace_back(grp[0], -1);
            continue;
        }

        float* subQ = new float[szg * dim];
        for (int i = 0; i < szg; i++) {
            memcpy(subQ + i*dim,
                   query + grp[i]*dim,
                   sizeof(float)*dim);
        }

        mqohnswlib::HierarchicalNSW<float>*  index = nullptr;
        mqohnswlib::L2Space*                spacel2 = nullptr;
        mqohnswlib::InnerProductSpace*       spaceip = nullptr;

        map<pair<int,int>,float>            distinfo;
        if (base_only) {
            tie(index, spacel2, distinfo)
                = build_ann_base_only(subQ, szg, dim,
                                      M, ef_construction);
        } else {
            if(use_l2)tie(index, spacel2, distinfo)= build_ann_full_l2(subQ, szg, dim,M, ef_construction);
            else tie(index, spaceip, distinfo)= build_ann_full_ip(subQ, szg, dim,M, ef_construction);
        }

        auto [locTraj, locAvgW] =
            approximateMSTUsingHNSW_undirected(index, distinfo, szg, dim);

        int localEdgeCount = 0;
        for (auto& e : locTraj) {
            int ni = e.first;
            int pi = e.second;
            int gni = grp[ni];
            int gpi = (pi >= 0 ? grp[pi] : -1);
            fullTrajectory.emplace_back(gni, gpi);
            if (pi >= 0) localEdgeCount++;
        }
        sumWeights += locAvgW * localEdgeCount;
        sumEdges   += localEdgeCount;

        // cleanup
        delete index;
        if(use_l2)delete spacel2;
        else delete spaceip;
        delete[] subQ;
    }

    double duration = timing::get_duration(t0);


    double weight_for_root = get_weight_for_root(query, n_query, dim);

    double avgWeight = (sumWeights + (n_query - sumEdges)*weight_for_root)/ n_query ;
                        

    return { move(fullTrajectory),
             { duration, avgWeight } };
}


inline pair<vector<pair<int,int>>, pair<double,double>>
optimize_trajectory_mixed_modified_mst(float* query,
                                       int   n_query,
                                       int   dim,
                                       bool use_l2,
                                       map<string,int>& params)
{
    int B               = params.at("B");
    int M               = params.at("M");
    int ef_construction = (params.find("ef") != params.end()) ? params.at("ef") : M;
    bool base_only      = (params.count("base_only") && params["base_only"] != 0);

    float* hyperplane = generateRandomHyperplane(B, /*rep=*/1, dim);
    auto t0 = timing::now();
    auto pq = split(query, hyperplane, n_query, dim, B);

    vector<pair<int,int>> fullTrajectory;
    fullTrajectory.reserve(n_query);
    double sumWeights = 0.0;
    long long sumEdges = 0;

    while (!pq.empty()) {
        GroupInfo gi = pq.top(); pq.pop();
        auto   grp = move(gi.group);
        int    szg = grp.size();

        if (szg <= 1) {
            // singleton → trivial component
            fullTrajectory.emplace_back(grp[0], -1);
            continue;
        }

        if (szg <= 500) {
            vector<pair<float,int>> key(szg, { numeric_limits<float>::infinity(), -1 });
            vector<bool> inMST(szg, false);
            key[0] = { 0.0f, -1 };
            double groupMSTWeight = 0.0;
            vector<pair<int,int>> locTraj;
            locTraj.reserve(szg);

            for (int i = 0; i < szg; ++i) {
                int curr = -1, parent = -1;
                float best = numeric_limits<float>::infinity();
                // select next vertex
                for (int j = 0; j < szg; ++j) {
                    if (!inMST[j] && key[j].first < best) {
                        best = key[j].first;
                        curr = j;
                        parent = key[j].second;
                    }
                }
                inMST[curr] = true;
                groupMSTWeight += key[curr].first;
                locTraj.emplace_back(curr, parent);

                // update keys
                for (int v = 0; v < szg; ++v) {
                    if (!inMST[v]) {
                        float d = use_l2
                                  ? get_distance(query, dim, grp[curr], grp[v])
                                  : get_distance_ip(query, dim, grp[curr], grp[v]);
                        if (d < key[v].first) {
                            key[v].first = d;
                            key[v].second = curr;
                        }
                    }
                }
            }

            double locAvgW = groupMSTWeight / (szg - 1);
            int localEdgeCount = 0;
            for (auto& e : locTraj) {
                int ni = e.first;
                int pi = e.second;
                int gni = grp[ni];
                int gpi = (pi >= 0 ? grp[pi] : -1);
                fullTrajectory.emplace_back(gni, gpi);
                if (pi >= 0) ++localEdgeCount;
            }
            sumWeights += locAvgW * localEdgeCount;
            sumEdges   += localEdgeCount;
        }
        else {
            float* subQ = new float[szg * dim];
            for (int i = 0; i < szg; i++) {
                memcpy(subQ + i*dim,
                       query + grp[i]*dim,
                       sizeof(float)*dim);
            }

            mqohnswlib::HierarchicalNSW<float>* index = nullptr;
            mqohnswlib::L2Space*                spacel2 = nullptr;
            mqohnswlib::InnerProductSpace*      spaceip = nullptr;
            map<pair<int,int>,float>            distinfo;

            if (base_only) {
                tie(index, spacel2, distinfo)
                    = build_ann_base_only(subQ, szg, dim, M, ef_construction);
            } else {
                if (use_l2)
                    tie(index, spacel2, distinfo)
                        = build_ann_full_l2(subQ, szg, dim, M, ef_construction);
                else
                    tie(index, spaceip, distinfo)
                        = build_ann_full_ip(subQ, szg, dim, M, ef_construction);
            }

            auto [locTraj, locAvgW]
                = approximateMSTUsingHNSW_undirected(index, distinfo, szg, dim);

            int localEdgeCount = 0;
            for (auto& e : locTraj) {
                int ni = e.first;
                int pi = e.second;
                int gni = grp[ni];
                int gpi = (pi >= 0 ? grp[pi] : -1);
                fullTrajectory.emplace_back(gni, gpi);
                if (pi >= 0) ++localEdgeCount;
            }
            sumWeights += locAvgW * localEdgeCount;
            sumEdges   += localEdgeCount;

            // cleanup
            delete index;
            if (use_l2) delete spacel2; else delete spaceip;
            delete[] subQ;
        }
    }

    double duration = timing::get_duration(t0);
    double weight_for_root = get_weight_for_root(query, n_query, dim);
    double avgWeight = (sumWeights + (n_query - sumEdges) * weight_for_root) / n_query;

    return { move(fullTrajectory),
             { duration, avgWeight } };
}
