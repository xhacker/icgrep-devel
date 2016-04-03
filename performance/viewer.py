######################
#Viewer for *-perf.json files
######################

import argparse
import json
import numpy as np
import matplotlib.pyplot as plt
import os
import os.path


stat_set = set()
def prepare_data(perf1, perf2):
    #key: 'cycles:u', 'instructions:u'
    #value: perf data
    y1 = {}
    y2 = {}

    # program, file, expression, stat
    p1 = perf1.keys()[0]
    p2 = perf2.keys()[0]

    for f in perf1[p1]:
        for e in perf1[p1][f]:
            for s in perf1[p1][f][e]:
                if s not in y1:
                    y1[s] = []
                if s not in y2:
                    y2[s] = []

                stat_set.add(s)
                y1[s].append(perf1[p1][f][e][s])
                y2[s].append(perf2[p2][f][e][s])

    return y1, y2

if __name__ == "__main__":
    #Parse args
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--text", help="generate text-based view", action="store_true")
    parser.add_argument("-l", "--latex", help="generate latex table of the view", action="store_true")
    args = parser.parse_args()

    file_list = [f for f in os.listdir('.') if os.path.isfile(f) and "-perf.json" in f]
    print "Found the following perf data:"
    for f in enumerate(file_list):
        print "{0} ... {1}".format(f[0], f[1])
    f1 = input("Choose the 1st file:")
    f2 = input("Choose the 2nd file:")

    perf1 = json.load(fp = open(file_list[f1], 'r'))
    perf2 = json.load(fp = open(file_list[f2], 'r'))

    y1,y2 = prepare_data(perf1, perf2)

    if len(y1) != len(y2):
        print "Error: number of data doesn't match"
        exit(0)

    stat_set = list(stat_set)
    print "Found the following stats available:"
    for f in enumerate(stat_set):
        print "{0} ... {1}".format(f[0], f[1])

    while True:
        s1 = input("Choose the stat (any illegal number to quit):")
        if s1 < 0 or s1 >= len(stat_set):
            exit(0)

        stat = stat_set[s1]

        if not args.text and not args.latex:
            ind = np.arange(len(y1[stat]))

            width = (2 - 0.4 * 2) / 2.0

            p1 = plt.bar(ind * 2, y1[stat], width=width, color='y')
            p2 = plt.bar(ind * 2 + width, y2[stat], width=width, color='r')
            plt.ylabel(stat)
            plt.legend((p1[0], p2[0]), (file_list[f1], file_list[f2]))

            plt.show()

        if args.text:
            row_format = "{:>20}" * 3
            print row_format.format("y1", "y2", "y2 : y1")
            print "-" * 20 * 3
            for i in xrange(0, len(y1[stat])):
                print row_format.format(y1[stat][i], y2[stat][i],
                                        y2[stat][i] * 1.0 / y1[stat][i])
            print "-" * 20 * 3
            print 'y1 = ' + file_list[f1]
            print 'y2 = ' + file_list[f2]

