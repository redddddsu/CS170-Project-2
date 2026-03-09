#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

double euclidean_distance(vector<double> &classes, vector<double> &input) {
    double distance = 0;
    for (int i = 0; i < classes.size(); i++) {
        distance += pow(classes[i] - input[i], 2);
    }
    return sqrt(distance);
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

//This function reduces the class feature so it will only have the selective features
vector<vector<double>> reduce_feature(vector<vector<double>> &cls, vector<int> &features) {
    vector<vector<double>> reduce_class(cls.size(), vector<double>(features.size()));

    for (int i = 0; i < cls.size(); i++) {
        for (int j = 0; j < features.size(); j++) {
            reduce_class[i][j] = cls[i][features[j]];
        }
    }
    return reduce_class;
}
void forward_selection(vector<vector<double>> &class1, vector<vector<double>> &class2) {
    vector<double> accuracies;
    int highest_accuracy_feature;
    double highest_accuracy;
    vector<int> features;
    vector<int> best_features;
    vector<bool> visited(class1[0].size(), false);

    for (int i = 0; i < class1[0].size(); i++) {
        highest_accuracy = -1;

        for (int j = 0; j < class1[0].size(); j++) {
            if (visited[j])
                continue;
            
            features = best_features;
            features.push_back(j);
            vector<vector<double>> reduced_class1 = reduce_feature(class1, features);
            vector<vector<double>> reduced_class2 = reduce_feature(class2, features);
    
            double accuracy = cross_validation(reduced_class1, reduced_class2);
            if (highest_accuracy < accuracy) {
                highest_accuracy = accuracy;
                highest_accuracy_feature = j;
            }
        }
        visited[highest_accuracy_feature] = true;
        best_features.push_back(highest_accuracy_feature);

        cout << "Feature: " << i + 1 << endl;
        cout << "Accuracy: " << highest_accuracy << endl;
        cout << "Selected Features: {";
        for (int j = 0; j < best_features.size(); j++) {
            cout << best_features[j];
            if (j != best_features.size() - 1) cout << ", ";
        }
        cout << "}" << endl << endl;
    }
}

void backward_elimation(vector<vector<double>> &class1, vector<vector<double>> &class2) {
    vector<int> best_features;
    vector<int> features;
    int highest_accuracy_feature;
    double highest_accuracy;

    for (int i = 0; i < class1[0].size(); i++) {
        best_features.push_back(i);
    }

    for (int i = 1; i < class1[0].size(); i++) {
        highest_accuracy = -1;

        for (int j = 0; j < class1[0].size(); j++) {
            features = best_features;
            features.erase(features.begin() + j);

            vector<vector<double>> reduced_class1 = reduce_feature(class1, features);
            vector<vector<double>> reduced_class2 = reduce_feature(class2, features);

            double accuracy = cross_validation(reduced_class1, reduced_class2);
            if (highest_accuracy < accuracy) {
                highest_accuracy = accuracy;
                highest_accuracy_feature = j;
            }
        }
        best_features.erase(best_features.begin() + highest_accuracy_feature);

        cout << "Feature: " << i + 1 << endl;
        cout << "Accuracy: " << highest_accuracy << endl;
        cout << "Selected Features: {";
        for (int j = 0; j < best_features.size(); j++) {
            cout << best_features[j];
            if (j != best_features.size() - 1) cout << ", ";
        }
        cout << "}" << endl << endl;
    }
}


int main() {
    ifstream file("Small_dataset/CS170_Small_DataSet__20.txt");
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
            if (values != 0)
                features.push_back(values);
        }

        if (classes == 1) 
            class1.push_back(features);
        else    
            class2.push_back(features);
        
    }

    forward_selection(class1, class2);
    // for (int i = 0; i < class2[0].size(); i++) {
    //     cout << class2[0][i] << endl;
    // }

}