#pragma once
#include "hnswlib/hnswlib.h"

using namespace std;




void build(string savepath, float * data, int max_elements, int dim, int M, int ef_construction)
{

    // Initing index
    hnswlib::L2Space space(dim);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = new hnswlib::HierarchicalNSW<float>(&space, max_elements, M, ef_construction);

    cout<<"starting build"<<endl;


    // Add data to index
    for (size_t i = 0; i < max_elements; i++) {
        if((i%1000000)==0) cout<< "building : "<<i/1000000<<"M"<<endl;
        alg_hnsw->addPoint(data + i * dim, i);
    }

    alg_hnsw->saveIndex(savepath);
    delete alg_hnsw;
    cout<<"built"<<endl;
}

pair<hnswlib::HierarchicalNSW<float>*, hnswlib::L2Space*> read(string hnsw_path, int dim)
{
    hnswlib::L2Space* space = new hnswlib::L2Space(dim);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = new hnswlib::HierarchicalNSW<float>(space, hnsw_path);
    // cout<<"done reading"<<endl;

    std::cout << "Index currentCount: " << alg_hnsw->cur_element_count << std::endl;
    cout<<"data size(=sizeof(float) * dim) : "<<alg_hnsw -> data_size_<<endl;
    return {alg_hnsw, space};
}


/*  ####################################################  */

void build_ip(string savepath, float * data, int max_elements, int dim, int M, int ef_construction)
{

    hnswlib::InnerProductSpace space(dim);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = new hnswlib::HierarchicalNSW<float>(&space, max_elements, M, ef_construction);

    cout<<"starting build"<<endl;


    // Add data to index
    for (size_t i = 0; i < max_elements; i++) {
        if((i%1000000)==0) cout<< "building : "<<i/1000000<<"M"<<endl;
        alg_hnsw->addPoint(data + i * dim, i);
    }

    alg_hnsw->saveIndex(savepath);
    delete alg_hnsw;
    cout<<"built"<<endl;
}

pair<hnswlib::HierarchicalNSW<float>*, hnswlib::InnerProductSpace*> read_ip(string hnsw_path, int dim)
{
    hnswlib::InnerProductSpace* space = new hnswlib::InnerProductSpace(dim);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = new hnswlib::HierarchicalNSW<float>(space, hnsw_path);
    // cout<<"done reading"<<endl;

    std::cout << "Index currentCount: " << alg_hnsw->cur_element_count << std::endl;
    cout<<"data size(=sizeof(float) * dim) : "<<alg_hnsw -> data_size_<<endl;
    return {alg_hnsw, space};
}
