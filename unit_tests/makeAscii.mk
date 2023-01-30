##
# ascii_unitTest
#
# @file
# @version 0.1

mfname = makeAscii.mk
binname = ascii
Flags = -Wall
files = TESTascii.c ../core/asciiGridParse.c
moptions = --no-print-directory

cleanf = rm $(binname) binary.double || true

ascii : $(files)
	gcc -o $(binname) $(Flags) $(files)
run :
	make $(moptions) -f $(mfname) && ./$(binname) && python test.py ; make clean $(moptions) -f $(mfname)

clean :
	$(cleanf)
# end
