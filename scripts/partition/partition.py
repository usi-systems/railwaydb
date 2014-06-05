#!/usr/bin/env python

import time
from itertools import chain, combinations, imap
from random import gauss
import functools 
import random
import os

current_milli_time = lambda: int(round(time.time() * 1000))

def powerset(iterable):
  xs = list(iterable)
  return chain.from_iterable( combinations(xs,n) for n in range(len(xs)+1) )

def genpart(n):
    s = [1] * n 
    m = [1] * n
    yield s 

    while True:
        i = 0
        s[0] = s[0] + 1
        
        while ((i < n - 1) & (s[i] > m[i] + 1)):
            s[i] = 1
            i = i + 1
            s[i] =  s[i] + 1
            
        if (i == n - 1):
            raise StopIteration()
            
        max = s[i]
            
        i = i - 1
        while i >= 0:
           m[i] = max
           i = i - 1
                
        yield s 

    
def main():


    # schema = "1234"

    with open("x.txt", 'w') as f:
        # for i in [1, 10, 20, 30, 40, 50, 60, 70, 80, 100]:
        for i in [1, 5, 10]:

            start = current_milli_time()
            s = list(genpart(i))
            stop = current_milli_time()
            sbuf = "%d\t%f\n" % (i, stop-start)
            f.write(sbuf)

    os.system("gnuplot partition.gnu")
            
    # s = list(genpart(len(schema)))
    # p = list(powerset(schema))
    # print p
    # print s

    # random.seed()
    # g = list(imap(lambda x: float(random.randint(1, 100)), range(len(p))))
    

    # summation =  functools.reduce(lambda x, y: x + y, g)
    # normalized = map(lambda x: x/summation, g)
    # percentages = map(lambda x: x * 100, normalized)

    # print g
    # print summation
    # print normalized
    # print percentages

    
if __name__ == '__main__':
    main() 
