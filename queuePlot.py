import matplotlib.pyplot as plt
import csv
import sys

x = []
y = []

#python queuePlot.py ../queue_P2P.csv

#If we recieved program argument
if(len(sys.argv) > 1):

    #Retrive filename
    dataFilename = str(sys.argv[1])

    #Open file and store each value in list
    with open(dataFilename, 'r') as datafile:
        plots = csv.reader(datafile, delimiter=',')
        for row in plots:
            x.append(float(row[0]))
            y.append(int(row[1]))

    #Create plot and show
    plt.plot(x,y)
    plt.xlabel('Time')
    plt.ylabel('Queue size')
    plt.title('Graph plotted from: ' + dataFilename)
    plt.legend()
    plt.show()
