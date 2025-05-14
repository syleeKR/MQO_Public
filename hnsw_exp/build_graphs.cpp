
#include "../data_handler/data_handler.hpp"
#include "graph_handler.hpp"
#include <bits/stdc++.h>

using namespace std;



int main(int argc, char** argv)
{
    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " <dpath> <gpath> <use_l2 (0 or 1)> <M> <ef_construction>\n";
        return 1;
    }

    string dpath = argv[1];
    string gpath = argv[2];
    bool use_l2 = (stoi(argv[3]) != 0);
    int M = stoi(argv[4]);
    int ef_construction = stoi(argv[5]);

    int n_data, dim;
    float* data = get_data(dpath, n_data, dim);
    cout << "reading done : " << n_data << " " << dim << endl;

    if (use_l2) build(gpath, data, n_data, dim, M, ef_construction);
    else build_ip(gpath, data, n_data, dim, M, ef_construction);

    delete[] data;
    return 0;
}