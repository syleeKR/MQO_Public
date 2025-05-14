
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
    if (argc < 15) {
        std::cerr << "Usage: " << argv[0]
                  << " <datapath> <querypath> <answerpath> <indexpath> <savepath> "
                  << "<approx_method> <M> <ef> <B> <min_beamsize> <max_beamsize> <increment_ratio> "
                  << "<data_type> <dist_fn>\n";
        return 1;
    }

    string datapath = argv[1];
    string querypath = argv[2];
    string answerpath = argv[3];
    string indexpath = argv[4];
    string savepath = argv[5];
    string approx_method = argv[6];
    int M = stoi(argv[7]);
    int ef = stoi(argv[8]);
    int B = stoi(argv[9]);
    int min_beamsize = stoi(argv[10]);
    int max_beamsize = stoi(argv[11]);
    float increment_ratio = stof(argv[12]);
    string data_type = argv[13];
    bool use_l2 = (stoi(argv[14]) != 0);


    string dist_fn = (use_l2) ? "l2" : "mips";
    int n_query, dim;
    float* query = get_data(querypath, n_query, dim);

    vector<vector<int>> answers;
    get_answers(answerpath, answers);
    cout << "reading done : " << n_query << " " << dim << endl;

    auto index_ptr = read_vamana(datapath, indexpath, data_type, dist_fn);

    std::map<std::string, int> params = {{"M", M}, {"ef", ef}, {"B", B}, {"base_only", 0}};
    vector<pair<int, int>> trajectory = mqo(approx_method, n_query, dim, query, use_l2, params);

    csvSaver result_handler(savepath);
    int topk = 1;

    for (int beamsize = min_beamsize; beamsize <= max_beamsize; beamsize *= increment_ratio) {
        auto resNormal = get_recall(*index_ptr, query, answers, n_query, dim, topk, beamsize);
        auto resOpt = get_recall_optimized(*index_ptr, query, answers, trajectory, n_query, dim, topk, beamsize);
        result_handler.add(beamsize, {resNormal, resOpt});
    }

    result_handler.save();

    delete[] query;
}
