#!/usr/bin/python
import sys
import os
import os.path
import re
import array
#Modified for wxHexEditor application, to create one big file that has all the icons. It's much clean in that way.
USAGE = """png2c - Embed a PNG in a C header file (like XPM)
Usage: png2c [file ..] Convert and sums all the file <file.png>'s to <png2c.h>"""
 
if len(sys.argv) < 2:
	print USAGE
	sys.exit(1)
 
r = re.compile("^([a-zA-Z._][a-zA-Z._0-9]*)[.][pP][nN][gG]$")
 
text=''
for path in sys.argv[1:]:
	filename = os.path.basename(path)
	m = r.match(filename)
	# Allow only filenames that make sense
	# as C variable names
	if not(m):
		print "Skipped file (unsuitable filename): " + filename
		continue
 
	# Read PNG file as character array
	bytes = array.array('B', open(path, "rb").read())
	count = len(bytes)
 
	# Create the C header
	text += "/* %s - %d bytes */\nstatic const unsigned char %s_png[] = {\n" % (filename, count, m.group(1))
 
	# Iterate the characters, we want
	# lines like:
	#   0x01, 0x02, .... (8 values per line maximum)
	i = 0
	count = len(bytes)
	for byte in bytes:
		# Every new line starts with two whitespaces
		if (i % 8) == 0:
			text += "  "
		# Then the hex data (up to 8 values per line)
		text += "0x%02x" % (byte)
		# Separate all but the last values 
		if (i + 1) < count:
			text += ", "
		if (i % 8) == 7:
			text += '\n'
		i += 1
 
	# Now conclude the C source
	text += "};\n"

text+=" /* End Of File */\n"
open("png2c.h", 'w').write(text)
