#
#  PyTextProcessor used (https://bitbucket.org/morglod/miscpythonprojects/src/000754d1c7ba/PyTextProcessor/?at=master)
#

import os

# 'x' is tag name
tagBegin = "[x]"
tagEnd = "[/x]"

def TagImg(whatImg):
	return "<img src=\"" + whatImg + "\"></img>"

def TagFileBegin(what):
	return "<html>"+what+"<body>"

def TagFileEnd(what):
	return "</body>"+what+"</html>"

def TagHeading(what):
	return "<h3>"+what+"</h3>"

def TagSplit(what):
	return "<hr>"

def TagEnum(what):
	lines = what.split("\n")
	result = ""
	for s in lines:
		result += "<p>" + s
	return result

tagProcDict = {}
tagProcDict["img"] = TagImg
tagProcDict["filebegin"] = TagFileBegin
tagProcDict["fileend"] = TagFileEnd
tagProcDict["heading"] = TagHeading
tagProcDict["split"] = TagSplit
tagProcDict["enum"] = TagEnum

def FullTagName(tag):
	return tagBegin.replace('x', tag), tagEnd.replace('x', tag)

def ProcessTag(text, tag):
	tagBegStr, tagEndStr = FullTagName(tag)
	beg = text.find(tagBegStr)
	end = text.find(tagEndStr)
	innerText = text[beg + len(tagBegStr) : end]
	return ProcessNextTag(text.replace(text[beg : end + len(tagEndStr)], tagProcDict[tag](innerText)))

def ProcessNextTag(text):
	for tag in tagProcDict:
		if FullTagName(tag)[0] in text:
			return ProcessTag(text, tag)
	return text

def Main():
	fl = open("FeaturesList.txt")
	flout = open("FeaturesList.html", "wt")
	filestr = str(fl.read())
	fl.close()
	flout.write(ProcessNextTag(filestr))
	flout.close()

if __name__ == "__main__":
	Main()