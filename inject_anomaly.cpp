#include "edge.hpp"
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

using namespace std;

int sample_with_prob(vector<outEdge>& products, int inject_num)
{
    int sum_of_weight = 0;
    int num_choices = products.size() - 1 - inject_num;
    for(int i = 0; i < num_choices; i++) {
        sum_of_weight += products[i].out.size();
    }
    int rnd = rand()%sum_of_weight;
    for(int i = 0; i < num_choices; i++) {
        if(rnd < products[i].out.size())
            return i;
        rnd -= products[i].out.size();
    }
    return -1;
}

int inject_base_anomaly(vector<outEdge>& customers, vector<outEdge>& products, double inject_ratio, double inject_density, double camo_ratio, int type,\
        int normal_edge, int& inject_edge, int& camo_edge)
{
    srand(time(NULL));
    int inject_num = products.size()*inject_ratio;
    for(int i = 0; i < inject_num; i++)
    {
        customers.push_back(outEdge());
        products.push_back(outEdge());
    }

    inject_edge = inject_num*inject_num*inject_density;
    if(inject_edge > 0.1*normal_edge)
        inject_edge = 0.1*normal_edge;

    if(type == 3) // HIJACKED ATTACK
    {
        for(int i = 0; i < inject_edge; i++)
        {
            //int source = customers.size() - 1 - inject_num - rand()%inject_num; // FOR FIGURES
            int source = rand()%(customers.size() - 1 - inject_num);
            int target = products.size() - 1 - rand()%inject_num;
            customers[source].out.push_back(target);
            products[target].out.push_back(source);
        }
        return inject_edge;
    }

    for(int i = 0; i < inject_edge; i++)
    {
        int source = customers.size() - 1 - rand()%inject_num;
        int target = products.size() - 1 - rand()%inject_num;
        customers[source].out.push_back(target);
        products[target].out.push_back(source);
    }

    camo_edge = inject_edge*camo_ratio;
    switch(type)
    {
        case 0: //no camouflage
            return inject_edge;
        case 1: //random camouflage
            for(int i = 0; i < camo_edge; i++)
            {
                int source = customers.size() - 1 - rand()%inject_num;
                int target = rand()%(products.size() - 1 - inject_num);
                customers[source].out.push_back(target);
                products[target].out.push_back(source);
            }
            break;
        case 2: //biased camouflage
            for(int i = 0; i < camo_edge; i++)
            {
                int source = customers.size() - 1 - rand()%inject_num;
                int target = sample_with_prob(products, inject_num);
                customers[source].out.push_back(target);
                products[target].out.push_back(source);
            }

        default:
            break;
    }
    return inject_edge + camo_edge;
}
