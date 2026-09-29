-------------------------------------------------------------------------------
----
---- opsys.as: High-level interface to operating system functions.
----
-------------------------------------------------------------------------------
----
---- This file is part of Aldor.
----
---- Aldor is licensed under the Apache License, Version 2.0.
----
----
---- See legal/LICENSE in the Aldor distribution for details.
----
---- Copyright (C) 1990-2026 Stephen M. Watt.

#include "axllib"

CommandLine: with {
	command:   	String;
	arguments: 	Array String;
}
== add {
	import {
		mainArgc:  SingleInteger;
		mainArgv:  PrimitiveArray String;
	} from Foreign;

	import from SingleInteger, PrimitiveArray String;

	command:   String       == mainArgv 1;
	arguments: Array String == [mainArgv i for i in 2..mainArgc];
}


StandardIO: with {
	stdin:   InFile;

	stdout:  OutFile;
	stderr:	 OutFile;
	stdsink: OutFile;
}
== add {
	CStream ==> Pointer;
	import from Pointer;

	import {
		stdinFile:  () -> CStream;
		stdoutFile: () -> CStream;
		stderrFile: () -> CStream;
	} from Foreign;

	stdin:   InFile  == stdinFile()  pretend InFile;
	stdout:  OutFile == stdoutFile() pretend OutFile;
	stderr:  OutFile == stderrFile() pretend OutFile;
	stdsink: OutFile == nil          pretend OutFile;
}
