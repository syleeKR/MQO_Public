#pragma once

#include <bits/stdc++.h>
#include "calculator.hpp"
#include "data_handler.hpp"
#include "omp.h"
using namespace std;


vector<vector<int>> calculate_answers(float * data, float * query, int dim, int n_query, int n_data, int topk, bool l2=  true)
{
    vector<vector<int>> answers;

    answers.resize(n_query);

    omp_set_num_threads(40);

    #pragma omp parallel for
    for(int i=0; i<n_query; i++)
    {
        int tid = omp_get_thread_num();

        if(tid ==0 ) cout<<"query : "<<i<<endl;
        // we will make vector<int> of length k sorted.
        vector<int> answer_for_query(topk);

        priority_queue<pair<float, int>> pq;

        for(int dataindex = 0; dataindex<n_data; dataindex ++)
        {
            float distwithquery;
            if(l2) distwithquery = get_distance_data_query(data, query, dim, dataindex, i);
            else distwithquery = get_distance_data_query_ip(data, query, dim, dataindex, i);
            pq.push({distwithquery, dataindex});
            if(pq.size() > topk)pq.pop();
        }

        for(int pos = topk-1; pos>=0; pos--)
        {
            answer_for_query[pos] = pq.top().second;
            pq.pop();
        }
        answers[i] = answer_for_query;
    } 
    return answers;
}


bool save2DvecToBinary(const std::vector<std::vector<int>>& data, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    
    if (!outFile) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return false;
    }

    size_t outerSize = data.size();
    outFile.write(reinterpret_cast<const char*>(&outerSize), sizeof(outerSize));
    
    for (const auto& innerVec : data) {
        size_t innerSize = innerVec.size();
        outFile.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));
        
        if (innerSize > 0) {
            outFile.write(reinterpret_cast<const char*>(innerVec.data()), innerSize * sizeof(int));
        }
    }

    outFile.close();

    if (!outFile) {
        std::cerr << "Error: Failed to write data to file " << filename << ".\n";
        return false;
    }

    return true;
}

// Function --- load a vector of vectors of integers from a binary file
bool load2DvecFromBinary(std::vector<std::vector<int>>& data, const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    
    if (!inFile) {
        std::cerr << "Error: Could not open file " << filename << " for reading.\n";
        return false;
    }

    data.clear();

    size_t outerSize = 0;
    inFile.read(reinterpret_cast<char*>(&outerSize), sizeof(outerSize));
    
    if (!inFile) {
        std::cerr << "Error: Failed to read the outer size from file " << filename << ".\n";
        return false;
    }

    data.reserve(outerSize);

    for (size_t i = 0; i < outerSize; ++i) {
        size_t innerSize = 0;
        inFile.read(reinterpret_cast<char*>(&innerSize), sizeof(innerSize));
        
        if (!inFile) {
            std::cerr << "Error: Failed to read the size of inner vector " << i << " from file " << filename << ".\n";
            return false;
        }

        std::vector<int> innerVec;
        innerVec.resize(innerSize);

        if (innerSize > 0) {
            inFile.read(reinterpret_cast<char*>(innerVec.data()), innerSize * sizeof(int));
            if (!inFile) {
                std::cerr << "Error: Failed to read data for inner vector " << i << " from file " << filename << ".\n";
                return false;
            }
        }

        data.emplace_back(std::move(innerVec));
    }

    inFile.close();

    return true;
}


/* ################################## Interface ########################################## */

void save_answers(string apath, string dpath, string qpath, int topk, bool l2 = true)
{
    int n_data; 
    int n_query;
    int dim;
    
    float * data = get_data(dpath, n_data, dim);
    float * query = get_data(qpath , n_query, dim);

    vector<vector<int>> answers = calculate_answers(data, query, dim, n_query, n_data, topk, l2);
    save2DvecToBinary(answers, apath);
}

void get_answers(string apath, vector<vector<int>> & answers)
{
    load2DvecFromBinary(answers, apath);
    cout<<"loaded answers with size : "<<answers.size()<<" "<<answers[0].size()<<" "<<endl;
}