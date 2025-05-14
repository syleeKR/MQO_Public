#pragma once
#include <bits/stdc++.h>
using namespace std;


float get_distance_data_query(float * data, float * query, int dim, int dindex, int qindex )
{
    float sum = 0.0;
    int dstart = dim * dindex;
    int qstart = dim * qindex;
    for(int i=0 ; i< dim; i++)
    {
        float diff =  data[dstart +  i] - query[qstart + i];
        sum += diff * diff;
    }
    return sqrt(sum);
}


float get_distance_data_query_ip(float * data, float * query, int dim, int dindex, int qindex )
{
    float sum = 0.0;
    int dstart = dim * dindex;
    int qstart = dim * qindex;
    for(int i=0 ; i< dim; i++)
    {
        float ip =  data[dstart +  i] * query[qstart + i];
        sum += ip;
    }
    return 1.0 - sum;
}

