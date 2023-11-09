#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath> 
// Lin-alg library 
#include <Eigen/Dense>

#include <ctime> 
#include <algorithm> 
#include <cstdlib> 

using namespace std;

vector<vector<double>> loadCSV(const string& filename) {
    vector<vector<double>> data;
    vector<double> featureMeans;  // Store means of each feature
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Unable to open the file." << endl;
        return data;
    }

    string line;
    bool skipHeader = true;  // Skip the first line (header)

    while (getline(file, line)) {
        if (skipHeader) {
            skipHeader = false;
            continue;  // Skip the header row
        }

        vector<double> row;
        istringstream lineStream(line);
        string cell;
        size_t colIdx = 0;  // Track the current column index

        while (getline(lineStream, cell, ',')) {
            if (cell == "NA") {
                // Handle "NA" value: replace with the mean of the corresponding feature
                if (colIdx < featureMeans.size()) {
                    row.push_back(featureMeans[colIdx]);
                } else {
                    cerr << "Error: More missing values than feature means." << endl;
                    return data;  // Abort further processing on error
                }
            } else {
                try {
                    double value = stod(cell);
                    row.push_back(value);
                } catch (const std::invalid_argument& e) {
                    cerr << "Error: Invalid numeric value in CSV." << endl;
                    return data;  // Abort further processing on error
                } catch (const std::out_of_range& e) {
                    cerr << "Error: Numeric value out of range in CSV." << endl;
                    return data;  // Abort further processing on error
                }
            }

            colIdx++;
        }

        // Update feature means with the current row
        if (featureMeans.size() < row.size()) {
            featureMeans.resize(row.size(), 0.0);
        }
        for (size_t i = 0; i < row.size(); ++i) {
            featureMeans[i] += row[i];
        }

        data.push_back(row);
    }

    // Calculate means of each feature
    for (size_t i = 0; i < featureMeans.size(); ++i) {
        featureMeans[i] /= data.size();
    }

    file.close();
    return data;
}

//------------------Normalization---------------------------------------

// Calculate the mean of a vector
double mean(const vector<double>& values) {
    double sum = 0.0;
    for(double value : values) {
        sum += value; 
    }
    return sum / values.size(); 
}

// Calculate the std of a vector
double stdev(const vector<double>& values) {
    double meanVal = mean(values); 
    double squared_diff = 0.0;
    for(double value : values) {
        double diff = value - meanVal; 
        squared_diff += diff * diff; 
    }
    return sqrt(squared_diff / values.size()); 
}

// Standardize a vector (Z-score standardization
void standardize(vector<double>& values) {
    double meanVal = mean(values);
    double sigma = stdev(values);
    for(double& value : values){
        value = (value - meanVal) /sigma; 
    }
}

vector<vector<double>> standardizeCols(vector<vector<double>> data) {
    for(size_t col = 0; col < data[0].size(); ++col){
        vector<double> feature; 
        for(size_t row = 0; row < data.size(); ++row){
            feature.push_back(data[row][col]);
        }
        standardize(feature);
        for(size_t row = 0; row < data.size(); ++row){
            data[row][col] = feature[row]; 
        }
    }
    return data;
}

// --------------Splitting data -------------------------------------
using namespace std;

//Split data into training and testing sets with a given split percentage
void splitData(const vector<vector<double>>& data, double splitPercentage, vector<vector<double>>& trainingData, vector<vector<double>>& testingData) {
   
    // Seed the random number generator for shuffling
    srand(time(nullptr));

    // Create an index vector to shuffle data
    vector<size_t> indices(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        indices[i] = i;
    }

    // Shuffle the indices
    random_shuffle(indices.begin(), indices.end());

    // Determine the split point
    size_t splitIndex = static_cast<size_t>(splitPercentage * data.size());

    // Split the data
    trainingData.clear();
    testingData.clear();

    for (size_t i = 0; i < data.size(); ++i) {
        if (i < splitIndex) {
            trainingData.push_back(data[indices[i]]);
        } else {
            testingData.push_back(data[indices[i]]);
        }
    }
}


void printCSV(vector<vector<double>> data) {
    for (const vector<double>&row : data) {
        for(double value : row){
            cout << value << " "; 
        }
        cout << endl; 
    }
    return; 
}

// ------------------ Regression (Least Squares)-------------------

vector<vector<double>> transpose(const vector<vector<double>>& matrix) {
    int rows = matrix.size(); 
    int cols = matrix[0].size(); 
    // Result Vector
    vector<vector<double>> result(cols, vector<double>(rows, 0.0));
    // Transpose
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            result[j][i] = matrix[i][j]; 
        }
    }
    return result
}



int main() {
    // Example usage
    vector<vector<double>> trainData; 
    vector<vector<double>> testData; 

    // Load data
    string filename = "HousingData.csv";
    vector<vector<double>> data = loadCSV(filename);

    // Standardize data 
    data = standardizeCols(data);

    // Split data 70/30
    splitData(data, 0.7, trainData, testData);


    return 0;
}
