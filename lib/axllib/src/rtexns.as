-------------------------------------------------------------------------------
----
---- rtexns.as: Runtime exceptions
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

#if Axiom

#else
+++
define RuntimeException: Category == with {
	name: () -> String;
	printError: TextWriter -> ();
}

RuntimeError(s: String): RuntimeException with == add {
	name(): String == s;
	printError(o: TextWriter): () == o << s;
}

export {
	aldorRuntimeException: String -> ();
	aldorUnhandledException: Pointer -> ();
} to Foreign Builtin;

-- should print and raise exception...
aldorRuntimeException(error: String): () == runtimeError(error)$ExceptionPackage;

aldorUnhandledException(p: Pointer): () == unhandledHandler(p)$ExceptionPackage;

+++
ExceptionPackage: with {
	unhandledHandler: Pointer -> ();
	installHandler: (Pointer -> ()) -> ();
	defaultHandler: Pointer -> ();
	runtimeError: String -> ();
} == add {
	defaultHandler(p: Pointer): () == {
		import from DomainName;
		print << "Unhandled Exception: " << typeName(p pretend Type) << newline;
		-- should have a way of adding extra handlers
		printRTInfoIfAny(p pretend with);
	}

	printRTInfoIfAny(t: with): () == {
		if t has RuntimeException then {
			printError(print)$t;
			print << newline;
		}
	}

	theHandler: Pointer -> () := defaultHandler;

	unhandledHandler(p: Pointer): () == theHandler(p);

	installHandler(f: Pointer -> ()): () == {
		free theHandler := f;
	}
	
	runtimeError(s: String): () == {
		--print << "Exception raised: " << s << newline;
		throw RuntimeError(s);
	}
}

#endif
