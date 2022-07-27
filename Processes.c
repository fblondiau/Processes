/*****
 * Processes
 *	(c) FrŽdŽric Blondiau
 *****/

#pragma load "MacHeaders"

/*****/

Boolean checkType, checkSignature, extendedFormat, toBeQuoted, withHeaderFormat,
	fullPathFormat, onlyActive;
Boolean showType, showSignature, showSize, showFreeMem, showActiveTime,
	showLaunchDate, showFlags, showLocation, showID, showOwnerID, showOwnerName;
OSType typeToFind, signatureToFind;
short namesToFind;
Boolean firstPass;
char ** gArgv;

/*****/

static void ShowProcessInfo (ProcessInfoRecPtr theProcessInfoRecPtr)
{
	Str255 processStr;

/* if type to check does not match, return */
	if (checkType && (theProcessInfoRecPtr -> processType
			!= typeToFind))
		return;

/* if signature to check does not match, return */
	if (checkSignature && (theProcessInfoRecPtr -> processSignature
			!= signatureToFind))
		return;

/* if is a DA, skip first null byte... */
	if (theProcessInfoRecPtr -> processName [1] == 0)
	{
		BlockMove (& theProcessInfoRecPtr -> processName [2],
			& processStr [1], theProcessInfoRecPtr -> processName [0] - 1);
		processStr [0] = theProcessInfoRecPtr -> processName [0] - 1;
	}
	else
	{
		BlockMove (theProcessInfoRecPtr -> processName,
			processStr, theProcessInfoRecPtr -> processName [0] + 1);
	}

/* if no names to check does match, return */
	if (namesToFind)
	{
		short tmpShort;
		Boolean stringDifferent;

		for (stringDifferent = true, tmpShort = 1;
				stringDifferent && (tmpShort <= namesToFind);
				stringDifferent = IUEqualString (processStr, gArgv [tmpShort]),
					tmpShort ++)
			;
		if (stringDifferent)
			return;
	}

/* if full path, loop back to root (max 255 chars) */
	if (fullPathFormat)
	{
		Boolean truncated;
		
		truncated = false;
		if (theProcessInfoRecPtr -> processAppSpec != nil)
		{
			FSSpec tmpFSSpec;
			OSErr theOSErr;

			theOSErr = fnfErr;
			tmpFSSpec = * theProcessInfoRecPtr -> processAppSpec;
			do
			{
				theOSErr = FSMakeFSSpec (tmpFSSpec.vRefNum, tmpFSSpec.parID,
					"\p", & tmpFSSpec);
				if ((theOSErr == noErr) && (! truncated))
				{
					if (processStr [0] < 255 - 32)
					{
						BlockMove (& processStr [1],
							& processStr [1 + tmpFSSpec.name [0] + 1],
							processStr [0]);
						processStr [1 + tmpFSSpec.name [0]] = ':';
						BlockMove (& tmpFSSpec.name [1], & processStr [1],
							tmpFSSpec.name [0]);
	
						processStr [0] += tmpFSSpec.name [0] + 1;
					}
					else
						truncated = true;
				}
			} while ((tmpFSSpec.parID != fsRtParID) && (theOSErr == noErr));
			
			if (truncated && (theOSErr == noErr))
			{
				BlockMove (& processStr [1],
					& processStr [1 + tmpFSSpec.name [0] + 3],
					processStr [0]);
				processStr [1 + tmpFSSpec.name [0]] = ':';
				processStr [1 + tmpFSSpec.name [0] + 1] = 'É';
				processStr [1 + tmpFSSpec.name [0] + 2] = ':';
				BlockMove (& tmpFSSpec.name [1], & processStr [1],
					tmpFSSpec.name [0]);

				processStr [0] += tmpFSSpec.name [0] + 1;
			}
		}
	}

/* quote if non alnum chars */
	if (toBeQuoted)
	{
		Boolean hasAlNumChars;
		short i;

		for (i = 1, hasAlNumChars = true; (i <= processStr [0]) &&
				hasAlNumChars; i++)
		{
			hasAlNumChars = isalnum (processStr [i]);
		}
		
		if (! hasAlNumChars)
		{
			BlockMove (& processStr [1], & processStr [2], processStr [0]);
			processStr [0] += 2;
			processStr [1] = '\'';
			processStr [processStr [0]] = '\'';
		}
	}

/* do effective printouts */
	p2cstr (processStr);
	if (extendedFormat)
	{
		if (firstPass && withHeaderFormat)
		{
			printf ("Name                  ");
			if (showType)
				printf ("Type ");
			if (showSignature)
				printf ("Crtr ");
			if (showSize)
				printf (" Size  ");
			if (showFreeMem)
				printf (" Free  ");
			if (showActiveTime)
				printf ("  Active-Time    ");
			if (showLaunchDate)
				printf ("   Launch-Date     ");
			if (showFlags)
				printf ("    Flags     ");
			if (showLocation)
				printf (" Location  ");
			if (showID)
				printf ("    Process ID     ");
			if (showOwnerID)
				printf ("     Owner ID      ");
			if (showOwnerName)
				printf ("     Owner Name      ");
			printf ("\n");

			printf ("--------------------  ");
			if (showType)
				printf ("---- ");
			if (showSignature)
				printf ("---- ");
			if (showSize)
				printf ("------ ");
			if (showFreeMem)
				printf ("------ ");
			if (showActiveTime)
				printf ("--------------- ");
			if (showLaunchDate)
				printf ("------------------- ");
			if (showFlags)
				printf ("------------- ");
			if (showLocation)
				printf ("---------- ");
			if (showID)
				printf ("------------------ ");
			if (showOwnerID)
				printf ("------------------ ");
			if (showOwnerName)
				printf ("-------------------- ");
			printf ("\n");

			firstPass = false;
		}

		printf ("%-20.20s  ", processStr);

		if (showType)
			printf ("%4.4s ", & theProcessInfoRecPtr -> processType);
		if (showSignature)
			printf ("%4.4s ", & theProcessInfoRecPtr -> processSignature);
		if (showSize)
			printf ("%5dK ", theProcessInfoRecPtr -> processSize / 1024);
		if (showFreeMem)
			printf ("%5dK ", theProcessInfoRecPtr -> processFreeMem / 1024);
		if (showActiveTime)
		{
			Str255 activeTimeStr;

			IUTimeString (theProcessInfoRecPtr -> processActiveTime / 60,
				true, activeTimeStr);
			p2cstr (activeTimeStr);
	
			printf ("%3d%9s.%2.02d ",
				theProcessInfoRecPtr -> processActiveTime / 60 / 60 / 60 / 24,
				activeTimeStr,
				theProcessInfoRecPtr -> processActiveTime % 60 * 100 / 60);
		}
		if (showLaunchDate)
		{
			Str255 launchDateStr, launchTimeStr;
			unsigned long tmpLong;

			GetDateTime (& tmpLong);
			IUDateString (tmpLong - (TickCount () - 
					theProcessInfoRecPtr -> processLaunchDate) / 60,
				shortDate, launchDateStr);
			p2cstr (launchDateStr);

			IUTimeString (tmpLong - (TickCount () - 
					theProcessInfoRecPtr -> processLaunchDate) / 60,
				false, launchTimeStr);
			p2cstr (launchTimeStr);
			printf ("%10s%9s ", launchDateStr, launchTimeStr);
		}
		if (showFlags)
		{
			Str255 flagsStr = "dmncabfethrsu";

			printf ("%13s ", flagsStr);
		}
		if (showLocation)
			printf ("Ox%08X ", theProcessInfoRecPtr -> processLocation);
		if (showID)
			printf ("Ox%08X%08X ",
				theProcessInfoRecPtr -> processNumber.highLongOfPSN,
				theProcessInfoRecPtr -> processNumber.lowLongOfPSN);
		if (showOwnerID)
			printf ("Ox%08X%08X ",
				theProcessInfoRecPtr -> processLauncher.highLongOfPSN,
				theProcessInfoRecPtr -> processLauncher.lowLongOfPSN);
		if (showOwnerName)
		{
			ProcessInfoRec tmpProcessInfoRec;
			Str32 theOwnerName;
			FSSpec theOwnerFSSpec;

			tmpProcessInfoRec.processInfoLength = sizeof (tmpProcessInfoRec);
			tmpProcessInfoRec.processName = & theOwnerName;
			tmpProcessInfoRec.processAppSpec = & theOwnerFSSpec;
		
			GetProcessInformation (& theProcessInfoRecPtr -> processLauncher,
				& tmpProcessInfoRec);
			p2cstr (theOwnerName);
			printf ("%20.20s ", theOwnerName);
		}
		printf ("\n");
	}
	else
		printf ("%s\n", processStr);

	return;
}

