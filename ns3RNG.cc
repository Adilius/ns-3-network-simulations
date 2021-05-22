#include "ns3/core-module.h"

#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ns3RNG");

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
int n - the amount of generated values
Return vector with LCG generated numbers
*/
std::vector<double> LCG(int seed, int m, int a, int c, int n){
    std::vector<double> generatedValues;

    for( int i = 0; i < n; i++){
        double max = 1.0 / (1.0 + (m + 1));
        seed = (int)(a * seed + c) % (int)m;

        if (seed < 0){
            seed += m;
        }

        generatedValues.push_back((double)seed * max);
    }

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
Return vector with RVN generated values
*/
std::vector<double> RVN(double mean, double bound, int n){
    std::vector<double> generatedValues;

    Ptr<ExponentialRandomVariable> RVN = CreateObject<ExponentialRandomVariable>();
    RVN->SetAttribute("Mean", DoubleValue(mean));
    RVN->SetAttribute("Bound", DoubleValue(bound));

    for(int i = 0; i < n; i++){
        generatedValues.push_back(RVN->GetValue());
    }

    return generatedValues;
}

int main (int argc, char *argv[]){

    std::vector<double> LCG_random_values = LCG(1, 100, 13, 1, 1000);
    std::vector<double> URV_random_values = URV(0,1, 1000);
    std::vector<double> RVN_random_values = RVN(0.5, 1.0, 1000);

    writeToFile("LCG", LCG_random_values);
    writeToFile("URV", URV_random_values);
    writeToFile("RVN", RVN_random_values);


    return 0;
}