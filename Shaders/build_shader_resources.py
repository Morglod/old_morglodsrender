# Build shader files to source (ShaderCodeDefault.hpp)

import sys

def index_replace(inWhat, withIt, beginIndex, endIndex):
	return inWhat[:beginIndex] + withIt + inWhat[endIndex:]

header = ""
with open("ShaderCodeDefault.hpp", "rt") as out:
	header = out.read()

if(not '//BEGIN FILE TO CHAR "' in header):
	sys.exit()

offset = 0
while(header.find('//BEGIN FILE TO CHAR "', offset) != -1):
	begin = header.find('//BEGIN FILE TO CHAR "', offset)
	end = header.find('//END FILE TO CHAR', offset)-1

	filenameBegin = begin+len('//BEGIN FILE TO CHAR "')
	filenameEnd = header.find('"', filenameBegin)
	filename = header[filenameBegin : filenameEnd]

	print(filename)
	file_str = ""

	with open(filename, "rt") as fl:
		file_str = '"' + fl.read().replace("\n", "\\n") + '";'

	insertion = '//BEGIN FILE TO CHAR "' + filename + '"' + "\n\n" + 'const char* ' + filename.replace('.','_') + ' = ' + file_str + "\n\n" + '//END FILE TO CHAR'
	header = index_replace(header, insertion, begin, end+1+len('//END FILE TO CHAR'))
	offset = begin + len(insertion)

with open("ShaderCodeDefault.hpp", "wt") as out:
	out.write(header)