/*****/

static OSErr ListProcesses (void)
{
	ProcessSerialNumber theProcessSerialNumber;
	OSErr theOSErr;
	ProcessInfoRec theProcessInfoRec;
	FSSpec theProcessFSSpec;
	Str32 theProcessName;
	
	theProcessSerialNumber.lowLongOfPSN = kNoProcess;
	theProcessSerialNumber.highLongOfPSN = kNoProcess;

	theProcessInfoRec.processInfoLength = sizeof (theProcessInfoRec);
	theProcessInfoRec.processName = & theProcessName;
	theProcessInfoRec.processAppSpec = & theProcessFSSpec;

	firstPass = true;
	if (onlyActive)
	{
		theOSErr = GetFrontProcess (& theProcessSerialNumber);
		if (theOSErr == noErr)
		{
			GetProcessInformation (& theProcessSerialNumber,
				& theProcessInfoRec);
			ShowProcessInfo (& theProcessInfoRec);
		}
	}
	else
	{		
		do
		{
			theOSErr = GetNextProcess (& theProcessSerialNumber);
			if (theOSErr == noErr)
			{
				GetProcessInformation (& theProcessSerialNumber,
					& theProcessInfoRec);
				ShowProcessInfo (& theProcessInfoRec);
			}
		}
		while (theOSErr == noErr);
	}

	return noErr;
}

