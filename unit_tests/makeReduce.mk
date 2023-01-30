##
# reduce_unitTest
#
# @file
# @version 0.1

mfname = makeReduce.mk
binname = reduce
Flags = -Wall
files = TESTreduce.c ../core/reduce.c ../core/array.c
moptions = --no-print-directory


reduce : $(files)
	gcc -o $(binname) $(Flags) $(files)

run :
	make $(moptions) -f $(mfname) && ./$(binname); make clean $(moptions) -f $(mfname)

clean :
	rm $(binname)
# end
