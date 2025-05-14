#pragma once 

#include <iostream>
#include <chrono>
#include <thread>   
#include <string>
#include <memory>

#include "../data_handler/data_handler.hpp"

#include "DiskANN/include/index.h"
#include "DiskANN/include/index_factory.h"
#include "DiskANN/include/memory_mapper.h"
#include "DiskANN/include/ann_exception.h"
#include "DiskANN/include/utils.h"                   
#include "DiskANN/include/program_options_utils.hpp" 
#include "DiskANN/include/abstract_index.h"

void build_vamana(const std::string &index_path_prefix,
                  const std::string &data_path,
                  const std::string &data_type,
                  const std::string &dist_fn,
                  uint32_t num_threads = 0,
                  uint32_t R = 64,
                  uint32_t L = 100,
                  float alpha = 1.2f,
                  uint32_t build_PQ_bytes = 0,
                  bool use_opq = false,
                  uint32_t Lf = 0,
                  const std::string &label_file = "",
                  const std::string &universal_label = "",
                  const std::string &label_type = "uint") {

    // If not specified, choose the maximum available threads.
    if (num_threads == 0) {
        num_threads = static_cast<uint32_t>(std::thread::hardware_concurrency());
    }

    diskann::Metric metric;
    if (dist_fn == "mips")
        metric = diskann::Metric::INNER_PRODUCT;
    else if (dist_fn == "l2")
        metric = diskann::Metric::L2;
    else if (dist_fn == "cosine")
        metric = diskann::Metric::COSINE;
    else {
        std::cerr << "Unsupported distance function. Supported values: l2, mips, cosine." << std::endl;
        return;
    }

    size_t data_num, data_dim;
    diskann::get_bin_metadata(data_path, data_num, data_dim);
    std::cout << "Data loaded from: " << data_path << " | Points: " << data_num 
              << " | Dimension: " << data_dim << std::endl;

    auto index_build_params = diskann::IndexWriteParametersBuilder(L, R)
                                    .with_filter_list_size(Lf)
                                    .with_alpha(alpha)
                                    .with_saturate_graph(false)
                                    .with_num_threads(num_threads)
                                    .build();

    auto filter_params = diskann::IndexFilterParamsBuilder()
                                .with_universal_label(universal_label)
                                .with_label_file(label_file)
                                .with_save_path_prefix(index_path_prefix)
                                .build();

    auto config = diskann::IndexConfigBuilder()
                        .with_metric(metric)
                        .with_dimension(data_dim)
                        .with_max_points(data_num)
                        .with_data_load_store_strategy(diskann::DataStoreStrategy::MEMORY)
                        .with_graph_load_store_strategy(diskann::GraphStoreStrategy::MEMORY)
                        .with_data_type(data_type)
                        .with_label_type(label_type)
                        .is_dynamic_index(false)
                        .with_index_write_params(index_build_params)
                        .is_enable_tags(false)
                        .is_use_opq(use_opq)
                        .is_pq_dist_build(build_PQ_bytes > 0)
                        .with_num_pq_chunks(build_PQ_bytes)
                        .build();

    auto index_factory = diskann::IndexFactory(config);
    auto index = index_factory.create_instance();

    std::cout << "Starting Vamana index build with parameters:" << std::endl;
    std::cout << "Max Degree (R): " << R << ", Lbuild: " << L << ", Alpha: " << alpha 
              << ", Threads: " << num_threads << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    index->build(data_path, data_num, filter_params);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Vamana index built in " << elapsed.count() << " seconds." << std::endl;

    index->save(index_path_prefix.c_str());
    std::cout << "Vamana index saved to: " << index_path_prefix << std::endl;
}





std::unique_ptr<diskann::AbstractIndex> read_vamana(const std::string &data_path,
                                            const std::string &index_path,
                                            const std::string &data_type,
                                            const std::string &dist_fn,
                                            uint32_t L_search = 100,
                                            uint32_t num_threads = 0) 
{
    if (num_threads == 0)
        num_threads = static_cast<uint32_t>(std::thread::hardware_concurrency());

    size_t data_num, data_dim;
    diskann::get_bin_metadata(data_path, data_num, data_dim);
    std::cout << "Loading Vamana index with data info: " 
              << data_num << " points, dimension = " << data_dim << std::endl;

    diskann::Metric metric;
    if (dist_fn == "mips")
        metric = diskann::Metric::INNER_PRODUCT;
    else if (dist_fn == "l2")
        metric = diskann::Metric::L2;
    else if (dist_fn == "cosine")
        metric = diskann::Metric::COSINE;
    else {
        std::cerr << "Unsupported distance function: " << dist_fn 
                  << ". Use l2, mips, or cosine.\n";
        return nullptr;
    }

    auto config = diskann::IndexConfigBuilder()
                      .with_metric(metric)
                      .with_dimension((uint32_t)data_dim)
                      .with_max_points((uint32_t)data_num)
                      .with_data_load_store_strategy(diskann::DataStoreStrategy::MEMORY)
                      .with_graph_load_store_strategy(diskann::GraphStoreStrategy::MEMORY)
                      .with_data_type(data_type)
                      .build();

    auto index_factory = diskann::IndexFactory(config);
    auto index = index_factory.create_instance();

    index->load(index_path.c_str(), num_threads, L_search);

    std::cout << "Vamana index loaded from: " << index_path << std::endl;
    return index;
}
