
#pragma once

#include <bits/stdc++.h>
#include "basic.hpp"

using namespace std;


vector<pair<int, int>> reorder_single_DFS(vector<pair<int, int>> &edges) {
    int n = edges.size();
    vector<vector<int>> children(n);
    int root = -1;

    for (const auto &edge : edges) {
        int node = edge.first;
        int parent = edge.second;
        if (parent == -1)root = node;
        else children[parent].push_back(node);
    }

    vector<pair<int, int>> result;
    result.reserve(n);

    stack<pair<int, int>> s;
    s.push({root, -1});
    
    while (!s.empty()) {
        auto [node, parent] = s.top(); s.pop();
        result.push_back({node, parent});
        
        const auto &childList = children[node];
        for (int i = childList.size() - 1; i >= 0; --i)s.push({childList[i], node});  
    }
    
    return result;
}


// vector<pair<int, int>> reorder_multiple_DFS(vector<pair<int, int>> &edges) {
//     int n = edges.size();
//     vector<vector<int>> children(n);
//     vector<int> roots;

//     for (const auto &edge : edges) {
//         int node = edge.first; int parent = edge.second;
//         if (parent == -1)roots.push_back(node);
//         else children[parent].push_back(node);
//     }

//     vector<pair<int, int>> result;
//     result.reserve(n);

//     for (int root : roots) {
//         stack<pair<int, int>> s;
//         s.push({root, -1});
//         while (!s.empty()) {
//             auto [node, parent] = s.top();
//             s.pop();
//             result.push_back({node, parent});
            
//             const auto &childList = children[node];
//             for (int i = childList.size() - 1; i >= 0; --i)s.push({childList[i], node});
//         }
//     }
    
//     return result;
// }

void reorder_multiple_DFS(vector<pair<int, int>>& edges) {
    int n = edges.size();
    vector<vector<int>> children(n);
    vector<int> roots;

    for (const auto& edge : edges) {
        int node = edge.first;
        int parent = edge.second;
        if (parent == -1) roots.push_back(node);
        else children[parent].push_back(node);
    }

    vector<pair<int, int>> reordered;
    reordered.reserve(n);

    for (int root : roots) {
        stack<pair<int, int>> s;
        s.push({root, -1});
        while (!s.empty()) {
            auto [node, parent] = s.top();
            s.pop();
            reordered.push_back({node, parent});

            const auto& childList = children[node];
            for (int i = childList.size() - 1; i >= 0; --i)
                s.push({childList[i], node});
        }
    }

    edges = std::move(reordered); 
}




vector<pair<int, int>> reorder_single_optimized(vector<pair<int, int>> &edges, float* query, int dim, float alpha=1.05) {
    int n = edges.size();
    vector<vector<int>> children(n);
    int root = -1;

    for (const auto &edge : edges) {
        int node = edge.first;
        int parent = edge.second;
        if (parent == -1)root = node;
        else children[parent].push_back(node);
    }

    vector<pair<int, int>> result;
    result.reserve(n);

    stack<pair<int, int>> s; s.push({root, -1});

    int prev=-1;
    int count = 0;
    while (!s.empty()) {
        auto [node, parent] = s.top(); s.pop();

        if (parent == -1)result.push_back({node, parent});
        else if(get_distance(query, dim, node, parent)*alpha > get_distance(query, dim, node, prev)){result.push_back({node, prev});count++;}
        else result.push_back({node, parent});
        prev = node;
        
        const auto &childList = children[node];
        for (int i = childList.size() - 1; i >= 0; --i)s.push({childList[i], node});  
    }
    
    cout<<"num flattened : "<<count<<endl;
    return result;
}

vector<pair<int, int>> reorder_multiple_optimized(vector<pair<int, int>> &edges, float* query, int dim, float alpha=1.05) {
    int n = edges.size();
    vector<vector<int>> children(n);
    vector<int> roots;

    // Build children lists and identify roots.
    for (const auto &edge : edges) {
        int node = edge.first;
        int parent = edge.second;
        if (parent == -1) roots.push_back(node);
        else children[parent].push_back(node);
    }

    vector<pair<int, int>> result; result.reserve(n);

    int count = 0;

    // Process each tree in the forest.
    for (int root : roots) {
        stack<pair<int, int>> s;
        s.push({root, -1});
        int prev = -1;  // Reset prev for each separate DFS.
        while (!s.empty()) {
            auto [node, parent] = s.top();
            s.pop();

            if (parent == -1)
                result.push_back({node, parent});
            else if (get_distance(query, dim, node, parent) * alpha > get_distance(query, dim, node, prev)) {
                result.push_back({node, prev});
                count++;
            } else {
                result.push_back({node, parent});
            }
            prev = node;

            const auto &childList = children[node];
            for (int i = childList.size() - 1; i >= 0; --i)
                s.push({childList[i], node});
        }
    }

    cout << "num flattened : " << count << endl;
    return result;
}

