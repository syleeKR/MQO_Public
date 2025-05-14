
#include <bits/stdc++.h>
#include "../data_handler.hpp"
using namespace std;


/*
 g++ -std=c++17 -O3 -march=native -o subsample_random subsample_random.cpp
*/ 
int main()
{
    string fulldata = "some path goes here";
    string savepath = "some path goes here";

    int start = 20000000; //110M
    int end =   49000000; // 120M
    int samplesize = 10000000; // 10M
    
    sample_and_save_subset(fulldata, start, end, samplesize, savepath);
}