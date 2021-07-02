#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <numeric>
#include <map>
#include "edge.hpp"

using namespace std;

vector<size_t> sort_indexes(const vector<outEdge> &v) {
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1].out.size() < v[i2].out.size();});
    return idx;
}

void arrange_graph(vector<outEdge>& c_old, vector<outEdge>& c_new, vector<outEdge>& p_old, vector<outEdge>& p_new)
{
    vector<size_t> c_idx = sort_indexes(c_old);
    vector<size_t> p_idx = sort_indexes(p_old);

    vector<size_t> cc_idx(c_idx.size());
    vector<size_t> pp_idx(p_idx.size());
    for(int i = 0; i < c_idx.size(); i++)
        cc_idx[c_idx[i]] = i;
    for(int i = 0; i < p_idx.size(); i++)
        pp_idx[p_idx[i]] = i;
    for(int idx = 0; idx < c_idx.size(); idx++)
    {
        outEdge edge;
        int i = c_idx[idx];
        for(int j = 0; j < c_old[i].out.size(); j++)
            edge.out.push_back(pp_idx[c_old[i].out[j]]);
        c_new.push_back(edge);
    }
    for(int idx = 0; idx < p_idx.size(); idx++)
    {
        outEdge edge;
        int i = p_idx[idx];
        for(int j = 0; j < p_old[i].out.size(); j++)
            edge.out.push_back(cc_idx[p_old[i].out[j]]);
        p_new.push_back(edge);
    }
}

void read_data(string graph, vector<outEdge>& customers, vector<outEdge>& products, int& m)
{
    string filePath = "DATA/" + graph + ".txt";
    ifstream graphFile(filePath.c_str());
    string line;
    string delimiter("\t");
    vector<int> source;
    vector<int> target;
    while(getline(graphFile, line))
    {
        size_t pos = 0;
        vector<int> tokens;
        while ((pos = line.find(delimiter)) != string::npos)
        {
            tokens.push_back(stoi(line.substr(0, pos)));
            line.erase(0, pos + delimiter.length());
        }
        tokens.push_back(stoi(line));
        source.push_back(tokens[0]);
        target.push_back(tokens[1]);
    }
    int n1 = *max_element(source.begin(), source.end()) + 1;
    int n2 = *max_element(target.begin(), target.end()) + 1;
    m = source.size();

    bool smallSource = n1 < n2;

    for(int i = 0; i < max(n1,n2); i++)
        customers.push_back(outEdge());
    for(int i = 0; i < min(n1,n2); i++)
        products.push_back(outEdge());
    for(int i = 0; i < source.size(); i++)
    {
        if(smallSource)
        {
            customers[target[i]].out.push_back(source[i]);
            products[source[i]].out.push_back(target[i]);

        }
        else
        {
            customers[source[i]].out.push_back(target[i]);
            products[target[i]].out.push_back(source[i]);
        }
    }
    graphFile.close();
}

void read_review(string path, string output)
{
    ifstream graphFile(path.c_str());
    string line;
    map<string, int> products;
    map<string, int> users;
    int product_id = 0;
    int user_id = 0;
    string delimiter_p("product/productId: ");
    string delimiter_u("review/userId: ");

    ofstream writeFile;
    writeFile.open(output.c_str(), ofstream::out);

    while(getline(graphFile, line))
    {
        size_t pos = 0;
        vector<int> tokens;
        if((pos = line.find(delimiter_p)) != string::npos)
        {
            string product_name = line.substr(pos, line.length());
            if(products.find(product_name) == products.end())
            {
                products[product_name] = product_id;
                product_id++;
            }

            getline(graphFile, line);
            pos =  line.find(delimiter_u);
            string user_name = line.substr(pos, line.length());
            if(users.find(user_name) == users.end())
            {
                users[user_name] = user_id;
                user_id++;
            }
            writeFile << products[product_name] << " " << users[user_name] << endl;
        }
    }
    graphFile.close();
    writeFile.close();
    writeFile.open("DATA/finefood_product.txt", ofstream::out);
    for(map<string,int>::iterator it = products.begin(); it != products.end(); it++)
        writeFile << it->first << "\t" << it->second<<endl;
    writeFile.close();
    writeFile.open("DATA/finefood_user.txt", ofstream::out);
    for(map<string,int>::iterator it = users.begin(); it != users.end(); it++)
        writeFile << it->first << "\t" << it->second<<endl;
    writeFile.close();
}

void write_data(vector<outEdge>& products, string graph, int inject_num, int anomaly_type, double anomaly_ratio, double anomaly_density, double camo_ratio)
{
    std::ostringstream filePath;
    filePath << "DATA/" << graph << "/" << anomaly_type << "_" << std::setprecision(2) << anomaly_ratio << "_" << anomaly_density << "_" << camo_ratio << ".txt";
    ofstream writeFile;
    writeFile.open(filePath.str().c_str(), ofstream::out);
    writeFile << inject_num << endl;
    for(int i = 0; i < products.size(); i++)
    {
        for(int j = 0; j < products[i].out.size() ; j++)
            writeFile << i << " " << products[i].out[j]<<endl;
    }
    writeFile.close();

}
