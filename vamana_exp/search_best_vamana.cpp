
#include "graph_handler.hpp"
#include "../data_handler/data_handler.hpp"
#include "../data_handler/answer_handler.hpp"
#include "../mqo/interface.hpp"
#include "../result_handler/saver.hpp"
#include "process.hpp"

#include <bits/stdc++.h>
#include <fstream>
#include "../controller/exp2/parameters_vamana.hpp"

using namespace std;

int main(int argc, char** argv)
{
    string datapath = argv[1];
    string querypath = argv[2];
    string answerpath = argv[3];
    string indexpath = argv[4];
    string savepath = argv[5];

    int beamsize = stoi(argv[6]);
    bool use_l2 = (stoi(argv[7]) != 0);
    string data_type = "float";
    string dist_fn = (use_l2) ? "l2" : "mips";



    int n_query, dim;
    float* query = get_data(querypath, n_query, dim);
    vector<vector<int>> answers;
    get_answers(answerpath, answers);
    cout << "reading done : " << n_query << " " << dim << endl;
    auto index_ptr = read_vamana(datapath, indexpath, data_type, dist_fn);




    int topk = 1;
    auto resNormal = get_recall(*index_ptr, query, answers, n_query, dim, topk, beamsize);
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

        int t2i = (use_l2)?0:4;

        auto resOpt = get_recall_optimized(*index_ptr, query, answers, trajectory, n_query, dim, topk, beamsize-t2i);
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


    delete[] query;
}
