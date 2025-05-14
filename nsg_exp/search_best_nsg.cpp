
#include "graph_handler.hpp"
#include "../data_handler/data_handler.hpp"
#include "../data_handler/answer_handler.hpp"
#include "../mqo/interface.hpp"
#include "../result_handler/saver.hpp"
#include "process.hpp"

#include <bits/stdc++.h>
#include <fstream>
#include "../controller/exp2/parameters_nsg.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string datapath = argv[1];
    string querypath = argv[2];
    string answerpath = argv[3];
    string gpath = argv[4];
    string savepath = argv[5];

    int beamsize = stoi(argv[6]);
    bool use_l2 = (stoi(argv[7]) != 0);

    // Read query
    int n_query, dim;
    float * query = get_data(querypath, n_query, dim);
    int n_data;
    float * data = get_data(datapath, n_data, dim);
    // Read answer
    vector<vector<int>> answers;
    get_answers(answerpath, answers);
    cout<<"reading done : "<<n_query<<" "<<dim<<endl;

    // Read graph
    // need fix : one more argument please
    efanna2e::IndexNSG* nsg;
    if(use_l2) nsg= read_nsg_l2(datapath, gpath);
    else nsg = read_nsg_ip(datapath, gpath);



    int topk = 1;
    std::pair<double, double> resNormal;
    if(use_l2)resNormal = get_recall(nsg, query, answers, n_query, dim, topk, beamsize);
    else resNormal = get_recall_ip(nsg, data, query, answers, n_query, dim, topk, beamsize);
    cout<<"recall : "<<resNormal.first<<" original time : "<<resNormal.second<<endl;


    vector<pair<int, int>> candidate_params = get_candidate_params(n_query, datapath);
    double best_time = 1000000;
    pair<double, double> best_params = candidate_params[0];
    double best_mst_time = -1;
    double best_query_time = -1;
    double recall_value = -1;

    
    for (auto params : candidate_params)
    {
        map<string, int> params_input;
        params_input["B"] = params.first;
        params_input["M"] = params.second;
        string method = (params_input["M"]!=-1) ? "mixed_modified" : "lsh";
        pair<vector<pair<int, int>>, pair<double, double>> mqo_res = mqo_with_info(method, n_query, dim, query, use_l2, params_input);
        vector<pair<int, int>> trajectory = mqo_res.first;
        double mst_time = mqo_res.second.first;

        int t2i = (use_l2)?0:3;

        std::pair<double, double> resOpt;
        if(use_l2)resOpt = get_recall_optimized(nsg, query, answers, trajectory, n_query, dim, topk, beamsize);
        else resOpt = get_recall_optimized_ip(nsg, data, query, answers, trajectory, n_query, dim, topk, beamsize-t2i);

        double query_time = resOpt.second;

        double total_time = mst_time + query_time;
        if(total_time < best_time)
        {
            best_time = total_time;
            best_params = params;
            best_mst_time = mst_time;
            best_query_time = query_time;
            recall_value = resOpt.first;
        }
        cout<<"recall : "<< resOpt.first<<" total time : "<<total_time<<endl;
    }

    double speedup = resNormal.second / (best_mst_time + best_query_time);
    cout<<"best speedup : "<<speedup<<endl;
    save_result_to_csv(savepath, resNormal, best_params, best_mst_time, best_query_time, speedup, recall_value);



    delete nsg;
    delete [] query;
}
