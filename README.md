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

## Results
![LCG](https://user-images.githubusercontent.com/43440295/119241584-d4b34780-bb57-11eb-9a82-ad91720f7595.png)
![RVN](https://user-images.githubusercontent.com/43440295/119241585-d54bde00-bb57-11eb-8a94-93edfc7cbd9c.png)
![URV](https://user-images.githubusercontent.com/43440295/119241586-d54bde00-bb57-11eb-870a-252e4502e896.png)
