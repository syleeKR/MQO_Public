#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>


/*

g++ -std=c++17 -O3 -march=native -ffast-math -fopenmp  -ftree-vectorize -funroll-loops -o convert_type convert_type.cpp

*/

#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>

void convert_u8bin_to_fbin(const std::string& input_path, const std::string& output_path) {
    std::ifstream in(input_path, std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open " << input_path << std::endl;
        return;
    }

    int32_t n = 0, d = 0;
    in.read(reinterpret_cast<char*>(&n), sizeof(int32_t));
    in.read(reinterpret_cast<char*>(&d), sizeof(int32_t));

    if (n <= 0 || d <= 0) {
        std::cerr << "Invalid header: n or d is non-positive" << std::endl;
        return;
    }

    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open " << output_path << std::endl;
        return;
    }

    out.write(reinterpret_cast<char*>(&n), sizeof(int32_t));
    out.write(reinterpret_cast<char*>(&d), sizeof(int32_t));

    std::vector<uint8_t> row(d);
    std::vector<float> frow(d);

    for (int i = 0; i < n; ++i) {
        in.read(reinterpret_cast<char*>(row.data()), d);
        if (in.gcount() != d) {
            std::cerr << "Unexpected end of file at row " << i << std::endl;
            break;
        }

        for (int j = 0; j < d; ++j) {
            frow[j] = static_cast<float>(row[j]);
        }

        out.write(reinterpret_cast<char*>(frow.data()), sizeof(float) * d);
    }

    in.close();
    out.close();

    std::cout << "Converted " << input_path << " to " << output_path
              << " with " << n << " vectors of dimension " << d << std::endl;
}


int main() {
    std::string datapath = "some path goes here";
    std::string outpath = "some path goes here";
    convert_u8bin_to_fbin(datapath, outpath);
    return 0;
}
