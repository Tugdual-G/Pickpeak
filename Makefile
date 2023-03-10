##
# PickPeak
#
# @file
# @version 0.1
mfname = Makefile

binname = pickpeak

Flags = -Wall -pedantic -Wextra -fanalyzer

dependancies = core/parseArgs.c core/array.c core/findPeak.c \
		core/asciiGridParse.c core/writeJson.c core/reduce.c \
		core/pickPeak.c -ljson-c

moptions = --no-print-directory

cleanf = (rm "$(binname)")>/dev/null 2>&1

pickpeak : $(dependancies)
	$(cleanf);\
	gcc -o $(binname) $(Flags) -O2 $(dependancies)

debug : $(dependancies)
	$(cleanf);\
	gcc -o $(binname) $(Flags) -g $(dependancies)

clean :
	$(cleanf)


# end
