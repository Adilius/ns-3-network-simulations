import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from scipy.stats import poisson
import sys

def plot_poisson(df, col=0, n_pts=100):
    lm = df[col].mean() #get the mean value of your data
    poisdata = np.random.poisson(lm, n_pts)
    plt.hist(poisdata, density=True, alpha=0.5)
    plt.ylabel('Frequency')
    plt.xlabel('Value')
    plt.title("Poisson")
    plt.show()

if(len(sys.argv) > 1):
    dataFilename = str(sys.argv[1])

    print('Generating plot from file ', dataFilename)

    data = pd.read_csv(dataFilename, sep=',', header=None)

    plot_poisson(data)
