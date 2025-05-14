#pragma once

#include <bits/stdc++.h>
using namespace std;

void checkTrajectory(vector<pair<int, int>> & trajectory, int n_query)
{
    assert(trajectory.size() == n_query);
    set<int> full;
    for(int i=0; i<n_query; i++)full.insert(i);

    set<int> nodes;
    for(auto x : trajectory)nodes.insert(x.first);

    assert(full == nodes);

    set<int> prev;
    bool possible = true;
    for(auto x : trajectory)
    {
        if(x.second!=-1)
        {
            if(prev.count(x.second)!=1)
            {
                cout<<x.first<<" "<<x.second<<endl;
                possible=false;
            }
        }
        prev.insert(x.first);
    }
    assert(possible);
}