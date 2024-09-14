# build all word lists
echo "Creating word lists"
python createWordLists.py ../WordShift/resources/words/
echo "Creating frequency list"
python createFrequencyList.py ../WordShift/resources/words/
