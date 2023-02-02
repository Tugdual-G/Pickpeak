##
# parseargs_unitTest
#
# @file
# @version 0.1

mfname = makeArgs.mk
binname = args
Flags = -Wall
files = TESTparseargs.c ../core/parseArgs.c
moptions = --no-print-directory

cleanf = rm $(binname) || true

args : $(files)
	$(cleanf) ; gcc -o $(binname) $(Flags) $(files)

run :
	make $(moptions) -f $(mfname) && ./$(binname) -m 23 --outfile out/file.json --infile fs.scd -R 434;
	$(cleanf)

clean :
	$(cleanf)
# end
