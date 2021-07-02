#include "edge.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#define EPSILON  0.001
#define coeff 0.15

using namespace std;

double propagate(vector<outEdge>& A, vector<double>& input, vector<double>& output, bool decaying)
{
    for(int i = 0; i < output.size(); i++)
        output[i] = 0;

    for(int i = 0; i < input.size(); i++)
    {
        if(input[i] == 0)
            continue;
        if(A[i].out.size() == 0)
            continue;

        double delta = 0;
        if(decaying)
            delta = (1-coeff)*input[i]/A[i].out.size();
        else
            delta = input[i]/A[i].out.size();

        std::vector<int>::iterator j = A[i].out.begin();
        for(; j != A[i].out.end(); j++)
            output[*j] += delta;
    }

    double delta_score = 0;
    for(int i = 0; i < output.size(); i++)
        delta_score += std::abs(output[i]);
    return delta_score;
}


void bipartite_rwr(vector<outEdge>& primary, vector<outEdge>& secondary, vector<double>& rwr)
{
    for(int i = 0; i < rwr.size(); i++)
        rwr[i] *= coeff;
    vector<double> primary_score(rwr);
    vector<double> secondary_score(secondary.size(), 0);
    double delta = 200.;
    while(delta > EPSILON)
    {
        delta = propagate(primary, primary_score, secondary_score, true);
        delta += propagate(secondary, secondary_score, primary_score, false);
        for(int i = 0; i < rwr.size(); i++)
            rwr[i] += primary_score[i];
    }

    double sum = 0.0;
    for(int i = 0; i < rwr.size(); i++)
        sum += rwr[i];
    for(int i = 0; i < rwr.size(); i++)
        rwr[i] /= sum;

}

bool bipartite_tpa(vector<outEdge>& primary, vector<outEdge>& secondary, vector<double>& rwr, int s_iter, int t_iter)
{
    for(int i = 0; i < rwr.size(); i++)
        rwr[i] *= coeff;
    vector<double> primary_score(rwr);
    vector<double> secondary_score(secondary.size(), 0);
    double delta = 200.;
    int iter = 0;
    while(delta > EPSILON)
    {
        delta = propagate(primary, primary_score, secondary_score, true);
        delta += propagate(secondary, secondary_score, primary_score, false);
        for(int i = 0; i < rwr.size(); i++)
            rwr[i] += primary_score[i];
        if(iter == s_iter)
            break;
        if(iter == t_iter)
        {
            for(int i = 0; i < rwr.size(); i++)
                rwr[i] = 0;
        }
        iter++;
    }

    bool dead_end = false;
    if(delta == 0 && iter == 1)
        dead_end = true;
    double sum = 0.0;
    for(int i = 0; i < rwr.size(); i++)
        sum += rwr[i];
    for(int i = 0; i < rwr.size(); i++)
        rwr[i] /= sum;

    return dead_end;
}
