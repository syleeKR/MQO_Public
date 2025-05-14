#pragma once

#include <bits/stdc++.h>
#include "hnswlib/hnswalg.h"   
#include "hnswlib/hnswlib.h"  

using namespace std;


namespace mqohnswlib {


    template <typename dist_t>
    tableint addPoint_modified(
        HierarchicalNSW<dist_t>* hnsw,
        const dist_t* data_point,
        labeltype label,
        //std::unordered_map<std::pair<int,int>, dist_t, PairHash> &dist_map
        std::map<std::pair<int, int>, float> & dist_map
    )
    {    
    
        tableint cur_c = hnsw->cur_element_count;
        hnsw->cur_element_count++;
    
        {
            std::unique_lock<std::mutex> lock_table(hnsw->label_lookup_lock);
            hnsw->label_lookup_[label] = cur_c;
        }
    
        hnsw->element_levels_[cur_c] = 0;

    
        std::memset(
            hnsw->data_level0_memory_ + cur_c * hnsw->size_data_per_element_ + hnsw->offsetLevel0_,
            0,
            hnsw->size_data_per_element_);
    
        std::memcpy(hnsw->getExternalLabeLp(cur_c), &label, sizeof(labeltype));
        std::memcpy(hnsw->getDataByInternalId(cur_c), data_point, hnsw->data_size_);
    
        if ((int)hnsw->enterpoint_node_ < 0) {
            hnsw->enterpoint_node_ = cur_c;
            hnsw->maxlevel_ = 0; // No multi-level structure
            return cur_c;
        }

        {
            size_t ef_base = std::max(hnsw->M_, hnsw->ef_construction_);
    
            // The existing entry point:
            tableint currObj = hnsw->enterpoint_node_;
    
            auto top_candidates = hnsw->searchBaseLayerST_logging(
                currObj, 
                data_point,
                ef_base,
                dist_map
            );

            hnsw->mutuallyConnectNewElement_logging(
                data_point,
                cur_c,
                top_candidates,
                0,
                false,
                dist_map
            );

        }
    
        return cur_c;
    }
    
} // end namespace hnswlib

    

//std::tuple<hnswlib::HierarchicalNSW<float>*, hnswlib::L2Space*,std::unordered_map<std::pair<int,int>, float, PairHash>>
std::tuple<mqohnswlib::HierarchicalNSW<float>*, mqohnswlib::L2Space*, map<pair<int, int>, float>>
build_ann_base_only(
    float* data,
    int num_elements,
    int dim,
    int M,
    int ef_construction
) {
    auto* space = new mqohnswlib::L2Space(dim);

    auto* alg_hnsw = new mqohnswlib::HierarchicalNSW<float>(
        space,
        num_elements,  // max elements
        M,
        ef_construction
    );


    std::map<std::pair<int, int>, float> dist_map;

    std::cout << "Starting base-layer-only build (with logging)...\n";
    for (int i = 0; i < num_elements; i++) {
        const float* vec_ptr = data + size_t(i)*dim;
        mqohnswlib::addPoint_modified<float>(
            alg_hnsw,
            vec_ptr,
            /* label = */ i,
            dist_map    // pass our logging map
        );
    }
    std::cout << "Finished building base-layer-only.\n";

    return std::make_tuple(alg_hnsw, space, std::move(dist_map));
}





