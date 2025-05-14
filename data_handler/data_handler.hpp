#pragma once

#include "fbin_handler.hpp"
#include "u8bin_handler.hpp"


// float * get_data(const std::string &dpath, int &max_elements, int &dim)
// {
//     return load_fbin(dpath, max_elements, dim);
// }


// float * get_data(string dpath, int & max_elements, int & dim)
// {
//     uint8_t * u8_data = load_u8bin(dpath, max_elements, dim);
//     float* data = new float[(long long)max_elements * dim];

//     for (size_t i = 0; i < (long long)max_elements * dim; ++i) {
//         data[i] = static_cast<float>(u8_data[i]) ;
//     }
//     delete[] u8_data;
//     return data;
// }

float * get_data(const std::string &dpath, int &max_elements, int &dim)
{
    if (dpath.size() >= 6 && dpath.substr(dpath.size() - 6) == ".u8bin") {
        // Load uint8_t data and convert to float
        uint8_t * u8_data = load_u8bin(dpath, max_elements, dim);
        float* data = new float[(long long)max_elements * dim];

        for (size_t i = 0; i < (long long)max_elements * dim; ++i) {
            data[i] = static_cast<float>(u8_data[i]);
        }
        delete[] u8_data;
        return data;
    } 
    else if (dpath.size() >= 5 && dpath.substr(dpath.size() - 5) == ".fbin") {
        return load_fbin(dpath, max_elements, dim);
    } 
    else {
        throw std::runtime_error("Unsupported file format: " + dpath);
    }
}



void sample_and_save_subset(const std::string &path, int l, int r, int sample_size, std::string output_filename)
{
    if (path.size() >= 6 && path.substr(path.size() - 6) == ".u8bin") {
        sample_and_save_subset_u8bin(path, l,r,sample_size, output_filename);
    } 
    else if (path.size() >= 5 && path.substr(path.size() - 5) == ".fbin") {
        sample_and_save_subset_fbin(path, l,r,sample_size, output_filename);
    } 
    else {
        throw std::runtime_error("Unsupported file format: " + path);
    }
}
