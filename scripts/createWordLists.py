# Build alphabetical word lists organized by letter count
# Combines the system dictionary and custom word list
import time
import sys

# thanks to http://stackoverflow.com/questions/287871/print-in-terminal-with-colors-using-python
# for the information about color characters
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

def buildFile( wordSource, excludeSource, wordLen ):
	matches = set()
	output_dir = sys.argv[1]
	for word in wordSource:
		if( len(word) == wordLen and not word[0].isupper() ):
			matches.add( word.lower() )
	f = open( output_dir + str(wordLen) + "-letters", 'w' )
	for word in excludeSource:
		if(len(word) == wordLen and not word in matches and not word[0] == "#"):
			print bcolors.WARNING
			print word + " is not in the base dictionary."
			print bcolors.ENDC
		matches.discard( word )
	matches = list(matches)
	matches.sort()
	f.write( '\n'.join( matches ) )
	f.close()
	print "%s Letters: %s words" % (wordLen, len( matches ))
	return len( matches )

def buildFiles():
	start = time.time()
	wordList = []
	excludeList = []
	sources = ['enable_words','custom_words']
	# profanity contains swears and slurs
	# exclude_words are non-dictionary words that are too weird to include
	excludes = ['profanity', 'exclude_words']
	for source in sources:
		words = open(source)
		new_words = [w.strip() for w in words if not w[0] == "#"]
		redundant_words = '\t'.join(set(wordList).intersection( new_words ))
		if( len(redundant_words) > 0 ):
			print bcolors.WARNING
			print "Redundant words in %s:\n%s" % (source, redundant_words)
			print bcolors.ENDC
		wordList += new_words
		words.close()
	for source in excludes:
		words = open(source)
		excludeList += [w.strip() for w in words]
		words.close()
	includedWordCount = 0
	for wordLength in range( 4, 10 ):
		includedWordCount += buildFile( wordList, excludeList, wordLength )
	finish = time.time()
	print "Finished building word files in %s seconds" % (finish - start)
	print "Using %s of %s words" % (includedWordCount, len(wordList) )

buildFiles();