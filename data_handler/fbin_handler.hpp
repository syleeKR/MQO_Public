#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <string>
#include <memory>
#include <cstdint>
using namespace std;

float * load_fbin(const std::string &filename, int &n, int &d)
{
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    // Read n d
    ifs.read(reinterpret_cast<char*>(&n), sizeof(int));
    ifs.read(reinterpret_cast<char*>(&d), sizeof(int));

    if (!ifs.good()) {
        throw std::runtime_error("Error reading header (n, d) from " + filename);
    }

    size_t total_size = static_cast<size_t>(n) * d;

    if (n <= 0 || d <= 0 || total_size / d != static_cast<size_t>(n)) {
        throw std::runtime_error("Invalid n or d values in " + filename);
    }

    float* data_ptr = new float[total_size];

    // Read n*d floats
    ifs.read(reinterpret_cast<char*>(data_ptr), total_size * sizeof(float));

    if (!ifs.good()) {
        throw std::runtime_error("Error reading data array from " + filename);
    }

    ifs.close();
    return data_ptr;
}

void sample_and_save_subset_fbin(const std::string &path, int l, int r, int sample_size, std::string output_filename)
{
    int n, d;
    std::unique_ptr<float[]> data( load_fbin(path, n, d) );
    
    if (l < 0 || r >= n || l > r) {
        throw std::runtime_error("Invalid range [l, r]. Must be within [0, n-1] and l <= r.");
    }
    int range_size = r - l + 1;
    if (sample_size <= 0 || sample_size > range_size) {
        throw std::runtime_error("Invalid sample_size. Must be > 0 and <= (r - l + 1).");
    }

    std::vector<int> indices;
    indices.reserve(range_size);
    for (int i = l; i <= r; i++) {
        indices.push_back(i);
    }

    // Shuffle
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    indices.resize(sample_size);

    size_t subset_size = static_cast<size_t>(sample_size) * d;
    std::unique_ptr<float[]> subset_data(new float[subset_size]);

    for (int i = 0; i < sample_size; i++) {
        int idx = indices[i];
        const float* src_ptr = data.get() + (static_cast<size_t>(idx) * d);
        float* dst_ptr = subset_data.get() + (static_cast<size_t>(i) * d);
        std::copy(src_ptr, src_ptr + d, dst_ptr);
    }

    // Format: int(new_n), int(d), then new_n * d floats
    std::ofstream ofs(output_filename, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Cannot open file for writing: " + output_filename);
    }

    // Write header: sample_size, d
    ofs.write(reinterpret_cast<const char*>(&sample_size), sizeof(int));
    ofs.write(reinterpret_cast<const char*>(&d), sizeof(int));

    // Write the data
    ofs.write(reinterpret_cast<const char*>(subset_data.get()), 
              static_cast<std::streamsize>(subset_size * sizeof(float)));

    if (!ofs.good()) {
        throw std::runtime_error("Error writing subset data to " + output_filename);
    }

    ofs.close();
    std::cout << "Subset saved to: " << output_filename << std::endl;
}
