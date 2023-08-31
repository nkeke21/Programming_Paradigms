#!/usr/bin/env python

import random # for seed, random
import sys    # for stdout



################################### TEST PART ##################################
################################################################################

# Tests align strands and scores
# Parameters types:
#    score          =  int   example: -6
#    plusScores     = string example: "  1   1  1"
#    minusScores    = string example: "22 111 11 "
#    strandAligned1 = string example: "  CAAGTCGC"
#    strandAligned2 = string example: "ATCCCATTAC"
#
#   Note: all strings must have same length
def test(score, plusScores, minusScores, strandAligned1, strandAligned2):
    print("\n>>>>>>START TEST<<<<<<")

    if testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
        sys.stdout.write(">>>>>>>Test SUCCESS:")
        sys.stdout.write("\n\t\t" + "Score: "+str(score))
        sys.stdout.write("\n\t\t+ " + plusScores)
        sys.stdout.write("\n\t\t  " + strandAligned1)
        sys.stdout.write("\n\t\t  " + strandAligned2)
        sys.stdout.write("\n\t\t- " + minusScores)
        sys.stdout.write("\n\n")
    else:
        sys.stdout.write("\t>>>>!!!Test FAILED\n\n")


# converts character score to int
def testScoreToInt(score):
    if score == ' ':
        return 0
    return int(score)


# computes sum of scores
def testSumScore(scores):
    result = 0
    for ch in scores:
        result += testScoreToInt(ch)
    return result


# test each characters and scores
def testValidateEach(ch1, ch2, plusScore, minusScore):
    if ch1 == ' ' or ch2 == ' ':
        return plusScore == 0 and minusScore == 2
    if ch1 == ch2:
        return plusScore == 1 and minusScore == 0
    return plusScore == 0 and minusScore == 1


# test and validates strands
def testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
    if len(plusScores) != len(minusScores) or len(minusScores) != len(strandAligned1) or len(strandAligned1) != len(
            strandAligned2):
        sys.stdout.write("Length mismatch! \n")
        return False

    if len(plusScores) == 0:
        sys.stdout.write("Length is Zero! \n")
        return False

    if testSumScore(plusScores) - testSumScore(minusScores) != score:
        sys.stdout.write("Score mismatch to score strings! TEST FAILED!\n")
        return False
    for i in range(len(plusScores)):
        if not testValidateEach(strandAligned1[i], strandAligned2[i], testScoreToInt(plusScores[i]),
                                testScoreToInt(minusScores[i])):
            sys.stdout.write("Invalid scores for position " + str(i) + ":\n")
            sys.stdout.write("\t char1: " + strandAligned1[i] + " char2: " +
                             strandAligned2[i] + " +" + str(testScoreToInt(plusScores[i])) + " -" +
                             str(testScoreToInt(minusScores[i])) + "\n")
            return False

    return True

######################## END OF TEST PART ######################################
################################################################################

memory = {}
# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(strand1, strand2):
	curr = strand1 + " " + strand2

	if(curr in memory):
		return memory[curr]
	# if one of the two strands is empty, then there is only
	# one possible alignment, and of course it's optimal
	
	best = {"score": 0, "beststrand1": "", "beststrand2": ""}

	if len(strand1) == 0:
		for i in range(len(strand2)):
			best["beststrand1"] += " "
			best["beststrand2"] += strand2[i]
			best["score"] -= 2
		memory[curr] = best
		return best

	if len(strand2) == 0: 
		for i in range(len(strand1)):
			best["beststrand2"] += " "
			best["beststrand1"] += strand1[i]
			best["score"] -= 2
		memory[curr] = best
		return best
	
	# There's the scenario where the two leading bases of
	# each strand are forced to align, regardless of whether or not
	# they actually match.
	
	bestWith = findOptimalAlignment(strand1[1:], strand2[1:])

	best["beststrand1"] = strand1[0] + bestWith["beststrand1"]
	best["beststrand2"] = strand2[0] + bestWith["beststrand2"]
	best["score"] = bestWith["score"]

	if strand1[0] == strand2[0]:
		best["score"] += 1
		memory[curr] = best
		return best # no benefit from making other recursive calls
	
	best["score"] = bestWith["score"] - 1

	# It's possible that the leading base of strand1 best
	# matches not the leading base of strand2, but the one after it.
	bestWithout = findOptimalAlignment(strand1, strand2[1:])
	score = bestWithout["score"]
	score -= 2 # penalize for insertion of space

	if score > best["score"]:
		best["beststrand1"] = " " + bestWithout["beststrand1"]
		best["beststrand2"] = strand2[0] + bestWithout["beststrand2"]
		best["score"] = score

	# opposite scenario
	bestWithout = findOptimalAlignment(strand1[1:], strand2)
	score = bestWithout["score"]
	score -= 2 # penalize for insertion of space

	if score > best["score"]:
		best["beststrand2"] = " " + bestWithout["beststrand2"]
		best["beststrand1"] = strand1[0] + bestWithout["beststrand1"]
		best["score"] = score

	memory[curr] = best
	return best
	
	

# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
	assert minlength > 0, \
	       "Minimum length passed to generateRandomDNAStrand" \
	       "must be a positive number" # these \'s allow mult-line statements
	assert maxlength >= minlength, \
	       "Maximum length passed to generateRandomDNAStrand must be at " \
	       "as large as the specified minimum length"
	strand = ""
	length = random.choice(xrange(minlength, maxlength + 1))
	bases = ['A', 'T', 'G', 'C']
	for i in xrange(0, length):
		strand += random.choice(bases)
	return strand

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.
def printAlignment(score, out = sys.stdout):
	out.write("Optimal alignment score is " + str(score) + "\n")

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.

def helper(list):
	plus = ""
	minus = ""
	s1 = list["beststrand1"]
	s2 = list["beststrand2"]
	for i in range(len(s1)):
		if(s1[i] == s2[i]):
			plus += "1"
			minus += " "
		elif(s1[i] == " " or s2[i] == " "):
			plus += " "
			minus += "2"
		else: 
			plus += " "
			minus += "1"
	print(plus)
	print(s1)
	print(s2)	
	print(minus)
	test(list["score"],
			plus,
			minus,
			s1,
			s2)


def main():
	test(-4,
			"  11 1 1 11 ",
			"12  2 2 1  2",
			"G ATCG GCAT ",
			"CAAT GTGAATC")

	while (True):
		memory["pairs"] = []
		memory["lsts"] = []
		sys.stdout.write("Generate random DNA strands? ")
		answer = sys.stdin.readline()
		if answer == "no\n": break
		strand1 = generateRandomDNAStrand(500, 600)
		strand2 = generateRandomDNAStrand(500, 600)
		sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
		print("--------------------------------------")
		sys.stdout.write("                            " + strand2 + "\n")
		alignment = findOptimalAlignment(strand1, strand2)
		printAlignment(alignment["score"])
		helper(alignment)
		
if __name__ == "__main__":
  main()
