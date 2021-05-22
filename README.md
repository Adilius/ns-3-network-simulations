# ns-3-network-simulations
Network simulations written in ns-3

## Installation
From ns-3-dev directory, run these commands:

SSH
```shell
cd scratch
git init
git remote add origin git@github.com:Adilius/ns-3-network-simulations.git
git fetch --all --prune
git checkout main
```

HTTPS
```shell
cd scratch
git init
git remote add origin https://github.com/Adilius/ns-3-network-simulations.git
git fetch --all --prune
git checkout main
```

## Run
`./waf --run "ns3RNG"`
`python RNGplot.py ./LCG.csv`