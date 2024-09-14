# Count letter frequencies
from __future__ import division
import time
import sys

def buildList():
	start = time.time()
	output_dir = sys.argv[1]
	input_dir = output_dir
	letters = []
	sources = []
	for i in range(4, 10):
		print "loading %s-letter words" % i
		sources.append( input_dir + str(i) + "-letters" )
	for source in sources:
		words = open(source)
		for w in words:
			if not w.isupper():
				letters += w.strip()
		words.close()
	letters = [word.lower() for word in letters]
	letters.sort()
	counts = {}
	for letter in letters:
		if counts.has_key(letter):
			counts[letter] += 1
		else:
			counts[letter] = 1
	bad_keys = [key for key in counts.keys() if not key.isalpha()]
	print "Removing non-alphabetical keys: %s" % bad_keys
	for key in bad_keys:
		counts.pop( key )
	total_letters = sum( counts.values() )
	for key in counts:
		counts[key] /= total_letters
		counts[key] *= 124
		if(counts[key] > 11):
			counts[key] = counts[key] - 1
		counts[key] = round( counts[key] )
		if(counts[key] == 0):
			counts[key] = 1
	sorted_keys = counts.keys()
	sorted_keys.sort()
	output = open( output_dir + 'letter-counts', 'w' )
	for key in sorted_keys:
		output.write( "%s:%i\n" % (key, counts[key]) )
	output.close()
	finish = time.time()
	print "Finished building letter counts in %s seconds" % (finish - start)

buildList()
