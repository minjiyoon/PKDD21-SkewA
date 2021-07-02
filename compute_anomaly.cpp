#include <vector>
#include <numeric>
#include <iostream>
#include <math.h>
#include "edge.hpp"
#include "inject_anomaly.cpp"

using namespace std;

void compute_step_function(vector<vector<double>>& rwr, double inject_ratio)
{
    vector<double> stepness;
    for(int i = 0; i < rwr.size(); i++)
    {
        int* hist = new int[200];
        for(int j = 0; j < 200; j++)
            hist[j] = 0;
        for(int j = 0; j < rwr.size(); j++)
        {
            if(rwr[j][i]==0)
                hist[199]++;
            else
            {
                int bucket = int(-10*log(rwr[j][i]));
                hist[bucket]++;
            }
        }
        // DEADEND
        if(hist[199] == rwr.size()-1)
        {
            stepness.push_back(100);
            continue;
        }
        int local_minimum = 198;
        int cummulated = 0;
        for(int j = 199; j > 1; j--)
        {
            // NO CAMOUFLAGE
            if(hist[199] > 0.5*rwr.size())
            {
                break;
            }
            cummulated += hist[j];
            if(hist[j] > hist[j-1] && hist[j-1] < hist[j-2] && cummulated > 0.5*rwr.size())
            {
                local_minimum = j-1;
                break;
            }
        }
        int num1 = 0, num2 = 0;
        double mean1 = 0, mean2 = 0;
        double var1 = 0, var2 = 0;
        double sum1 = 0, sum2 = 0;
        for(int j = 0; j < rwr.size(); j++)
        {
            if(rwr[j][i] == 0)
            {
                num1++;
                continue;
            }
            double score = rwr[j][i];

            if(int(-10*log(score)) >= local_minimum)
            {
                sum1 += rwr[j][i];
                num1++;
                mean1 += score;
                var1 += pow(score, 2);
            }
            else
            {
                sum2 += rwr[j][i];
                num2++;
                mean2 += score;
                var2 += pow(score, 2);
            }
        }

        if(num1 > 0)
        {
            mean1 /= num1;
            var1 /= num1;
        }
        if(num2 > 0)
        {
            mean2 /= num2;
            var2 /= num2;
        }
        var1 -= pow(mean1,2); var2 -= pow(mean2,2);

        // ISOLATED HONEST USER
        double ratio = 1.5;
        double score =  pow(var1*var2, ratio/2)/pow(sum2,2/ratio);
        //double score =  sqrt(var1)*sqrt(var2)/pow(sum2,2);
        //cout<< i <<"th node: " << score << " " << sqrt(var1) << " " <<sqrt(var2) << " " << num1 << " "<<sum1 << " " << 1/sum2<<endl;
        stepness.push_back(score);
    }
    vector<size_t> idx(stepness.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&stepness](size_t i1, size_t i2) {return stepness[i1] < stepness[i2];});

    int error = 0;
    int inject_num = rwr.size()*inject_ratio;
    for(int i = 0; i < inject_num; i++)
    {
        //cout << idx[i] << " ";
        if(idx[i] < rwr.size() - inject_num)
            error++;
    }
    cout<< double(inject_num - error)/inject_num << endl;
}

void compute_sum(vector<vector<double>>& rwr, double inject_ratio)
{
    int inject_num = rwr.size()*inject_ratio;
    vector<double> sum;
    for(int i = 0; i < rwr.size(); i++)
    {
        double partial_sum = 0;
        for(int j = 0; j < rwr[i].size() - inject_num; j++)
            partial_sum += rwr[j][i];
        sum.push_back(partial_sum);
    }
    vector<size_t> idx(sum.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&sum](size_t i1, size_t i2) {return sum[i1] < sum[i2];});

    int error = 0;
    for(int i = 0; i < inject_num; i++)
    {
        if(idx[i] < rwr.size() - inject_num)
            error++;
    }
    cout<<"False Positive: " << double(error)/inject_num << endl;
}

