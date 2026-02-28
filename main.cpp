#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

int main() {
    ifstream file("SanityCheck_DataSet__1.txt");
    string s;

    int classes;
    double values;

    vector<vector<double>> class1;
    vector<vector<double>> class2;

    while (getline(file, s)) {
        stringstream ss(s);
        ss >> classes;

        vector<double> features;
        while (ss >> values) {
            features.push_back(values);
        }

        if (classes == 1) 
            class1.push_back(features);
        else    
            class2.push_back(features);
    }
}