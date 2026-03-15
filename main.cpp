#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <limits>
#include <chrono>


using namespace std;
using namespace std::chrono;

// calculates th euclidean distance between a given point and all the points of a class
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

    // determines which point is closer and classifies it
    if (class1_nearest < class2_nearest) 
        return 1;
    return 2;
}

//splitting my data into 80% training and 20% testing or k = 5
double cross_validation(vector<vector<double>> &class1, vector<vector<double>> &class2) {
    double k = 5;

    int fold_class1 = class1.size() / k;
    int fold_class2 = class2.size() / k;

    vector<vector<double>> training_class1;
    vector<vector<double>> testing_class1;
    vector<vector<double>> training_class2;
    vector<vector<double>> testing_class2;

    double accuracies = 0;

    /*
    testing set takes up 20% of the dataset
    so we split the dataset into 5 sections
    each iteration will use a different section of the dataset as the testing set
    */ 
    for (int i = 0; i < k; i++) {
        // divdes the class into a training set and testing set
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

        // use nearest neighbor to see if the predict classification is the same as the actual classification
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

/* 
this function reduces the class feature so it will only have the selective features
also converts a vector to a 2D vector to use cross_valdiation and NN functions
*/
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
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    double overall_best_accuracy = -1;
    vector<int> overall_best_set;

    // need a visited set so it doesnt select the same feature twice
    vector<bool> visited(class1[0].size(), false);

    for (int i = 0; i < class1[0].size(); i++) {
        highest_accuracy = -1;

        // given the current set, check the each feature using cross_validation to determine which feature has the best heuristic to add to the set
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

        // adds to the set
        visited[highest_accuracy_feature] = true;
        best_features.push_back(highest_accuracy_feature);

        if (overall_best_accuracy < highest_accuracy) {
            overall_best_accuracy = highest_accuracy;
            overall_best_set = best_features;
        }

        cout << "Feature: " << i + 1 << endl;
        cout << "Accuracy: " << highest_accuracy << "%" << endl;
        cout << "Selected Features: {";
        for (int j = 0; j < best_features.size(); j++) {
            cout << best_features[j];
            if (j != best_features.size() - 1) cout << ", ";
        }
        cout << "}" << endl;
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Time in seconds: " << std::chrono::duration<double>(duration).count() << endl << endl;
    }

    cout << "The best accuarcy is: " << overall_best_accuracy << "%" << endl;
    cout << "These are the best features: ";
    for (int i = 0; i < overall_best_set.size(); i++) {
            cout << overall_best_set[i];
            if (i != overall_best_set.size() - 1) cout << ", ";
        }
}

void backward_elimation(vector<vector<double>> &class1, vector<vector<double>> &class2) {
    vector<int> best_features;
    vector<int> features;
    int highest_accuracy_feature;
    double highest_accuracy;
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    double overall_best_accuracy = -1;
    vector<int> overall_best_set;

    // push all the features onto the set
    for (int i = 0; i < class1[0].size(); i++) {
        best_features.push_back(i);
    }

    for (int i = 1; i < class1[0].size(); i++) {
        highest_accuracy = -1;

        // given the current set, check the each feature using cross_validation to determine which feature has the best heuristic to remove from the set
        for (int j = 0; j < best_features.size(); j++) {
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
        // remove the feature from the set
        best_features.erase(best_features.begin() + highest_accuracy_feature);

        cout << "Feature Removed: " << i << endl;
        cout << "Accuracy: " << highest_accuracy << "%" << endl;
        cout << "Selected Features: {";
        for (int j = 0; j < best_features.size(); j++) {
            cout << best_features[j];
            if (j != best_features.size() - 1) cout << ", ";
        }

        if (overall_best_accuracy < highest_accuracy) {
            overall_best_accuracy = highest_accuracy;
            overall_best_set = best_features;
        }

        cout << "}" << endl << endl;
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "Time in seconds: " << std::chrono::duration<double>(duration).count() << endl << endl;
    }
    cout << "The best accuarcy is: " << overall_best_accuracy << "%" << endl;
    cout << "These are the best features: ";
    for (int i = 0; i < overall_best_set.size(); i++) {
            cout << overall_best_set[i];
            if (i != overall_best_set.size() - 1) cout << ", ";
    }
}

int main() {
    int size;
    string data_set_id;
    int algo;
    cout << "Enter 1 for small dataset or 2 for large dataset" << endl;
    cin >> size;
    cout << endl;
    cout << "Enter which dataset (1-120)" << endl;
    cout << "Note: The one used in the report is 20" << endl;
    cin >> data_set_id;
    cout << endl;
    cout << "Enter 1 for forward selection or 2 for backward elimination" << endl;
    cin >> algo;
    cout << endl;

    string fileName;
    if (size == 1) 
        fileName = "Small_dataset/CS170_Small_DataSet__"; 
    else
        fileName = "Large_dataset/CS170_Large_DataSet__";
    fileName += data_set_id + ".txt";

    ifstream file(fileName);
    string s;

    double classes;
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
    if (algo == 1)
        forward_selection(class1, class2);
    else    
        backward_elimation(class1, class2);
}