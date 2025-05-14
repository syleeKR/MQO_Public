#pragma once
#include <bits/stdc++.h>
#include "hnswlib/hnswalg.h"   

using namespace std;




namespace mqohnswlib {

    template <typename dist_t>
    std::vector<std::pair<int, float>> get_neighbors(
        const HierarchicalNSW<dist_t>& index,
        map<pair<int, int>, float> & distinfo,  // not using 
        tableint v,
        bool collect_all_levels = false)
    {
        char* dataV = index.getDataByInternalId(v);
        // goingto store (neighbor_id, distance)
        std::vector<std::pair<int, float>> result;
    
    
    
        int level = 0;
        linklistsizeint* ll_cur = index.get_linklist_at_level(v, level);
        int count = index.getListCount(ll_cur);
        tableint* neighbors = reinterpret_cast<tableint*>(ll_cur + 1);
    
        for (int i = 0; i < count; i++)
        {
            tableint nbId = neighbors[i];
    
            //Distance is computed on the fly:
            float dist = index.fstdistfunc_(dataV,
                                            index.getDataByInternalId(nbId),
                                            index.dist_func_param_);
            
            //float dist = distinfo[{min(v, nbId), max(v,nbId)}];
            dist = sqrt(dist);
            result.push_back({static_cast<int>(nbId), dist});
        }
    
        return result;
    }
    
} 



pair<std::vector<pair<int, int>>, double> approximateMSTUsingHNSW(
    mqohnswlib::HierarchicalNSW<float>* index,
    //std::unordered_map<std::pair<int,int>, float, PairHash> & distinfo,
    map<pair<int, int>, float> & distinfo,
    int n_query,
    int dim)
{
    std::vector<pair<int, int>> mstEdges;
    mstEdges.reserve(n_query);

    std::vector<bool> visited(n_query, false);

    double totalDist = 0.0;  // Sum dist
    int connectedComponents = 0; // how many separate components we find
    int visitedCount = 0;        // how many distinct points we have added

    using Edge = std::tuple<float,int,int>; // (dist, node, parentInMST)
    auto cmp = [](const Edge& a, const Edge& b) {
        return std::get<0>(a) > std::get<0>(b); // min-hea
    };
    std::priority_queue<Edge, std::vector<Edge>, decltype(cmp)> pq(cmp);

    // Prim
    for(int start = 0; start < n_query; start++) {
        if (!visited[start]) {
            connectedComponents++;
            visited[start] = true;
            visitedCount++;
            mstEdges.push_back({start, -1});

            // Push its adjacency into the PQ
            auto neighbors = mqohnswlib::get_neighbors(*index, distinfo, (mqohnswlib::tableint)start, true);
            for (auto& [nbId, dist] : neighbors) {
                // Only push if neighbor is not visited
                if (!visited[nbId]) {
                    pq.push({dist, nbId, start});
                }
            }

            while (!pq.empty()) {
                auto [dist, node, parent] = pq.top();
                pq.pop();

                if (!visited[node]) {
                    visited[node] = true;
                    visitedCount++;

                    mstEdges.push_back({node, parent});
                    totalDist += dist;

                    auto nb2 = mqohnswlib::get_neighbors(*index, distinfo, (mqohnswlib::tableint)node, true);
                    for (auto& [nbId, dist2] : nb2) {
                        if (!visited[nbId]) {
                            pq.push({dist2, nbId, node});
                        }
                    }
                }
            }
        }
    }


    int mstEdgesUsed = (n_query - connectedComponents);
    double avgDist = 0.0;
    if (mstEdgesUsed > 0) {
        avgDist = totalDist / mstEdgesUsed;
    }

    std::cout << "Approx MST edges: " << mstEdgesUsed
              << " (forming " << connectedComponents
              << " component(s)). Average MST distance: " << avgDist << "\n";

    return {std::move(mstEdges), avgDist};  // size == n_query
}



struct PQItem {
    float w;
    int  v;
    int  p;
    bool operator<(PQItem const& o) const { 
      return w > o.w; 
    }
  };

  
pair<std::vector<pair<int, int>>, double> approximateMSTUsingHNSW_undirected(
    mqohnswlib::HierarchicalNSW<float>* index,
    //std::unordered_map<std::pair<int,int>, float, PairHash> & distinfo,
    map<pair<int, int>, float> & distinfo,
    int n_query,
    int dim)
{
    vector<vector<pair<int,float>>> adj(n_query); adj.reserve(n_query);
    for(auto &v : adj) v.reserve(10);      
    
    for (int v = 0; v < n_query; ++v) {
        auto nbrs = get_neighbors(*index, distinfo, v, /*all_levels=*/true);

        for (auto &pr : nbrs) {
            int u    = pr.first;
            float d  = pr.second;
            if (u > v) {
                adj[v].emplace_back(u, d);
                adj[u].emplace_back(v, d);
            }
        }
    }

    std::vector<char> visited(n_query, 0);
    std::vector<std::pair<int,int>> mstEdges;
    mstEdges.reserve(n_query);

    std::vector<PQItem> buf;
    buf.reserve(n_query * 4);  
    std::priority_queue<PQItem, std::vector<PQItem>> pq(std::less<PQItem>(), std::move(buf));

    double totalDist = 0.0;
    int components = 0;

    for (int start = 0; start < n_query; ++start) {
        if (visited[start]) continue;
        components++;
        pq.push({0.0f, start, -1});
        while (!pq.empty()) {
            auto it = pq.top(); 
            pq.pop();
            int v = it.v;
            if (visited[v]) continue;
            visited[v] = 1;
            mstEdges.emplace_back(v, it.p);
            totalDist += it.w;
            for (auto &e : adj[v]) {
                int u = e.first;
                if (!visited[u]) 
                    pq.push({ e.second, u, v });
            }
        }
    }


    int usedEdges = n_query - components;
    double avgDist = (usedEdges > 0 ? totalDist / usedEdges : 0.0);


    return { move(mstEdges), avgDist };
}
