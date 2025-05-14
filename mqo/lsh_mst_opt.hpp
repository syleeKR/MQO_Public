#pragma once

#include "basic.hpp"
#include "../utils/time.hpp"

#include <bits/stdc++.h>
#define REP0(i,n) for(int i=0; i<n; i++)
#define REP1(i,n) for(int i=1; i<=n; i++)
#define REP(i,a,b) for(int i=a; i<=b; i++)
#define sz(v) ((int)(v).size())
#define pb push_back
#define fi first
#define se second
#define pii pair<int, int>
#define pll pair<ll, ll>
#define vint vector<int>
#define vll vector<ll>
#define vpii vector<pair<int, int>>
#define vpll vector<pair<ll,ll>>
using namespace std;

//---------------------------------------------------------
struct GroupInfo {
    vint group;
    int size; // Store the group size ....s
};
struct GroupCompare {
    bool operator()(const GroupInfo& a, const GroupInfo& b) const {
    return a.size < b.size;
}
};

//---------------------------------------------------------


float * generateRandomHyperplane(int B, int rep, int dim, int seed=42) {

    float * hyperplane = new float[(B-1) * rep * dim];

    std::mt19937 gen(seed);
    std::normal_distribution<float> normDist(0.0f, 1.0f);

    for(int i = 0; i<(B-1)*rep*dim; i++)hyperplane[i] = normDist(gen);
    return hyperplane;
}
float get_innerproduct(const float * query, const float * hyperplane, int dim, int qindex, int hindex)
{
    float sum = 0.0;
    int qstart = dim * qindex;
    int hstart = dim * hindex;
    for(int i=0; i<dim; i++)sum += query[qstart + i] * hyperplane[hstart+i]; 
    return sum;
}

float get_quality(vint & left, vint & right, const float * query, int dim, float alpha, int n_quality_calc = 300, int seed=42)
{
    static std::mt19937 gen(seed); 
    std::uniform_int_distribution<size_t> distLeft(0, left.size() - 1);
    std::uniform_int_distribution<size_t> distRight(0, right.size() - 1);

    float mindist = INT_MAX;
    int numSamples = n_quality_calc;

    for (int i = 0; i < numSamples; ++i) {
        size_t lIndex = distLeft(gen);
        size_t rIndex = distRight(gen);

        mindist = min(mindist, get_distance(query, dim, left[lIndex], right[rIndex]));
    }

    float imbalance = (float)abs(sz(left) - sz(right))/(sz(left) + sz(right));
    float score = mindist - alpha * imbalance;

    return score;
}
float get_variance(priority_queue<GroupInfo, vector<GroupInfo>, GroupCompare> & pq)
{
    vector<int> res;
    while(!pq.empty())
    {
        int x = pq.top().size; res.push_back(x); pq.pop();
    }
    float sumX = 0.0; float sumX2=0.0;
    for(auto x : res)
    {
        sumX += x;
        sumX2 += x*x;
    }
    return sumX2 / sz(res) - (sumX/sz(res))*(sumX/sz(res)) ; 
}



pair<vint, vint> splitgroup(const vint& group, 
                            const float* query, 
                            const float* hyperplane, 
                            int dim, 
                            int split_num
                            )  
{

    int hindex = split_num;
    vint left;
    vint right;
    left.reserve(group.size());
    right.reserve(group.size());

    for (int qindex : group) {
        float sum = get_innerproduct(query, hyperplane, dim, qindex, hindex);
        if (sum < 0.0f)left.push_back(qindex);
        else right.push_back(qindex);
    }
    return {move(left), move(right)};
}

priority_queue<GroupInfo, vector<GroupInfo>, GroupCompare>
split(const float* query, const float* hyperplane, int n_query, int dim, int B)
{
    // initialize.
    priority_queue<GroupInfo, vector<GroupInfo>, GroupCompare> pq;
    vint initial(n_query);
    for (int i = 0; i < n_query; ++i)initial[i] = i;
    pq.push({ std::move(initial), n_query});

    //split B times
    for (int split_num = 0; split_num < B - 1; split_num++) {
        
        GroupInfo groupInfo = pq.top();  pq.pop();
        auto splitRes = splitgroup(groupInfo.group, query, hyperplane, dim, split_num);
        int lsize = sz(splitRes.first); int rsize = sz(splitRes.second);
        if(lsize > 0)pq.push({ std::move(splitRes.first), lsize });
        if(rsize > 0)pq.push({ std::move(splitRes.second), rsize });
    }
    return pq;
}

pair< vector<pair<int, int>>, double> computeMSTs(float* query, int n_query, int dim, bool use_l2, priority_queue<GroupInfo, vector<GroupInfo>, GroupCompare>& pq)
{
    double totalMSTWeight = 0.0;
    long long totalEdges = 0;
    vector<pii> trajectory; trajectory.reserve(n_query);
    
    // for each group run Prim
    while (!pq.empty()) {
        vint group = std::move(pq.top().group);
        pq.pop();
        
        const int n = group.size();
        if (n == 1) {
            trajectory.push_back({ group[0], -1 });
            continue;
        }
        
        vector<pair<float, int>> key(n, {numeric_limits<float>::infinity(), -1});
        vector<bool> inMST(n, false);
        key[0] = {0.0f, -1};  
        
        float groupMSTWeight = 0.0f;
        
        for (int i = 0; i < n; i++) {
            int curr = -1;
            int parent =-1;
            float best = numeric_limits<float>::infinity();
            
            // get edge to add
            for (int j = 0; j < n; j++) {
                if (!inMST[j] && key[j].first < best) {
                    best = key[j].first;
                    curr = j;
                    parent = key[j].second;
                }
            }
            inMST[curr] = true;
            groupMSTWeight += key[curr].first;

            int pp = parent!=-1?group[parent]:-1;
            trajectory.pb({group[curr], pp});
            
            for (int v = 0; v < n; v++) {
                if (!inMST[v]) {
                    float d;
                    if(use_l2)d = get_distance(query, dim, group[curr], group[v]);
                    else d = get_distance_ip(query, dim, group[curr], group[v]);
                    
                    if (d < key[v].first){key[v].first = d; key[v].second = curr;}
                }
            }
        }
        totalMSTWeight += groupMSTWeight;
        totalEdges += (n - 1);
    }
    
    return {std::move(trajectory), (totalMSTWeight + (n_query - totalEdges) * get_weight_for_root(query, n_query, dim)) /n_query};

}


pair<vector<pii>, pair<double, double>> optimize_trajectory_lsh_mst(float * query, int n_query, int dim, bool use_l2, map<string, int> & params)
{
    // hyperplanes of size 
    int B = params["B"];
    float *  hyperplane = generateRandomHyperplane(B,1, dim);

    auto start = timing::now();
    priority_queue<GroupInfo, vector<GroupInfo>, GroupCompare> pq = split(query, hyperplane, n_query, dim, B);
    pair<vector<pii>, double> trajectory = computeMSTs(query, n_query, dim, use_l2, pq);
    double duration = timing::get_duration(start);


    return {move(trajectory.first), {duration, trajectory.second}};
}


