#include "ns3/core-module.h"

#include <string>
#include <algorithm>
#include <cmath>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ns3RNG");

// Normalizes a vector in-place with upper bound
void normalize_vector(std::vector<double> &vec, double upper){
    double max = *std::max_element(vec.begin(), vec.end());
    double min = *std::min_element(vec.begin(), vec.end());
    double delta = max - min;
    for (uint i = 0; i < vec.size(); i++){
        vec[i] = ((vec[i] - min)/(delta))*upper;
    }
}

void writeToFile(std::string filename, std::vector<double> data){
    std::ofstream output;
    std::string content;
    output.open(filename + ".csv", std::fstream::out);

    if(output.is_open()){
        if(data.size() > 0){
            for(uint i = 0; i < data.size(); i++){
                content += std::to_string(data[i]);
                content += "\n";
            }
        }
        //Remove trailing whitespace and comma
        content = content.substr(0, content.size()-2);
        output << content;

        //Close file
        std::cout << "Succesfully generated " << data.size() << " values to " << filename << ".csv" << std::endl;
        output.close();
    }
}

/*
int seed - affects the modulo operation to generate different numbers
int m - the modulus
int a - the multiplier
int c - the increment
double upper - upper bound
bool norm - normalize to [0,1]
int n - the amount of generated values
Return vector with LCG generated numbers
*/
std::vector<double> LCG(int seed, int m, int a, int c, double upper, int n){
    std::vector<double> generatedValues;

    //Initialize the seed state
    int value = seed;

    for(int i = 0; i < n; i++){
        value = (int)(a * value + c) % (int)m;

        generatedValues.push_back((double)value);
    }

    normalize_vector(generatedValues, upper);
    return generatedValues;
}

/*
double min - lower bound
double max - upper bound
int n - the amount of generated values
Return vector with URV generated values
*/
std::vector<double> URV(double min, double max, int n){
    std::vector<double> generatedValues;

    Ptr<UniformRandomVariable> URV = CreateObject<UniformRandomVariable>();
    URV->SetAttribute("Min", DoubleValue(min));
    URV->SetAttribute("Max", DoubleValue(max));

    for(int i = 0; i < n; i++){
        generatedValues.push_back(URV->GetValue());
    }

    return generatedValues;
}

/*
double mean - mean value 
double bound - upper bound
Return vector with ERV generated values
*/
std::vector<double> ERV(double mean, double bound, int n){
    std::vector<double> generatedValues;

    Ptr<ExponentialRandomVariable> ERV = CreateObject<ExponentialRandomVariable>();
    ERV->SetAttribute("Mean", DoubleValue(mean));
    ERV->SetAttribute("Bound", DoubleValue(bound));

    for(int i = 0; i < n; i++){
        generatedValues.push_back(ERV->GetValue());
    }

    return generatedValues;
}

int main (int argc, char *argv[]){
    int n = 10000;
    bool run_lcg = true;
    bool run_urv = true;
    bool run_erv = true;

    CommandLine cmd;
    cmd.AddValue("n","Number of generated values", n);
    cmd.AddValue("run_lcg","Run linear congruentail generator", run_lcg);
    cmd.AddValue("run_urv","Run linear congruentail generator", run_urv);
    cmd.AddValue("run_erv","Run linear congruentail generator", run_erv);

    cmd.Parse(argc, argv);

    if(run_lcg){
        //Seed, modulus, multiplier, increment, upper bound, amount
        std::vector<double> LCG_random_values = LCG(1, (std::pow(2,31)-1), std::pow(7, 5), 0, 1.0, n); //Uniform
       //std::vector<double> LCG_random_values = LCG(1, 100, 13, 1, 1.0, n);
        writeToFile("LCG", LCG_random_values);
    }

    if(run_urv){
        std::vector<double> URV_random_values = URV(0,1, n);
        writeToFile("URV", URV_random_values);
    }

    if(run_erv){
        std::vector<double> ERV_random_values = ERV(0.5, 1.0, n);
        writeToFile("ERV", ERV_random_values);
    }
    
    return 0;
}