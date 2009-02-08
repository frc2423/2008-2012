/* Title: DBug12 library wrapper */
/* Author: Motorola (Modified by Brett Wildermoth for the GNU-GCC-COMPILER)
/* This file may be #included with any C source file that uses the D-Bug12 */
/* user callable routines. It provides a simple, portable way to access the */
/* routines from C without having to use any assembly language glue code */
/* some typedefs used by D-Bug12 */
/* Changes from the original: Interrupt table removed, types changed to suit
GCC */
/* int != short, int = 4 bytes not 2 as in ICC12 !! */
/* all int types changed to short types*/
/* Date: 16/02/2003 */
/* Version: 1.00a */

typedef short * Address;
typedef char Boolean;
typedef unsigned char Byte;

typedef struct {
	short (*DB12getchar)(void);
	short (*DB12putchar)(char);
	short (*DB12printf)(const char *,...);
	short (*GetCmdLine)(char *CmdLineStr, unsigned char CmdLineLen);
	char * (*sscanhex)(char *HexStr, unsigned char *BinNum);
	short (*DB12isxdigit)(unsigned char c);
	short (*DB12toupper)(unsigned char c);
	short (*DB12isalpha)(unsigned char c);
	unsigned short (*DB12strlen)(const char *cs);
	char * (*DB12strcpy)(char *s1, char *s2);
	void (*DB12out2hex)(unsigned char num);
	void (*DB12out4hex)(unsigned short num);
	short (*DB12SetUserVector)(unsigned char VectNum, Address UserAddress);
	Boolean (*WriteEEByte)(Address EEAddress, Byte EEData);
	short (*EraseEE)(void);
	short (*ReadMem)(Address StartAddress, Byte *MemDataP, unsigned short
	NumBytes);
	short (*WriteMem)(Address StartAddress, Byte *MemDataP, unsigned short
	NumBytes);
} UserFN, * UserFNP;

/* defines a pointer to the start of D-Bug12's user accessible functable */
#define MyUserFNP ((UserFNP)0xEE84)/* in D-Bug12 version 2.x.x the user
accessible table begins at $f680 */

/* The following #defines are used to provide for portability and avoid a
linker */
/* conflict with the standard library functions of the same name. No #define
is */
/* included for DB12main() since all C programs must contain a main() function
*/

/*
#define printf MyUserFNP->DB12printf
//#define printf12 ((int (*)(char *,...))*(short *)(0xF686))
#define getchar MyUserFNP->DB12getchar
#define putchar MyUserFNP->DB12putchar
#define isxdigit MyUserFNP->DB12isxdigit
#define toupper MyUserFNP->DB12toupper
#define isalpha MyUserFNP->DB12isalpha
#define strlen MyUserFNP->DB12strlen
#define out2hex MyUserFNP->DB12out2hex
#define strcpy MyUserFNP->DB12strcpy
#define GetCmdLine MyUserFNP->GetCmdLine
#define SetUserVector MyUserFNP->DB12SetUserVector
*/