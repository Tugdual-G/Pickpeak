##
# PickPeak
#
# @file
# @version 0.1
mfname = Makefile

binname = core/pickpeak

Flags = -Wall

dependancies = core/parseArgs.c core/array.c core/findPeak.c \
		core/asciiGridParse.c core/writeJson.c core/reduce.c \
		core/pickPeak.c -ljson-c

moptions = --no-print-directory

cleanf = rm $(binname) || true

json : $(dependancies)
	gcc -o $(binname) $(Flags) $(dependancies)

clean :
	$(cleanf)


# end
