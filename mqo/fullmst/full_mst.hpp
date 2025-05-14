// Compile with: g++ -O3 -march=native -fopenmp -std=c++17
#pragma once
#include <vector>
#include <utility>
#include <chrono>
#include <cmath>
#include <limits>
#include <omp.h>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
using namespace std;

// Returns: { edges (child,parent) }, { total_time_sec, avg_edge_weight }
pair<vector<pair<int,int>>, pair<double,double>>
get_mst(int n_query, int dim, float* query, bool use_l2, int n_core) {
    // visited flag, best known dist, and parent for each node
    vector<char>   visited(n_query, 0);
    vector<double> dist   (n_query, numeric_limits<double>::infinity());
    vector<int>    parent (n_query, -1);
    vector<pair<int,int>> edges;
    edges.reserve(n_query > 0 ? n_query - 1 : 0);

    // Start from node 0
    dist[0] = 0.0;
    double totalWeight = 0.0;

    auto t0 = chrono::high_resolution_clock::now();

    for (int it = 0; it < n_query; ++it) {
        // 1) Find the next unvisited node u with minimal dist[u]
        int   u = -1;
        double best_d = numeric_limits<double>::infinity();
        for (int i = 0; i < n_query; ++i) {
            if (!visited[i] && dist[i] < best_d) {
                best_d = dist[i];
                u = i;
            }
        }
        if (u < 0) break;
        visited[u] = 1;

        // 2) Record edge (u,parent[u]) if not root, and accumulate weight
        if (parent[u] != -1) {
            double w = use_l2 ? sqrt(best_d) : best_d;
            totalWeight += w;
            edges.emplace_back(u, parent[u]);
        }

        // 3) Parallel distance‐update: for every unvisited j, relax via u
        #pragma omp parallel for schedule(static) num_threads(n_core)
        for (int j = 0; j < n_query; ++j) {
            if (!visited[j]) {
                float* x = query + size_t(u) * dim;
                float* y = query + size_t(j) * dim;
                double cost = 0.0;

                if (use_l2) {
                    // accumulate squared L2
                    for (int d = 0; d < dim; ++d) {
                        double diff = x[d] - y[d];
                        cost += diff * diff;
                    }
                } else {
                    // inner‐product distance = 1 – <x,y>
                    double dot = 0.0;
                    for (int d = 0; d < dim; ++d) {
                        dot += double(x[d]) * y[d];
                    }
                    cost = 1.0 - dot;
                }

                // relax
                if (cost < dist[j]) {
                    dist[j]   = cost;
                    parent[j] = u;
                }
            }
        }
    }

    auto t1 = chrono::high_resolution_clock::now();
    double duration = chrono::duration<double>(t1 - t0).count();
    double avg_edge = (n_query > 1 ? totalWeight / (n_query - 1) : 0.0);

    return { edges, { duration, avg_edge } };
}


void save_mst(const vector<pair<int,int>>& edges, const string& filename) {
    ofstream out(filename);
    if (!out) {
        throw runtime_error("Unable to open file for writing: " + filename);
    }
    out << edges.size() << '\n';
    for (const auto& pr : edges) {
        out << pr.first << ' ' << pr.second << '\n';
    }
}

// Read MST edges from file created by save_mst:
// Returns vector of (child, parent) pairs.
vector<pair<int,int>> read_mst(const string& filename) {
    ifstream in(filename);
    if (!in) {
        throw runtime_error("Unable to open file for reading: " + filename);
    }
    size_t m;
    in >> m;
    vector<pair<int,int>> edges;
    edges.reserve(m);
    for (size_t i = 0; i < m; ++i) {
        int child, parent;
        in >> child >> parent;
        edges.emplace_back(child, parent);
    }
    return edges;
}