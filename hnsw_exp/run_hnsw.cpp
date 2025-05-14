#include "hnswlib/hnswlib.h"
#include "process.hpp"
#include "../data_handler/data_handler.hpp"
#include "../data_handler/answer_handler.hpp"
#include "graph_handler.hpp"

#include "../mqo/interface.hpp"

#include "../result_handler/saver.hpp"

#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main(int argc, char * argv[]) 
{
    if (argc < 13) {
        std::cerr << "Usage: " << argv[0]
                  << " <datapath> <querypath> <answerpath> <graphpath> <savepath> "
                  << "<approx_method> <M> <ef> <B> <min_beamsize> <max_beamsize> <increment_ratio>\n";
        return 1;
    }

    string datapath = argv[1];      // Unused in this HNSW version
    string querypath = argv[2];
    string answerpath = argv[3];
    string graphpath = argv[4];
    string savepath = argv[5];
    string approx_method = argv[6]; // Should be "ann"
    int M = stoi(argv[7]);
    int ef = stoi(argv[8]);
    int B = stoi(argv[9]);          // Unused, kept for compatibility
    float min_beamsize = stof(argv[10]);
    float max_beamsize = stof(argv[11]);
    float increment_ratio = stof(argv[12]);
    bool use_l2 = (stoi(argv[13]) != 0);

    // Load data
    int dim, n_query;
    float * query = get_data(querypath , n_query, dim);
    vector<vector<int>> answers;
    get_answers(answerpath, answers);
    cout << "reading done : " << n_query << " " << dim << endl;

    // Load HNSW graph
    hnswlib::HierarchicalNSW<float>* alg_hnsw;
    hnswlib::L2Space * spacel2;
    hnswlib::InnerProductSpace * spaceinner;
    if(use_l2)
    {
        auto res = read(graphpath, dim);
        alg_hnsw = res.first;
        spacel2 = res.second;
    }
    else
    {
        auto res = read_ip(graphpath, dim);
        alg_hnsw = res.first;
        spaceinner = res.second;
    }


    // MQO Trajectory
    std::map<std::string, int> params = {{"M", M}, {"ef", ef}, {"B", B}, {"base_only", 0}};
    vector<pair<int, int>> trajectory = mqo(approx_method, n_query, dim, query, use_l2, params);

    // Result save
    csvSaver result_handler(savepath);
    int topk = 1;

    for (float beamsize = min_beamsize; beamsize <= max_beamsize; beamsize *= increment_ratio) {
        auto resNormal = get_recall(alg_hnsw, query, answers, n_query, dim, topk, beamsize);
        auto resOpt = get_recall_optimized(alg_hnsw, query, answers, trajectory, n_query, dim, topk, beamsize);

        result_handler.add(beamsize , {resNormal, resOpt});

    }

    result_handler.save();

    delete [] query;
    delete alg_hnsw;
    delete spacel2;
    delete spaceinner;
}
