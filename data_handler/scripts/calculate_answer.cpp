
#include <bits/stdc++.h>
#include "../answer_handler.hpp"
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " <dpath> <apath> <qpath> <use_l2 (0 or 1)>\n";
        return 1;
    }

    string dpath = argv[1];
    string apath = argv[2];
    string qpath = argv[3];
    bool use_l2_distance = (stoi(argv[4]) != 0);

    int topk = 10;

    save_answers(apath, dpath, qpath, topk, use_l2_distance);
}