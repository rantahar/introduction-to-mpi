#!/usr/bin/env python3
import sys, os
import pandas as pd
import matplotlib.pyplot as plt

def plot(data, columns, labels, filename):
    # Plot results of a scaling study from a dict or dataframe
    # and label each with the corresponding entry in labels
    # Save to filename
    plt.xlabel('Number of Ranks')
    plt.ylabel('Time')
    colours = ['red','blue']
    symbols = ['-o','-x']
    for i,[c,l] in enumerate(zip(columns,labels)):
        index = i%len(colours)
        cores = data['cores']
        col = data[c]
        plt.plot(cores, col, symbols[index], label=l, color=colours[index])
    plt.xscale('log', basex=2)
    plt.yscale('log', basey=2)
    plt.gca().set_aspect('equal', adjustable='box')
    plt.legend(loc=0, frameon=False)
    plt.savefig(filename)
    plt.clf() 

data = pd.read_csv( "timing.txt", sep=" *", header=[0] )
plot( data, ['time_512','time_2048'], ["MAX=512","MAX=2048"], "plot.png" )

