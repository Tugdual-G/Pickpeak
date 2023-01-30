##
# findpeak_unitTest
#
# @file
# @version 0.1

mfname = makeFindpeak.mk
binname = findpeak
Flags = -Wall -Wextra -Wconversion -pedantic
files = TESTfindpeak.c ../core/reduce.c ../core/findpeak.c ../core/array.c
moptions = --no-print-directory


reduce : $(files)
	gcc -o $(binname) $(Flags) $(files)

run :
	make $(moptions) -f $(mfname) && ./$(binname); make clean $(moptions) -f $(mfname)

clean :
	rm $(binname)
# end
