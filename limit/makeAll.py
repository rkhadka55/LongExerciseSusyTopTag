import glob, os
from makeCards import makeCards

files = glob.glob("cards/input*.txt")

for file in files:
    file2 = file.replace("input","simpleCard").replace(".txt","")
    makeCards(file,file2)

