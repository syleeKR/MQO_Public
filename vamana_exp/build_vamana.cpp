
#include "graph_handler.hpp"
#include "../data_handler/data_handler.hpp"


#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 7) {
        cerr << "Usage: " << argv[0]
             << " <datapath> <savepath> <distfn> <M> <L> <alpha>\n";
        return 1;
    }

    string datapath = argv[1];
    string savepath = argv[2];
    string distfn = argv[3];    // "l2" or "mips"
    int M = stoi(argv[4]);
    int L = stoi(argv[5]);
    float alpha = stof(argv[6]);

    string datatype = "float";  // fixed as float
    int n_thread = 40;

    build_vamana(savepath, datapath, datatype, distfn, n_thread, M, L, alpha);
    return 0;
}