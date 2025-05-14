#pragma once


#include <bits/stdc++.h>
using namespace std;


struct csvSaver
{
    string graphname;
    map<int, vector<pair<double, double>>> data;

    csvSaver(string graphname):graphname(graphname){}


    void add(double beamsize, vector<pair<double,double>> values, bool verbose=true)
    {
        data[beamsize] = values;

        if(verbose)
        {
            cout<<"Beamsize : "<<beamsize<<" ";
            for(int i=0; i<values.size(); i++)
            {
                pair<double, double> value = values[i];
                cout<<"/ " <<i <<"-";
                cout<<"Recall : "<<value.first <<" Time : "<<value.second << " ";
            }
            cout<<endl;
        }
    }

    int getlen()
    {
        for(auto x : data)
        {
            return x.second.size();
        }
        return 0;
    }

    void save()
    {
        std::ofstream ofs(graphname + ".csv");

        int n = getlen();
        ofs << "BeamSize";
        for (int i=1; i<=n; i++)
        {
            ofs<<",R"<<to_string(i)<<",T"<<to_string(i);
        }
        ofs<<"\n";

        for(auto x : data)
        {
            double beamsize = x.first;
            vector<pair<double, double>> d = x.second;

            ofs << beamsize;
            for (auto y : d)
            {
                ofs<<","<<y.first<<","<<y.second;
            }
            ofs<<"\n";

        }
        ofs.close();
        std::cout << "Results saved to "+graphname+".csv\n";


    }


};