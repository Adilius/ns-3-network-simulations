#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ns3RNG");

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

int main (int argc, char *argv[]){

    std::cout << "Hello world." << std::endl;

    std::vector<double> LCG_random_values = LCG(1, 100, 13, 1, 20);

    for (uint i = 0; i < LCG_random_values.size(); i++){
        std::cout << LCG_random_values[i] << ' ';
    }


    return 0;
}