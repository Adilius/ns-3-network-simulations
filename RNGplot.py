import matplotlib.pyplot as plt
import pandas as pd
import sys

if(len(sys.argv) > 1):
    dataFilename = str(sys.argv[1])

    print('Generating plot from file ', dataFilename)

    data = pd.read_csv(dataFilename, sep=',', header=None)

    print(data)

    plt.hist(data, bins='auto', density=True, histtype='step', edgecolor='black', linewidth=1)
    #plt.hist()
    plt.ylabel('Frequency')
    plt.xlabel('Value')
    plt.title(dataFilename.rsplit('.',1)[0].split('/')[1] if len(dataFilename.rsplit('.',1)[0].split('/'))>1 else dataFilename.rsplit('.',1)[0]) #Title only filename, trim away extension, and trim path if it exists

    plt.show()
