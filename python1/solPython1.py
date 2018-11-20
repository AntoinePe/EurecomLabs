# -*- coding: utf-8 -*-
"""
Created on Thu Nov 23 09:42:46 2017

@author: antoinepelletier
"""
 ##################################################################
 #                          Import                                #
 ##################################################################
import sys
import operator

 ##################################################################
 #                    Variables declaration                       #
 ##################################################################
data = sys.stdin
N = int(sys.argv[1])
myTupleList = list();


 ##################################################################
 #                 Function definitions                           #
 ##################################################################
def findN(line):
    """Return the number found in the string line"""
    ##################################################################
    #         Return number found in the string line                 #
    ##################################################################
    n=-1
    lineSplit=line.split(" ")    
    for char in lineSplit:
        try:
            n = int(char)
            break
        except ValueError:
            n=-1
    return n
        

def extractKnowledge():
    """Get the info we want"""
    ##################################################################
    #                      Ordering lines                                 #
    ##################################################################
    for line in data:
        n = findN(line)
        if n==-1 :
            print("Error somewhere")
        else :
            myTupleList.append((n,line))
            myTupleList.sort(key=operator.itemgetter(0))
            myTupleList.reverse()


##################################################################
#                      Main part                                 #
##################################################################

extractKnowledge()
lastValuePrinted = 0;
i = 0;
#There's a problem right here, i<N is always interpreted True even if i=4 and N=3
#FIXED : N was of type str and i int, needed to cast N when getting
for elt in myTupleList :
    j = elt[0]
    if i<N and lastValuePrinted == j :
        print(elt[1])
    elif i<N :
        i+=1 
        lastValuePrinted = j
        print(elt[1])
#
#for j in range(len(myTupleList)-1,0):
#    print(j)
#    elt = myTupleList[j]
#    if i<=N and lastValuePrinted==elt[0] :
#        print(elt[1])
#    elif(i<N):
#        i+=1
#        lastValuePrinted=elt[0]
#        print(elt[1])
                