/*****/

main (short argc, char * argv [])
{
	short i;
	Boolean toList;

	toList = true;
	gArgv = argv;

	checkType = false;
	checkSignature = false;
	extendedFormat = false;
	toBeQuoted = true;
	withHeaderFormat = true;
	fullPathFormat = false;
	onlyActive = false;

	showType = false;
	showSignature = false;
	showSize = false;
	showFreeMem = false;
	showActiveTime = false;
	showLaunchDate = false;
	showFlags = false;
	showLocation = false;
	showID = false;
	showOwnerID = false;
	showOwnerName = false;
	
	namesToFind = 0;

	for (i = 1; i < argc; i++)
	{
		switch (argv [i] [0])
		{
			case '-':
				switch (argv [i] [1])
				{
					case 't':
						if (i + 1 != argc)
						{
							checkType = true;
							BlockMove (argv [i + 1], & typeToFind, 4);
							i ++;
						}
						else
						{
							printf ("### Processes - The \"-t\" option requires"
								" a parameter.\n");
							toList = false;
						}
						break;

					case 'c':
						if (i + 1 != argc)
						{
							checkSignature = true;
							BlockMove (argv [i + 1], & signatureToFind, 4);
							i ++;
						}
						else
						{
							printf ("### Processes - The \"-c\" option requires"
								" a parameter.\n");
							toList = false;
						}
						break;

					case 'l':
						extendedFormat = true;

						showType = true;
						showSignature = true;
						showSize = true;
						showFreeMem = true;
						showActiveTime = true;
						showLaunchDate = true;

						break;

					case 'q':
						toBeQuoted = false;
						break;

					case 'f':
						fullPathFormat = true;
						break;

					case 'n':
						withHeaderFormat = false;
						break;

					case 'x':
					{
						if (i + 1 != argc)
						{
							short j;

							i ++;
							for (j = 0; argv [i] [j]; j++)
							{
								switch (argv [i] [j])
								{
									case 't':
										showType = true;
										break;

									case 'c':
										showSignature = true;
										break;

									case 's':
										showSize = true;
										break;

									case 'm':
										showFreeMem = true;
										break;

									case 'a':
										showActiveTime = true;
										break;

									case 'd':
										showLaunchDate = true;
										break;

									case 'f':
										showFlags = true;
										break;

									case 'l':
										showLocation = true;
										break;

									case 'i':
										showID = true;
										break;

									case 'o':
										showOwnerID = true;
										break;

									case 'n':
										showOwnerName = true;
										break;

									default:
										printf ("### Processes - %c is not a"
											" valid format character.\n",
											argv [i] [j]);
									toList = false;
								}
							}
						}
						else
						{
							printf ("### Processes - The \"-x\" option requires"
								" a parameter.\n");
							toList = false;
						}
						extendedFormat = true;

						break;
					}
					case 'a':
						onlyActive = true;
						break;

					default:
						printf ("### Processes - \"%s\" is not an option.\n",
							argv [i]);
						toList = false;
						break;
				}
				break;

			default:
			{
				char * tmpCharPtr;

				namesToFind ++;
				tmpCharPtr = argv [namesToFind];
				argv [namesToFind] = argv [i];
				argv [i] = tmpCharPtr;
				
				c2pstr (argv [namesToFind]);

				break;
			}
		}
	}

	if (toList)
		ListProcesses ();
	else
	{
		printf ("# Usage - Processes [-l] [-q] [-f] [-n] [-x tcsmadflion] [-c creator] [-t type] [nameÉ]\n");
		return 1;
	}
		
	return 0;
}

/*****/
