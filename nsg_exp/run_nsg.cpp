

#include "graph_handler.hpp"
#include "../data_handler/data_handler.hpp"
#include "../data_handler/answer_handler.hpp"
#include "../mqo/interface.hpp"
#include "../result_handler/saver.hpp"
#include "process.hpp"

#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 11) { //need fix
        cerr << "Usage: " << argv[0] << " <datapath> <querypath> <answerpath> <gpath> <savepath> "
             << "<approximation_method> <M> <ef> <B> <min_beamsize> <max_beamsize> <increment_ratio>\n";
        return 1;
    }

    // Parse arguments
    string datapath = argv[1];
    string querypath = argv[2];
    string answerpath = argv[3];
    string gpath = argv[4];
    string savepath = argv[5];
    string approximation_method = argv[6];
    int M = stoi(argv[7]);
    int ef = stoi(argv[8]);
    int B = stoi(argv[9]);
    int minbeamsize = stoi(argv[10]);
    int maxbeamsize = stoi(argv[11]);
    float increment_ratio = stof(argv[12]);
    bool use_l2 = (stoi(argv[13]) != 0);

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

    // Run MQO
    auto start = timing::now();
    std::map<std::string, int> params = {{"M", M}, {"ef", ef}, {"B", B}, {"base_only", 0}};
    vector<pair<int, int>> trajectory = mqo(approximation_method, n_query, dim, query, use_l2, params);

    // Evaluation
    csvSaver result_handler(savepath);
    for(int beamsize = minbeamsize ; beamsize <= maxbeamsize; beamsize *= increment_ratio) {
        int topk = 1;
        std::pair<double, double> resNormal;
        if(use_l2)resNormal = get_recall(nsg, query, answers, n_query, dim, topk, beamsize);
        else resNormal = get_recall_ip(nsg, data, query, answers, n_query, dim, topk, beamsize);

        std::pair<double, double> resOpt;
        if(use_l2)resOpt = get_recall_optimized(nsg, query, answers, trajectory, n_query, dim, topk, beamsize);
        else resOpt = get_recall_optimized_ip(nsg, data, query, answers, trajectory, n_query, dim, topk, beamsize);

        result_handler.add(beamsize , {resNormal, resOpt});
    }

    result_handler.save();

    delete nsg;
    delete [] query;
}
