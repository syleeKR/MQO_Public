
#include "graph_handler.hpp"
#include "../data_handler/data_handler.hpp"
#include "../data_handler/answer_handler.hpp"
#include "../mqo/interface.hpp"
#include "../result_handler/saver.hpp"

#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 5) {
        cerr << "Usage: " << argv[0]
             << " <datapath> <savepath> <savepath_temp> <use_l2 (0 or 1)>\n";
        return 1;
    }

    string datapath = argv[1];
    string savepath = argv[2];
    string savepath_temp = argv[3];
    bool use_l2 = (stoi(argv[4]) != 0);

    int n, dim;
    float* data = get_data(datapath, n, dim);
    cout << "Data loaded: " << n << " points, dimension: " << dim << endl;

    if (use_l2)
        build_nsg_l2(savepath, savepath_temp, data, n, dim,
                     200, 200, 10, 10, 100,
                     40, 50, 500);
    else
        build_nsg_ip(savepath, savepath_temp, data, n, dim,
                     200, 200, 10, 10, 100,
                     40, 50, 500);

    delete[] data;
    return 0;
}