#include "full_mst.hpp"
#include "../../data_handler/data_handler.hpp"

#include <vector>
#include <utility>
#include <fstream>
#include <string>
using namespace std;


/*
g++ -O3 -march=native -fopenmp -std=c++17 -o build_full_mst build_full_mst.cpp
*/

int main(int argc, char** argv) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0]
             << " <querypath> <use_l2 (0|1)> <n_core> <savepath>\n";
        return 1;
    }

    string querypath = argv[1];
    bool   use_l2    = (stoi(argv[2]) != 0);
    int    n_core    = stoi(argv[3]);
    string savepath  = argv[4];

    int n_query, dim;
    float* query = get_data(querypath, n_query, dim);

    auto result = get_mst(n_query, dim, query, use_l2, n_core);
    auto& edges       = result.first;
    double duration   = result.second.first;
    double avg_weight = result.second.second;

    //save_mst(edges, savepath);
    cout<<"number of queries : "<<n_query<<endl;
    cout << "MST done in " << duration << " s, avg edge = "
         << avg_weight << "\n";

    return 0;
}