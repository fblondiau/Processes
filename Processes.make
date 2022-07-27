#/*****
# * Processes
# *	(c) FrŽdŽric Blondiau
# *****/

MacHeaders Ä MacHeaders.h Processes.h
	C -c MacHeaders.h

Processes ÄÄ Processes.make Processes.r Processes.h
	Rez Processes.r -append -o Processes

Processes ÄÄ Processes.make Processes.c.o
	Link -d -c 'MPS ' -t MPST ¶
		Processes.c.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		"{CLibraries}"StdCLib.o ¶
		#"{Libraries}"Stubs.o ¶
		#"{Libraries}"Runtime.o ¶
		#"{Libraries}"Interface.o ¶
		#"{Libraries}"ToolLibs.o ¶
		-o Processes

Processes.c.o ÄÄ Processes.make Processes.c MacHeaders
	 C -warnings full -r Processes.c

#/*****/
