#pragma once

#include <bits/stdc++.h>
using namespace std;

float get_distance_ip(const float * query, int dim, int i, int j)
{
    float sum = 0.0;
    int istart = dim * i;
    int jstart = dim * j;
    for(int pos=0; pos<dim; pos++)
    {
        sum +=  (query[istart+pos] * query[jstart+pos] ) ; 
    }
    return 1 - sum;
}

float get_distance_squared(const float * query, int dim, int i, int j)
{
    float sum = 0.0;
    int istart = dim * i;
    int jstart = dim * j;
    for(int pos=0; pos<dim; pos++)
    {
        float diff = (query[istart+pos] - query[jstart+pos]);
        sum += diff * diff;
    }
    return sum;
}

float get_distance(const float * query, int dim, int i, int j)
{
    return sqrt(get_distance_squared(query, dim, i, j));
}

// #include <immintrin.h>  // AVX intrinsics

// float get_distance_squared_avx(const float* data, int dim, int idx1, int idx2)
// {
//     int offset1 = idx1 * dim;
//     int offset2 = idx2 * dim;

//     __m256 sum = _mm256_setzero_ps();

//     // We'll process 8 floats per iteration
//     int limit = (dim / 8) * 8;
//     for (int pos = 0; pos < limit; pos += 8) {
//         __m256 v1 = _mm256_loadu_ps(&data[offset1 + pos]);
//         __m256 v2 = _mm256_loadu_ps(&data[offset2 + pos]);
//         __m256 diff = _mm256_sub_ps(v1, v2);
//         __m256 sq   = _mm256_mul_ps(diff, diff);
//         sum         = _mm256_add_ps(sum, sq);
//     }

//     // Horizontal add of sum
//     float partial[8];
//     _mm256_storeu_ps(partial, sum);
//     float total = partial[0] + partial[1] + partial[2] + partial[3] +
//                   partial[4] + partial[5] + partial[6] + partial[7];

//     // Remainder loop
//     for (int pos = limit; pos < dim; pos++) {
//         float diff = data[offset1 + pos] - data[offset2 + pos];
//         total += diff * diff;
//     }
//     return total;
// }

// float get_distance_avx(const float * query, int dim, int i, int j)
// {
//     return sqrt(get_distance_squared_avx(query, dim, i, j));
// }

#include <immintrin.h>
#include <cstddef>

float get_distance_squared_avx512(const float* query, int dim, int i, int j)
{
    int istart = dim * i;
    int jstart = dim * j;

    __m512 vsum = _mm512_setzero_ps();
    int pos = 0;

    // Process multiples of 16
    int end = dim - (dim % 16);
    for (; pos < end; pos += 16)
    {
        __m512 va = _mm512_loadu_ps(&query[istart + pos]);
        __m512 vb = _mm512_loadu_ps(&query[jstart + pos]);
        
        __m512 vdiff = _mm512_sub_ps(va, vb);
        __m512 vdiff_sq = _mm512_mul_ps(vdiff, vdiff);
        
        vsum = _mm512_add_ps(vsum, vdiff_sq);
    }
    
    // Horizontally sum the partial sums in vsum
    float sum = _mm512_reduce_add_ps(vsum);

    // Handle leftover elements that aren't a multiple of 16
    for (; pos < dim; ++pos)
    {
        float diff = query[istart + pos] - query[jstart + pos];
        sum += diff * diff;
    }

    return sum;
}
float get_distance_avx512(const float * query, int dim, int i, int j)
{
    return sqrt(get_distance_squared_avx512(query, dim, i, j));
}





std::pair<int, int> sample_two(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());  // Thread-safe alternative: make this thread_local if needed

    std::uniform_int_distribution<> dist(0, n - 1);

    int a = dist(gen);
    int b;
    do {
        b = dist(gen);
    } while (b == a);  // Ensure distinctness

    return {a, b};
}

double get_weight_for_root(float * query, int n_query, int dim, int n_sample = 3000)
{
    double weight_for_root = 0;
    for(int rep =0; rep<n_sample; rep++)
    {   
        pair<int ,int> random_pair = sample_two(n_query);
        weight_for_root += get_distance(query, dim, random_pair.first, random_pair.second );
    }
    weight_for_root /= n_sample;
    //cout<<"weight for root : "<<weight_for_root<<endl;
    return weight_for_root;
}
