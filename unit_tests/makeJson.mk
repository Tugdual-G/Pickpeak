
##
# json_unitTest
#
# @file
# @version 0.1

mfname = makeJson.mk
binname = json
Flags = -Wall
files = ../core/array.c TESTwritejson.c ../core/writeJson.c -ljson-c
moptions = --no-print-directory

cleanf = rm $(binname) points.json || true

json : $(files)
	$(cleanf) ; gcc -o $(binname) $(Flags) $(files)
run :
	make $(moptions) -f $(mfname) && ./$(binname) ; vim points.json ; $(cleanf)

clean :
	$(cleanf)
# end
