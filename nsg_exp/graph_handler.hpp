#include <iostream>
#include <chrono>
#include <string>

#include "../data_handler/data_handler.hpp"


// Include efanna/NSG headers
#include "efanna_graph/include/efanna2eold/index_graph.h"
#include "efanna_graph/include/efanna2eold/index_random.h"

#include "nsg/include/efanna2e/index_nsg.h"
#include "nsg/include/efanna2e/util.h"

void build_nsg_l2(const std::string &savepath,
                const std::string & savepath_temp,
               float* data,
               unsigned num_points,
               int dim,
               // Parameters for efanna graph construction:
               unsigned knn_K,
               unsigned knn_L,
               unsigned knn_iter,
               unsigned knn_S,
               unsigned knn_R,
               // Parameters for NSG index construction:
               unsigned nsg_L,
               unsigned nsg_R,
               unsigned nsg_C) {

    // Phase 1: Build the efanna kNN graph.
    std::cout << "Building initial kNN graph with efanna..." << std::endl;
    efanna2eold::IndexRandom init_index(dim, num_points);
    efanna2eold::IndexGraph index_graph(dim, num_points, efanna2eold::L2, (efanna2eold::Index*)(&init_index));
    
    std::cout<<"hello"<<std::endl;
    // Set parameters for the efanna graph builder.
    efanna2eold::Parameters paras_graph;
    paras_graph.Set<unsigned>("K", knn_K);
    paras_graph.Set<unsigned>("L", knn_L);
    paras_graph.Set<unsigned>("iter", knn_iter);
    paras_graph.Set<unsigned>("S", knn_S);
    paras_graph.Set<unsigned>("R", knn_R);
    
    std::cout<<"hello"<<std::endl;

    auto start_graph = std::chrono::high_resolution_clock::now();
    index_graph.Build(num_points, data, paras_graph);
    auto end_graph = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_graph = end_graph - start_graph;
    std::cout << "efanna kNN graph built in " << elapsed_graph.count() << " seconds." << std::endl;
    
    index_graph.Save(savepath_temp.c_str());
    std::cout << "efanna kNN graph saved to temporary file: " << savepath_temp << std::endl;
    

    // Phase 2: Build the NSG index using the efanna kNN graph.
    std::cout << "Building NSG index..." << std::endl;
    efanna2e::IndexNSG nsg_index(dim, num_points, efanna2e::L2, nullptr);
    
    efanna2e::Parameters paras_nsg;
    paras_nsg.Set<unsigned>("L", nsg_L);
    paras_nsg.Set<unsigned>("R", nsg_R);
    paras_nsg.Set<unsigned>("C", nsg_C);
    paras_nsg.Set<std::string>("nn_graph_path", savepath_temp);
    
    auto start_nsg = std::chrono::high_resolution_clock::now();
    nsg_index.Build(num_points, data, paras_nsg);
    auto end_nsg = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_nsg = end_nsg - start_nsg;
    std::cout << "NSG index built in " << elapsed_nsg.count() << " seconds." << std::endl;
    
    nsg_index.Save(savepath.c_str());
    std::cout << "NSG index saved to: " << savepath << std::endl;
}

efanna2e::IndexNSG* read_nsg_l2(const std::string & datapath, const std::string &gpath) {

    int n, dim;
    float * data = get_data(datapath, n, dim);

    efanna2e::IndexNSG* index = new efanna2e::IndexNSG(dim, n, efanna2e::L2, nullptr);
    index->Load(gpath.c_str());
    std::cout << "NSG index loaded from: " << gpath << std::endl;
    std::cout << "Index point count: " << n << std::endl;

    //optimize it
    index->OptimizeGraph(data);

    return index;
}


/* ######################################################## */

void build_nsg_ip(const std::string &savepath,
    const std::string & savepath_temp,
   float* data,
   unsigned num_points,
   int dim,
   // Parameters for efanna graph construction:
   unsigned knn_K,
   unsigned knn_L,
   unsigned knn_iter,
   unsigned knn_S,
   unsigned knn_R,
   // Parameters for NSG index construction:
   unsigned nsg_L,
   unsigned nsg_R,
   unsigned nsg_C) {

std::cout << "Building initial kNN graph with efanna..." << std::endl;
// Create an initial random index required for efanna's kNN graph construction.
efanna2eold::IndexRandom init_index(dim, num_points);
efanna2eold::IndexGraph index_graph(dim, num_points, efanna2eold::INNER_PRODUCT, (efanna2eold::Index*)(&init_index));

std::cout<<"hello"<<std::endl;
efanna2eold::Parameters paras_graph;
paras_graph.Set<unsigned>("K", knn_K);
paras_graph.Set<unsigned>("L", knn_L);
paras_graph.Set<unsigned>("iter", knn_iter);
paras_graph.Set<unsigned>("S", knn_S);
paras_graph.Set<unsigned>("R", knn_R);

std::cout<<"hello"<<std::endl;

auto start_graph = std::chrono::high_resolution_clock::now();
index_graph.Build(num_points, data, paras_graph);
auto end_graph = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> elapsed_graph = end_graph - start_graph;
std::cout << "efanna kNN graph built in " << elapsed_graph.count() << " seconds." << std::endl;

index_graph.Save(savepath_temp.c_str());
std::cout << "efanna kNN graph saved to temporary file: " << savepath_temp << std::endl;


std::cout << "Building NSG index..." << std::endl;
efanna2e::IndexNSG nsg_index(dim, num_points, efanna2e::INNER_PRODUCT, nullptr);

efanna2e::Parameters paras_nsg;
paras_nsg.Set<unsigned>("L", nsg_L);
paras_nsg.Set<unsigned>("R", nsg_R);
paras_nsg.Set<unsigned>("C", nsg_C);
paras_nsg.Set<std::string>("nn_graph_path", savepath_temp);

auto start_nsg = std::chrono::high_resolution_clock::now();
nsg_index.Build(num_points, data, paras_nsg);
auto end_nsg = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> elapsed_nsg = end_nsg - start_nsg;
std::cout << "NSG index built in " << elapsed_nsg.count() << " seconds." << std::endl;

nsg_index.Save(savepath.c_str());
std::cout << "NSG index saved to: " << savepath << std::endl;
}

efanna2e::IndexNSG* read_nsg_ip(const std::string & datapath, const std::string &gpath) {

    int n, dim;
    float * data = get_data(datapath, n, dim);

    efanna2e::IndexNSG* index = new efanna2e::IndexNSG(dim, n, efanna2e::INNER_PRODUCT, nullptr);
    // Load the index from the file.
    index->Load(gpath.c_str());
    std::cout << "NSG index loaded from: " << gpath << std::endl;
    std::cout << "Index point count: " << n << std::endl;

    //optimize it
    //index->OptimizeGraph(data);

    return index;
}
