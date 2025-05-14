#pragma once

#include <bits/stdc++.h>
#include "hnswlib/hnswalg.h"   

using namespace std;



//std::tuple<hnswlib::HierarchicalNSW<float>*, hnswlib::L2Space*,std::unordered_map<std::pair<int,int>, float, PairHash>>
std::tuple<mqohnswlib::HierarchicalNSW<float>*, mqohnswlib::L2Space*, map<pair<int, int>, float>>
build_ann_full_l2(
    float* data,
    int num_elements,
    int dim,
    int M,
    int ef_construction
) {

        auto* space  = new mqohnswlib::L2Space(dim);

        auto* alg_hnsw = new mqohnswlib::HierarchicalNSW<float>(
            space,
            num_elements,  // max elements
            M,
            ef_construction
        );
        std::map<std::pair<int, int>, float> dist_map;

        for (int i = 0; i < num_elements; i++) {
            alg_hnsw->addPoint(data+i*dim, i);
        }

        return std::make_tuple(alg_hnsw, space, std::move(dist_map));
}

std::tuple<mqohnswlib::HierarchicalNSW<float>*, mqohnswlib::InnerProductSpace*, map<pair<int, int>, float>>
build_ann_full_ip(
    float* data,
    int num_elements,
    int dim,
    int M,
    int ef_construction
) {

        auto* space  = new mqohnswlib::InnerProductSpace(dim);

        auto* alg_hnsw = new mqohnswlib::HierarchicalNSW<float>(
            space,
            num_elements,  // max elements
            M,
            ef_construction
        );
        std::map<std::pair<int, int>, float> dist_map;

        for (int i = 0; i < num_elements; i++) {
            alg_hnsw->addPoint(data+i*dim, i);
        }

        return std::make_tuple(alg_hnsw, space, std::move(dist_map));
}





