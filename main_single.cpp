#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <random>
#include <vector>
#include <iterator>
#include "time.h"

#include "compute_anomaly.cpp"
#include "rwr.cpp"
#include "read_data.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    /******** ARGUMENTS *******/
    string graph = argv[1];
    int anomaly_type = stoi(argv[2]);
    double anomaly_ratio = stod(argv[3]);
    double anomaly_density = stod(argv[4]);
    double camo_ratio = stod(argv[5]);
    int S = stoi(argv[6]);
    int T = stoi(argv[7]);

    /********* READ DATA **********/
    int normal_edge = 0;
    vector<outEdge> temp_c;
    vector<outEdge> temp_p;
    vector<outEdge> customers;
    vector<outEdge> products;

    /**** CHECK THE ORDER OF PRODUCT-CUSTOMER ****/
    read_data(graph, customers, products, normal_edge);

    /******** ORDER BY NUMBER OF DEGREE *******/
    //read_data(graph, temp_c, temp_p, normal_edge);
    //arrange_graph(temp_c, customers, temp_p, products);

    /******** ANOMALY INJECTION *******/
    int injected_edge = products.size()*anomaly_ratio;
    int fake_edge = 0;
    int camo_edge = 0;
    inject_base_anomaly(customers, products, anomaly_ratio, anomaly_density, camo_ratio, anomaly_type, normal_edge, fake_edge, camo_edge);
    write_data(products, graph, injected_edge, anomaly_type, anomaly_ratio, anomaly_density, camo_ratio);
    return 1;
    /********** TPA-RWR ***********/
    int n = products.size();
    vector<vector<double>> rwr(n);
    /*
    double merg_coeff = (1-pow((1-coeff),T));
    vector<double> pagerank(n, 1./n);
    bipartite_tpa(products, customers, pagerank, -1, T);
    */

    clock_t begin = clock();
    for(int i = 0; i < n; i++)
    {
        if(i%1000 == 0) cout<< i <<"th node RWR computation starts..." << endl;

        for(int j = 0; j < n; j++)
            rwr[i].push_back(0);
        rwr[i][i] = 1;

        if(bipartite_tpa(products, customers, rwr[i], S, -1))
            continue;

        /* STRANGER APPROXIMATION
        for(int j = 0; j < n; j++)
            rwr[i][j] = merg_coeff*rwr[i][j] + (1-merg_coeff)*pagerank[j];
        */

        /* TEST ACCURACY OF TPA
        vector<double> temp(n, 0.);
        temp[i] = 1;
        bipartite_rwr(products, customers, temp);
        double error = 0;
        for(int j = 0; j <n; j++)
            error += abs(rwr[i][j]-temp[j]);
        cout<< error<< endl;
        */
    }
    cout<< "Elapsed time for RWR computation: " << double(clock() - begin) / CLOCKS_PER_SEC << endl;;

    /* PURE RWR
    int n = products.size();
    vector<vector<double>> rwr(n);
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
            rwr[i].push_back(0);
        rwr[i][i] = 1;
        bipartite_rwr(products, customers, rwr[i]);
    }
    */

    /******** WRITE RWR SCORE ********/
    std::ostringstream filePath;
    filePath << "RESULT/" << graph << "/" << anomaly_type << "_" << std::setprecision(2) << anomaly_ratio << "_" << anomaly_density << "_" << camo_ratio << ".txt";
    ofstream writeFile;
    writeFile.open(filePath.str().c_str(), ofstream::out);
    writeFile << normal_edge << " " << fake_edge << " " <<camo_edge << endl;
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n ; j++)
            writeFile << rwr[i][j] << " ";
        writeFile << endl;
    }
    writeFile.close();

    /******** COMPUTE ANOMALOUSNESS ********/
    compute_step_function(rwr, anomaly_ratio);
    return 1;
}

