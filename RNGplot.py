import matplotlib.pyplot as plt
import pandas as pd
import sys

if(len(sys.argv) > 1):
    dataFilename = str(sys.argv[1])

    print('Generating plot from file ', dataFilename)

    data = pd.read_csv(dataFilename, sep=',', header=None, index_col=0)

    plt.hist(data, density=True, bins=5)
    #plt.hist()
    plt.ylabel('Frequency')
    plt.xlabel('Value')
    plt.title(dataFilename.rsplit('.',1)[0])

    plt.show()
