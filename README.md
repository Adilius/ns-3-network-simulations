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
Run random variable generation 
- `../waf --run "ns3RNG --n=100000 --run_lcg=true --run_urv=true --run_erv=true --run_poi=true"`

Plot LCG, URV, ERV

- `python RNGplot.py ./LCG.csv`

Plot Poisson

- `python Poissonplot.py ./Poisson.csv`

## Results
![LCG](https://user-images.githubusercontent.com/43440295/119241584-d4b34780-bb57-11eb-9a82-ad91720f7595.png)
![LCG_UNIFORM](https://user-images.githubusercontent.com/43440295/119853908-e5c1d700-bf10-11eb-8207-9114cefb69b1.png)
![URV](https://user-images.githubusercontent.com/43440295/119241586-d54bde00-bb57-11eb-870a-252e4502e896.png)
![Poisson](https://user-images.githubusercontent.com/43440295/119853949-ef4b3f00-bf10-11eb-8a9e-76f68a6de3ec.png)
![ERV](https://user-images.githubusercontent.com/43440295/119853876-dd699c00-bf10-11eb-89ce-6c80592dee12.png)