void compute_variance(vector<vector<double>>& rwr, double inject_ratio)
{
    int inject_num = rwr.size()*inject_ratio;
    vector<double> var;
    for(int i = 0; i < rwr.size(); i++)
    {
        double sum = 0;
        double sum2 = 0;
        for(int j = 0; j < rwr[i].size() - inject_ratio; j++)
        {
            sum += rwr[j][i];
            sum2 += rwr[j][i]*rwr[j][i];
            //sum += rwr[i][j];
            //sum2 += rwr[i][j]*rwr[i][j];
        }
        double m = sum/rwr.size();
        double v = sum2/rwr.size() - m*m;
        var.push_back(v);
    }
    vector<size_t> idx(var.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&var](size_t i1, size_t i2) {return var[i1] < var[i2];});

    int error = 0;
    for(int i = 0; i < inject_num; i++)
    {
        if(idx[i] < rwr.size() - inject_num)
            error++;
    }
    cout<<"False Positive: " << double(error)/inject_num << endl;
}

void compute_skewness(vector<vector<double>>& rwr, double inject_ratio)
{
    int inject_num = rwr.size()*inject_ratio;
    vector<double> skewness;
    for(int i = 0; i < rwr.size(); i++)
    {
        double sum = 0;
        double sum2 = 0;
        double sum3 = 0;
        for(int j = 0; j < rwr[i].size(); j++)
        {
            sum += rwr[j][i];
            sum2 += pow(rwr[j][i], 2);
            sum3 += pow(rwr[j][i], 3);
        }
        double m = sum/rwr[i].size();
        double d = sqrt(sum2/rwr[i].size() - m*m);
        double skew = (sum3 - 3*m*pow(d,2)-pow(m,3))/pow(d,3);
        skewness.push_back(skew);
    }
    vector<size_t> idx(skewness.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&skewness](size_t i1, size_t i2) {return skewness[i1] > skewness[i2];});

    int error = 0;
    for(int i = 0; i < inject_num; i++)
    {
        if(idx[i] < rwr.size() - inject_num)
            error++;
    }
    cout<<"False Positive: " << double(error)/inject_num << endl;
}

void compute_score_ratio(vector<vector<double>>& rwr, int normal_edge, int fake_edge, int camo_edge, double inject_ratio)
{
    int inject_num = rwr.size() * inject_ratio;

    double sum1 = 0;
    double sum2 = 0;
    for(int i = 0; i < rwr.size() - inject_num; i++)
    {
        for(int j = 0; j < rwr.size() - inject_num; j++)
            sum1 += rwr[i][j];
        for(int j = rwr.size() - inject_num; j < rwr.size(); j++)
            sum2 += rwr[i][j];
    }
    sum1 /= (rwr.size() - inject_num);
    sum2 /= (rwr.size() - inject_num);
    cout<< "Experimental Ratio (S1): " << sum1/sum2 << endl;

    sum1 = sum2 = 0;
    for(int i = rwr.size() - inject_num; i < rwr.size(); i++)
    {
        for(int j = 0; j < rwr.size() - inject_num; j++)
            sum1 += rwr[i][j];
        for(int j = rwr.size() - inject_num; j < rwr.size(); j++)
            sum2 += rwr[i][j];
    }
    sum1 /= inject_num;
    sum2 /= inject_num;
    cout<< "Experimental Ratio (S2): " << sum1/sum2 << endl;

    double s1, s2;
    double pa = float(camo_edge) / (normal_edge + camo_edge);
    double pc = float(camo_edge) / (fake_edge + camo_edge);
    s1 = 0.15; s2 = 0;
    sum1 = sum2 = 0;
    for(int i = 0; i < 5; i++)
    {
        double temp_s1 = (1-pa)*s1 + pa*pc*s1 + pc*s2;
        double temp_s2 = pa*(1-pc)*s1 + (1-pc)*s2;
        s1 = 0.75*temp_s1; s2 = 0.75*temp_s2;
        sum1 += s1;
        sum2 += s2;
    }
    cout<< "Theoretical Ratio (S1): " << sum1/sum2 << endl;

    s1 = 0; s2 = 0.15;
    sum1 = sum2 = 0;
    for(int i = 0; i < 5; i++)
    {
        double temp_s1 = (1-pa)*s1 + pa*pc*s1 + pc*s2;
        double temp_s2 = pa*(1-pc)*s1 + (1-pc)*s2;
        s1 = 0.75*temp_s1; s2 = 0.75*temp_s2;
        sum1 += s1;
        sum2 += s2;
    }
    cout<< "Theoretical Ratio (S2): " << sum1/sum2 << endl;
}
