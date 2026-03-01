#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;



double euclidean_distance(vector<double> &classes, vector<double> &input) {
    return sqrt(pow(classes[0] - input[0], 2) + pow(classes[1] - input[1], 2));
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

    if (class1_nearest < class2_nearest) 
        return 1;
    return 2;
}

//Splitting my data into 80% training and 20% testing
double cross_validation(vector<vector<double>> &class1, vector<vector<double>> &class2) {
    double k = 5;
    int fold_class1 = class1.size() / k;
    int fold_class2 = class2.size() / k;

    vector<vector<double>> training_class1;
    vector<vector<double>> testing_class1;
    vector<vector<double>> training_class2;
    vector<vector<double>> testing_class2;

    double accuracies = 0;

    for (int i = 0; i < k; i++) {
        int start1 = i * fold_class1;
        int end1 = start1 + fold_class1;
        
        int start2 = i * fold_class2;
        int end2 = start2 + fold_class2;

        for (int j = 0; j < class1.size(); j++) {
            if (j >= start1 && j < end1) 
                testing_class1.push_back(class1[j]);
            else    
                training_class1.push_back(class1[j]);
        }

         for (int j = 0; j < class2.size(); j++) {
            if (j >= start2 && j < end2) 
                testing_class2.push_back(class2[j]);
            else    
                training_class2.push_back(class2[j]);

        }

        double correct_prediction = 0;
        for (int j = 0; j < testing_class1.size(); j++) {
            int predicts = nearest_neighbor(training_class1, training_class2, testing_class1[j]);
            if (predicts == 1) {
                correct_prediction++;
            }
        } 
        for (int j = 0; j < testing_class2.size(); j++) {
            int predicts = nearest_neighbor(training_class1, training_class2, testing_class2[j]);
            if (predicts == 2) {
                correct_prediction++;
            }
        } 
        accuracies += correct_prediction / (testing_class1.size() + testing_class2.size());
        training_class1.clear();
        testing_class1.clear();
        training_class2.clear();
        testing_class2.clear();
    }
    return (accuracies / k) * 100;
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

    cout << "Cross Validation: " << cross_validation(class1, class2) << endl;

}