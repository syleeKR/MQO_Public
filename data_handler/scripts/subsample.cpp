
#include <bits/stdc++.h>
#include "../data_handler.hpp"
using namespace std;



void sample_and_save_norandom(const string& filepath, int samplesize, const string& savepath) {
    ifstream fin(filepath, ios::binary);
    if (!fin) throw runtime_error("Cannot open input file: " + filepath);

    int n, dim;
    fin.read((char*)&n, sizeof(int));
    fin.read((char*)&dim, sizeof(int));

    if (n < samplesize) throw runtime_error("Requested sample size exceeds dataset size.");

    bool is_float = filepath.find(".fbin") != string::npos;
    ofstream fout(savepath, ios::binary);
    if (!fout) throw runtime_error("Cannot open output file: " + savepath);

    // Write new header
    fout.write((char*)&samplesize, sizeof(int));
    fout.write((char*)&dim, sizeof(int));

    size_t vecsize = (size_t)samplesize * dim;

    if (is_float) {
        vector<float> buffer(vecsize);
        fin.read((char*)buffer.data(), vecsize * sizeof(float));
        fout.write((char*)buffer.data(), vecsize * sizeof(float));
    } else {
        vector<uint8_t> buffer(vecsize);
        fin.read((char*)buffer.data(), vecsize * sizeof(uint8_t));
        fout.write((char*)buffer.data(), vecsize * sizeof(uint8_t));
    }

    cout << "Saved " << samplesize << " vectors to " << savepath << endl;
}


/*
g++ -std=c++17 -O3 -march=native -o subsample subsample.cpp
*/
int main()
{
    string commonpath = "some path goes here";
    string fulldata = "some path goes here";

    // int samplesize = 100000000; //100M
    // int samplesize = 10000000;  //10M

    string savepath1 = commonpath + "query300K.fbin";
    int samplesize1 = 300000;   
    sample_and_save_norandom(fulldata, samplesize1, savepath1);

    string savepath2 = commonpath + "query100K.fbin";
    int samplesize2 = 100000;   
    sample_and_save_norandom(fulldata, samplesize2, savepath2);

    string savepath3 = commonpath + "query30K.fbin";
    int samplesize3 = 30000;   
    sample_and_save_norandom(fulldata, samplesize3, savepath3);

    string savepath4 = commonpath + "query10K.fbin";
    int samplesize4 = 10000;   
    sample_and_save_norandom(fulldata, samplesize4, savepath4);

    string savepath5 = commonpath + "query1K.fbin";
    int samplesize5 = 1000;   
    sample_and_save_norandom(fulldata, samplesize5, savepath5);

    string savepath6 = commonpath + "query100.fbin";
    int samplesize6 = 100;   
    sample_and_save_norandom(fulldata, samplesize6, savepath6);
}