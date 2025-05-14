#pragma once

#include "bits/stdc++.h"
#include "lsh_mst_opt.hpp"
#include "ann_mst_opt.hpp"
#include "reorder.hpp"
#include "checker.hpp"
#include "fullmst/full_mst.hpp"
#include "mixed_mst_opt.hpp"

using namespace std;



vector<pair<int, int>> mqo_no_reorder(string option, int n_query, int dim, float * query, bool use_l2, map<string, int> & params, bool verbose=true)
{

    pair<vector<pair<int, int>>, pair<double, double>> res;
    
    if (option == "lsh")res = optimize_trajectory_lsh_mst(query, n_query, dim, use_l2, params);
    else if (option == "ann")res = optimize_trajectory_ann_mst(query, n_query, dim, params);   // not supported
    else if (option == "mixed")res = optimize_trajectory_mixed_mst(query, n_query, dim, params);   // not supported
    else if (option == "mixed_modified")res = optimize_trajectory_mixed_modified_mst(query, n_query, dim, use_l2, params);
    else {
        vector<pair<int, int>> custom_trajectory = read_mst(option);
        custom_trajectory.push_back({0,-1});
        pair<double, double> time_weight_info = {0,0};
        res = {custom_trajectory, time_weight_info};
    }


    if (verbose)cout<<"duration : "<<res.second.first<<" "<<"avg mst dist : "<<res.second.second<<endl;

    vector<pair<int, int>> trajectory = res.first;
    //checkTrajectory(trajectory, n_query);

    return trajectory;
}


/*
    returns vector of {child, parent}
    if parent == -1 , it is the root of a new tree

*/
vector<pair<int, int>> mqo(string option, int n_query, int dim, float * query, bool use_l2, map<string, int> & params, bool verbose=true)
{

    pair<vector<pair<int, int>>, pair<double, double>> res;
    
    if (option == "lsh")res = optimize_trajectory_lsh_mst(query, n_query, dim, use_l2, params);
    else if (option == "ann")res = optimize_trajectory_ann_mst(query, n_query, dim, params);   // not supported
    else if (option == "mixed")res = optimize_trajectory_mixed_mst(query, n_query, dim, params);   // not supported
    else if (option == "mixed_modified")res = optimize_trajectory_mixed_modified_mst(query, n_query, dim, use_l2, params);
    else {
        vector<pair<int, int>> custom_trajectory = read_mst(option);
        custom_trajectory.push_back({0,-1});
        pair<double, double> time_weight_info = {0,0};
        res = {custom_trajectory, time_weight_info};
    }


    if (verbose)cout<<"duration : "<<res.second.first<<" "<<"avg mst dist : "<<res.second.second<<endl;

    vector<pair<int, int>> trajectory = res.first;

    auto start_reordering = timing::now();
    reorder_multiple_DFS(trajectory);
    double time_for_reordering = timing::get_duration(start_reordering);
    if (verbose)cout<<"time for reordering : "<<time_for_reordering<<endl;

    checkTrajectory(trajectory, n_query);

    return trajectory;
}


/*
    returns the following pair 
    first :  vector of {child, parent}
    second : {duration to optimize, average edge distance value}
    if parent == -1 , it is the root of a new tree
*/
pair<vector<pair<int, int>>, pair<double, double>> mqo_with_info(string option, int n_query, int dim, float * query, bool use_l2, map<string, int> & params, bool verbose=false)
{

    pair<vector<pair<int, int>>, pair<double, double>> res;
    
    if (option == "lsh")res = optimize_trajectory_lsh_mst(query, n_query, dim, use_l2, params);
    else if (option == "ann")res = optimize_trajectory_ann_mst(query, n_query, dim, params);   // not supported
    else if (option == "mixed")res = optimize_trajectory_mixed_mst(query, n_query, dim, params);   // not supported
    else if (option == "mixed_modified")res = optimize_trajectory_mixed_modified_mst(query, n_query, dim, use_l2, params);
    else {
        vector<pair<int, int>> custom_trajectory = read_mst(option);
        custom_trajectory.push_back({0,-1});
        pair<double, double> time_weight_info = {0,0};
        res = {custom_trajectory, time_weight_info};
    }

    if (verbose)cout<<"duration : "<<res.second.first<<" "<<"avg mst dist : "<<res.second.second<<endl;

    vector<pair<int, int>> trajectory = res.first;

    auto start_reordering = timing::now();
    reorder_multiple_DFS(trajectory);
    double time_for_reordering = timing::get_duration(start_reordering);
    res.second.first += time_for_reordering;
    if (verbose) cout<<"time for reordering : "<<time_for_reordering<<endl;

    checkTrajectory(trajectory, n_query);

    return {std::move(trajectory), res.second};
}