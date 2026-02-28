#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;



double euclidean_distance(vector<double> &classes, vector<double> &input) {
    return sqrt(pow(classes[0] - input[0], 2) + pow(classes[1] - input[1], 2) + pow(classes[2] - input[2], 2));
}

int nearest_neighbor(vector<vector<double>> &class1, vector<vector<double>> &class2, vector<double> input) {
    double class1_nearest = numeric_limits<double>::max();
    double class2_nearest = numeric_limits<double>::max();

    //go through each features and find the nearest neighbor given an input
    for (int i = 0; i < class1.size(); i++) {
        class1_nearest = min(euclidean_distance(class1[i], input), class1_nearest);
        
    }
    for (int i = 0; i < class2.size(); i++) {
        class2_nearest = min(euclidean_distance(class2[i], input), class2_nearest);
        

    }
    cout << "Class 1: " << class1_nearest << endl;
    cout << "Class 2: " << class2_nearest << endl;

    if (class1_nearest < class2_nearest) 
        return 1;
    return 2;
}

//Splitting my data into 80% training and 20% testing
double cross_validation(vector<vector<double>> &class1, vector<vector<double>> &class2) {
    int k = 5;
    int training_size_class1 = class1.size() * 0.8;
    int testing_size_class1 = class1.size() - training_size_class1;

    int training_size_class2 = class2.size() * 0.8;
    int testing_size_class2 = class2.size() - training_size_class2;

    int start = 0;
    int end = training_size_class1;
    vector<vector<double>> training_class1;

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < training_size_class1; j++) {
            training_class1[(j + start) % class1.size()] = class1[(j + start) % class1.size()];
        }
        start += testing_size_class1;
    }
}

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

    vector<double> input;

}