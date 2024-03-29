/*
	cmdline - Command Line (c) 1996 Daniel Roggen

	This library provides a basic function to deal with command line
	parameters.

	Function:

	int ScanCommandLine(char *X,int c,char **v,...);
		This function takes a command string (X), the number of arguments
		on the command line c=argc and a pointer to those arguments v=argv.
		The variable number of arguments must match those described in the
		command string. The only allowed types are pointer to int and pointer
		to char (string).

		The command string consist of words separated by spaces. Caution:
		no type checking is done on this command string, thus this string
		must not be empty and must not begin with a space character. Only one
		space character is allowed between each words.

		The command string looks like that:
		"@ -? -h -o@ -c"
		Each parameter passed after v matches the type described in this
		string. Thus, the first and fourth parameters are pointers to char.
		The second, third and fifth are pointers to int.
		If one of the commands is present on the command line (-?, -h, -c)
		then the corresponding parameter of the function will be set to 1,
		otherwise 0.
		When the last character of a word is '@' then the function copies
		the name entered after the flag on the command line. The @ character
		tells the function to copy a name to the corresponding parameter.

		For example, if the command line is "hello.c -o hello.o -c", then
		"hello.c" will be copied to the first parameter (@), "hello.o" will
		be copied to the fourth parameter (-o@) and the fifth parameter
		will be set to 1. The second and third will be set to 0.

		The function returns 1 if an error occurs or 0 otherwise.
		An error consist of:
			1. not enough arguments. For example if the command string is "-o@"
			and the command line consists of "-o" without a name following it.
			1. arguments doesn't match command string. For example if the
			command string is "@" and the command line consists of 2 names
			("hello world") the first name will be stored but the second
			doesn't match the command string. The function returns immediately
			thus any flag or name the faulting word won't be processed.



*/

int ScanCommandLine(char *X,int c,char **v,...);
