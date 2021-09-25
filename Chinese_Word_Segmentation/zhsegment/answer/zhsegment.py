import re, string, random, glob, operator, heapq, codecs, sys, optparse, os, logging, math
from functools import reduce
from collections import defaultdict
from math import log10

class Entry:
    def __init__(self,word, start_position, log_probability, back_pointer):
        self.word = word
        self.start_position = start_position
        self.log_probability = log_probability
        self.back_pointer = back_pointer
    def __lt__(self, other):
        return (self.log_probability > other.log_probability)

    def __eq__(self, other):
        return (self.word, self.start_position, self.log_probability) == (other.word, other.start_position, other.log_probability)
    
class Segment:

    def __init__(self, Pw, PwBigram):
        self.Pw = Pw
        self.PwBigram = PwBigram

    def segment(self, text):
        "Return a list of words that is the best segmentation of text."
        heap = []
        chart = {}
        JM_Lambda = 0.85 #used in Jelinek-Mercer Smoothing
        if not text: return []
        ## Initialize the heap ##
        for i in range(len(text)):
            w = text[0:i+1] #push words into initial heap
            if len(w) <= 7: #length as 7 to limit threshold
                #print(w)
                heapq.heappush(heap, Entry(w,0,log10(self.Pw(w)),None))

        for i in range(0, len(text)): #initialize the chart dictionary
            chart[i] = Entry(None,None,None,None)
            
        ## Iteratively fill in chart[i] for all i ##
        while (len(heap))>0: #while heap is not empty
            entry = heapq.heappop(heap) #pop the top entry
            endindex = len(entry.word)-1 + entry.start_position #assign the end index
            
            #print("entry word is: ", entry.word)
            #print("endindex is: ", endindex)
            
            if (chart[endindex].back_pointer is not None): #if there is a previous entry
                preventry = chart[endindex].back_pointer #set preventry to the previous entry
                
                #print("entry word to be compared is: ", entry.word, "entry log: ", entry.log_probability)
                #print("chart[", endindex, "] back pointer word is: ", preventry.word, "prev entry log:", preventry.log_probability)
                #print("entry log: ", entry.log_probability)
                #print("prev entry log:", preventry.log_probability)
                
                if entry.log_probability > preventry.log_probability: #if current probability > prev probability
                    chart[endindex] = entry  #set current to entry
                else:
                    continue
            else:
                chart[endindex] = entry #set chart
                
                #print("chart[", endindex, "] word is: ", entry.word)
                #print("entry.backpointer = ", entry.back_pointer)
                #print("chart[endindex].log_probability = ", chart[endindex].log_probability)
            
            for i in range(endindex+1, len(text)):
                nw = text[endindex+1 : i+1]
                if len(nw) <= 7: #set 7 as a threashold to limit number of characters
                    try: #use smoothed bigram probability when available, if not use unigram probability
                        newentry = Entry(nw, endindex+1, entry.log_probability + log10(((JM_Lambda*(self.PwBigram[entry.word + " " + nw]/self.Pw[entry.word])) + ((1-JM_Lambda)*self.Pw(entry.word)))), entry)
                    except KeyError:
                        newentry = Entry(nw, endindex+1, entry.log_probability + log10(self.Pw(nw)), entry)
                    if newentry not in heap:
                        heapq.heappush(heap, newentry)                
            
        ## Get the best segmentation ##
        finalindex = len(text) 
        finalentry = chart[finalindex-1]
        segmentation = []
        while finalentry:
            segmentation.insert(0, finalentry.word)                       
            finalentry = finalentry.back_pointer                                
        return segmentation

    def Pwords(self, words): 
        "The Naive Bayes probability of a sequence of words."
        return product(self.Pw(w) for w in words)

#### Support functions (p. 224)

def product(nums):
    "Return the product of a sequence of numbers."
    return reduce(operator.mul, nums, 1)

class Pdist(dict):
    "A probability distribution estimated from counts in datafile."
    def __init__(self, data=[], N=None, missingfn=None):
        for key,count in data:
            self[key] = self.get(key, 0) + int(count)
        self.N = float(N or sum(self.values()))
        self.missingfn = missingfn or (lambda k, N: 1./N)
    def __call__(self, key): 
        if key in self: return self[key]/self.N  
        else: return self.missingfn(key, self.N)

def datafile(name, sep='\t'):
    "Read key,value pairs from file."
    with open(name) as fh:
        for line in fh:
            (key, value) = line.split(sep)
            yield (key, value)
            
def decrease_prob_long_word(word, N):
    return (1/N) / 11580**(len(word)-1) #best long words probability after trial and error
            
if __name__ == '__main__':
    optparser = optparse.OptionParser()
    optparser.add_option("-c", "--unigramcounts", dest='counts1w', default=os.path.join('data', 'count_1w.txt'), help="unigram counts [default: data/count_1w.txt]")
    optparser.add_option("-b", "--bigramcounts", dest='counts2w', default=os.path.join('data', 'count_2w.txt'), help="bigram counts [default: data/count_2w.txt]")
    optparser.add_option("-i", "--inputfile", dest="input", default=os.path.join('data', 'input', 'dev.txt'), help="file to segment")
    optparser.add_option("-l", "--logfile", dest="logfile", default=None, help="log file for debugging")
    (opts, _) = optparser.parse_args()

    if opts.logfile is not None:
        logging.basicConfig(filename=opts.logfile, filemode='w', level=logging.DEBUG)

    PwBigram = Pdist(data=datafile(opts.counts2w),missingfn=decrease_prob_long_word)
    Pw = Pdist(data=datafile(opts.counts1w),missingfn=decrease_prob_long_word)
    segmenter = Segment(Pw, PwBigram)
    with open(opts.input) as f:
        for line in f:
            print(" ".join(segmenter.segment(line.strip())))
