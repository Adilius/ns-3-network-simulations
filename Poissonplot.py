import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import csv
import statistics
from scipy.stats import poisson
from scipy.optimize import curve_fit
from scipy.special import factorial
from scipy.stats import poisson
import sys
import itertools

def fit_function(k, lamb):
    return poisson.pmf(k, lamb)


if(len(sys.argv) > 1):
    dataFilename = str(sys.argv[1])
    print('Generating plot from file ', dataFilename)

    with open(dataFilename, newline='') as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)   # Read and convert all rows into floats
        data = list(reader)
        unpackedData = list(itertools.chain(*data))
        #unpackedData = [x for l in data for x in l] #Unpack list
        unsignedData = [x for x in unpackedData if x >= 0]  #Remove unsigned floats

        bins = np.arange(11) - 0.5
        entries, bin_edges, patches = plt.hist(unsignedData, bins=bins, density=True, label='Data')

        # Calculate bin centres
        bin_middles = 0.5 * (bin_edges[1:] + bin_edges[:-1])

        #Fit with curve_fit
        parameters, cov_matrix = curve_fit(fit_function, bin_middles, entries)

        # Plot with poisson-deviation with fitted parameter
        x_plot = np.arange(0, 4)

        plt.plot(
            x_plot,
            fit_function(x_plot, *parameters),
            linestyle='',
        )

        #print(type(data))
        #print(data)
        #lambd = np.mean(unsignedData) #get the mean value


        plt.ylabel('Frequency')
        plt.xlabel('Value')
        plt.title("Poisson")
        plt.legend()
        plt.show()

