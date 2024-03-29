/*
	Name 1: Ismael Marquez
	UTEID 1: im6549
*/
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
#include <stdbool.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

#define INSTR_FORMAT_0 0
#define INSTR_FORMAT_1 1
#define INSTR_FORMAT_2 2
#define INSTR_FORMAT_3 3
#define INSTR_FORMAT_4 4
#define INSTR_FORMAT_5 5


int symbolCount = 0;
int instrCount = 0;
int instrAndFill = 0;
int instrFormat;
int origIndex;
int origAddress = 0;
int PC = 0;
int regs[8] = {0};
int cCodes[3] = {0};

typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

typedef struct {
	int val;
    int address;
    char label[MAX_LABEL_LEN + 1];
    char op[MAX_LABEL_LEN + 1];
    char arg1[MAX_LABEL_LEN + 1];
    char arg2[MAX_LABEL_LEN + 1];
    char arg3[MAX_LABEL_LEN + 1];
    char arg4[MAX_LABEL_LEN + 1];
} InstrStruct;
InstrStruct instrTable[500];

enum
{
	DONE, OK, EMPTY_LINE
};

void callError(int code)
{
	exit(code);
}

bool isEmpty( char * stringPtr )
{
	if(stringPtr[0] == '\0')
		return true;

	return false;
}

// void setCC(int regIndex)
// {
// 	if( regs[regIndex] < 0)
// 	{
// 		cCodes[0] = 1;
// 		cCodes[1] = 0;
// 		cCodes[2] = 0;
// 	}
// 	if( regs[regIndex] == 0)
// 	{
// 		cCodes[0] = 0;
// 		cCodes[1] = 1;
// 		cCodes[2] = 0;
// 	}
// 	if( regs[regIndex] > 0)
// 	{
// 		cCodes[0] = 0;
// 		cCodes[1] = 0;
// 		cCodes[2] = 1;
// 	}
// }

void getCC(char ** pOpcode, int * n, int * z, int * p)
{
	int opLength = strlen(*pOpcode);
	if( opLength == 2 || opLength == 5)
	{
		*n = 1;
		*z = 1;
		*p = 1;
	}
	else if( opLength == 3 )
	{
		if( (*pOpcode)[2] == 'n' )
		{
			*n = 1;
			*z = 0;
			*p = 0;
		}
		else if( (*pOpcode)[2] == 'z' )
		{
			*z = 1;
			*n = 0;
			*p = 0;
		}
		else if( (*pOpcode)[2] == 'p' )
		{
			*p = 1;
			*n = 0;
			*z = 0;
		}
	}
	else if( opLength == 4 )
	{
		if( (*pOpcode)[2] == 'n' &&  (*pOpcode)[3] == 'z' )
		{
			*n = 1;
			*z = 1;
			*p = 0;
		}
		else if( (*pOpcode)[2] == 'n' &&  (*pOpcode)[3] == 'p' )
		{
			*n = 1;
			*p = 1;
			*z = 0;
		}
		else if( (*pOpcode)[2] == 'z' &&  (*pOpcode)[3] == 'p' )
		{
			*z = 0;
			*p = 0;
			*n = 0;
		}
	}
}

int getSymbolAddress(char * pLabel)
{
	//printSymbolTable();
	TableEntry symbol;
	for( int i = 0; i < symbolCount; i++)
	{
		symbol = symbolTable[i];
		//printf("%.4x\n", symbol.address);
		if( strcmp(symbol.label, pLabel) == 0 )
		{
			//printf("%.4x\n", symbol.address);
			return symbol.address;
		}
	}
	return -1;
}

int getOpcodeInt(char * opcode)
{
	// opcode[] always lowercase

	instrFormat = INSTR_FORMAT_0;
	if ( strcmp(opcode, "add") == 0) 
		return 1;
	if ( strcmp(opcode, "and") == 0)
		return 5;
	if ( strcmp(opcode, "not") == 0)
		return 9;
	if ( strcmp(opcode, "xor") == 0)
		return 9;

	instrFormat = INSTR_FORMAT_1;
	if ( strcmp(opcode, "ldb") == 0)
		return 2;
	if ( strcmp(opcode, "ldw") == 0)
		return 6;
	if ( strcmp(opcode, "stb") == 0)
		return 3;
	if ( strcmp(opcode, "stw") == 0)
		return 7;
	if ( strcmp(opcode, "jmp") == 0)
		return 12;
	if ( strcmp(opcode, "ret") == 0)
		return 12;

	instrFormat = INSTR_FORMAT_2;
	if ( strcmp(opcode, "lshf") == 0)
		return 13;
	if ( strcmp(opcode, "rshfl") == 0)
		return 13;
	if ( strcmp(opcode, "rshfa") == 0)
		return 13;

	instrFormat = INSTR_FORMAT_3;
	if ( strcmp(opcode, "jsr") == 0)
		return 4;	
	if ( strcmp(opcode, "jsrr") == 0)
		return 4;

	instrFormat = INSTR_FORMAT_4;
	if ( strcmp(opcode, "halt") == 0)
		return 15;
	if ( strcmp(opcode, "trap") == 0)
		return 15;

	instrFormat = INSTR_FORMAT_5;
	if ( strcmp(opcode, "lea") == 0)
		return 14;
	if ( strcmp(opcode, "br") == 0)
		return 0;
	if ( strcmp(opcode, "brn") == 0)
		return 0;
	if ( strcmp(opcode, "brz") == 0)
		return 0;
	if ( strcmp(opcode, "brp") == 0)
		return 0;
	if ( strcmp(opcode, "brnz") == 0)
		return 0;
	if ( strcmp(opcode, "brnp") == 0)
		return 0;
	if ( strcmp(opcode, "brzp") == 0)
		return 0;
	if ( strcmp(opcode, "brnzp") == 0)
		return 0;


	if ( strcmp(opcode, "rti") == 0)
		return 8;
	if ( strcmp(opcode, "nop") == 0)
		return 0;	

	instrFormat = -1;
	return -1;
}

/* check if given string is a valid opcode
	Valid Opcodes: ADD, AND, BR(all variations), HALT, JMP, JSRR, LDB, LDW, LEA,
				 NOP, NOT, RET, LSHF, RSHFL, RSHFA, RTI, STB, STW, TRAP, XOR */
bool isOpcode( char * lPtr ) {

	if (getOpcodeInt(lPtr) != -1)
		return true;

	return false;
}

/* Converts a string into a hex or decimal number */
int toNum( char * pStr ) {
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
		pStr++;
		if( *pStr == '-' )				/* dec is negative */
		{
			lNeg = 1;
			pStr++;
		}

		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for(k=0;k < t_length;k++)
		{
			if (!isdigit(*t_ptr))
			{
				printf("Error: invalid decimal operand, %s\n",orig_pStr);
				exit(4);
			}
			t_ptr++;
		}

		lNum = atoi(pStr);
		if (lNeg)
			lNum = -lNum;

		return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
		pStr++;
		if( *pStr == '-' )				/* hex is negative */
		{
			lNeg = 1;
			pStr++;
		}

		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for(k=0;k < t_length;k++)
		{
			if (!isxdigit(*t_ptr))
			{
				printf("Error: invalid hex operand, %s\n",orig_pStr);
				exit(4);
			}
			t_ptr++;
		}

		lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
		lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
		if( lNeg )
			lNum = -lNum;

		return lNum;
   }
   else
   {
		printf( "Error: invalid operand, %s\n", orig_pStr);
		exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

/* Take a line from the input file and parse it into its corresponding section
	(i.e. LABEL, OPCODE, OPERANDS, COMMENTS)
	*/
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
) {
	char * lRet, * lPtr;
	int i;
	if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );

	/* convert entire line to lowercase */
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	/* ignore the comments */
	lPtr = pLine;
	while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' ) 
		lPtr++;

	*lPtr = '\0';
	if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	/* return if label */
	if( isOpcode( lPtr ) == 0 && lPtr[0] != '.' )
	{
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	}

	/* return if opcode */
	*pOpcode = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg1 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg2 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg3 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	*pArg4 = lPtr;

	return( OK );
}

char* getArgType(char * pArg)
{
	if ( strlen(pArg) == 2 && pArg[0] == 'r' && isdigit(pArg[1]))
	{
		if( pArg[1] == '8' || pArg[1] == '9' )
		{
			callError(4);
		}

		return "reg";
	}
	if ( pArg[0] == '#' || pArg[0] == 'x')
		return "num";
}

int getArgInt(char * pArg)
{
	/* return the value of the register */
	if ( pArg[0] == 'r' ) {
		if( pArg[1] == '8' || pArg[1] == '9' || !isdigit(pArg[1]))
			callError(4);
		int num = pArg[1] - '0';
		return num;
	}
	/* return the value of the immediate */
	if ( pArg[0] == '#' || pArg[0] == 'x')
		return toNum(pArg);

	return 0;
}

/* ADD, AND, XOR, NOT */
int assembleFormat0(int opcodeInt, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
{
	bool setBit5 = false;
	int arg4Int = 0;
	int sign = 0;

	// DR
	int arg1Int = 0;
	if( !isEmpty(*pArg1) ) {
		arg1Int = getArgInt(*pArg1) << 9;
	}
	// SR1
	int arg2Int = 0;
	if( !isEmpty(*pArg2) ) {
		arg2Int = getArgInt(*pArg2) << 6;
		if( strcmp(*pOpcode, "not") == 0 )
		{
			setBit5 = true;
			arg4Int = 31;
		}
	}
	// SR2 or imm5
	int arg3Int = 0;
	if( !isEmpty(*pArg3) ) {
		arg3Int = getArgInt(*pArg3);
		if( getArgType(*pArg3) == "num")
			setBit5 = true;
	}

	if( arg3Int < -16 || arg3Int > 15)
	{
		callError(3);
	}

	// ???
	if(!isEmpty(*pArg4)) {
		arg4Int = getArgInt(*pArg4);
	}

	// // compute the result and store in DR
	// int source2;
	// if( strcmp(*pOpcode, "add") == 0 )
	// {
	// 	source2 = arg2Int;
	// 	if( !setBit5 )
	// 		source2 = regs[arg2Int];
	// 	regs[arg1Int] = regs[arg2Int] + source2;
	// }
	// else if( strcmp(*pOpcode, "and") == 0 )
	// {
	// 	source2 = arg2Int;
	// 	if( !setBit5 )
	// 		source2 = regs[arg2Int];
	// 	regs[arg1Int] = regs[arg2Int] & source2;
	// }
	// else if( strcmp(*pOpcode, "xor") == 0 )
	// {
	// 	source2 = arg2Int;
	// 	if( !setBit5 )
	// 		source2 = regs[arg2Int];
	// 	regs[arg1Int] = regs[arg2Int] ^ source2;
	// }
	// else if( strcmp(*pOpcode, "not") == 0 )
	// {
	// 	source2 = regs[arg2Int];
	// 	regs[arg1Int] = ~source2;
	// }

	// setCC(arg1Int);

	int bit5 = 0;
	if( setBit5 )
	{
		bit5 = 1 << 5;
		// bootleg af
		if( arg3Int < 0 )
		{
			arg3Int = arg3Int & 0xF;
			sign = 1 << 4;
		}
	}
	return opcodeInt + arg1Int + arg2Int + bit5 + arg3Int + arg4Int + sign;
}

/* LDB, LDW, STB, STW, JMP, RET */
int assembleFormat1(int opcodeInt, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
{
	int arg4Int = 0;
	int sign = 0;

	// DR or BR
	int arg1Int = 0;
	if( !isEmpty(*pArg1) ) 
	{
		if( strcmp(*pOpcode, "ret") == 0 )
			callError(4);

		if( strcmp(*pOpcode, "jmp") == 0 )
			arg1Int = getArgInt(*pArg1) << 6;
		else 
			arg1Int = getArgInt(*pArg1) << 9;
	}
	// BR
	int arg2Int = 0;
	if( !isEmpty(*pArg2) ) {
		if( strcmp(*pOpcode, "ret") == 0 )
			arg2Int = 7 << 6;
		else
			arg2Int = getArgInt(*pArg2) << 6;
	}
	// offset6
	int arg3Int = 0;
	if( !isEmpty(*pArg3) ) {
		arg3Int = getArgInt(*pArg3);
	}

	// ???
	if(!isEmpty(*pArg4)) {
		arg4Int = getArgInt(*pArg4);
	}

	if( arg3Int < -32 || arg3Int > 31 )
	{
		callError(3);
	}

	if( arg3Int < 0 )
	{
		arg3Int = arg3Int & 0x1F;
		sign = 1 << 5;
	}

	return opcodeInt + arg1Int + arg2Int + arg3Int + arg4Int + sign;
}

/* LSHF, RSHFL, RSHFA */
int assembleFormat2(int opcodeInt, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
{
	// DR
	int arg1Int = 0;
	if( !isEmpty(*pArg1) ) {
		arg1Int = getArgInt(*pArg1) << 9;
	}
	// SR
	int arg2Int = 0;
	if( !isEmpty(*pArg2) ) {
		arg2Int = getArgInt(*pArg2) << 6;
	}
	// amount4
	int arg3Int = 0;
	if( !isEmpty(*pArg3) ) {
		arg3Int = abs(getArgInt(*pArg3));
	}

	if( arg3Int < 0 || arg3Int > 15)
	{
		callError(3);
	}

	// ???
	int arg4Int = 0;
	if(!isEmpty(*pArg4)) { 
		arg4Int = getArgInt(*pArg4);
	}

	if( strcmp(*pOpcode, "rshfl") == 0 )
		arg4Int = 1 << 4;
	else if( strcmp(*pOpcode, "rshfa") == 0 )
		arg4Int = 3 << 4;

	return opcodeInt + arg1Int + arg2Int + arg3Int + arg4Int;
}

/* JSR, JSRR */
int assembleFormat3(int opcodeInt, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
{
	bool setBit11;
	int bit11 = 0;
	int symbolAddress;
	int offset = 0;

	// BR or PCoffset11
	int arg1Int = 0;
	if( !isEmpty(*pArg1) ) {
		if( getArgType(*pArg1) == "reg")
		{
			arg1Int = getArgInt(*pArg1) << 6;
		}
		else
		{
			setBit11 = true;
			symbolAddress = getSymbolAddress(*pArg1);
			if( symbolAddress == -1 )
			{
				if( (*pArg2)[0] == 'x' || (*pArg2)[0] == '#' )
					callError(4);
				else
					callError(1);
			}

			offset = (symbolAddress - (PC+1)) >> 1;

		}
	}

	if( setBit11 )
	{
		bit11 = 1 << 11;
		if( offset < -1024 || offset > 1023)
		{
			callError(3);
		}
	}

	if( offset < 0 )
	{
		offset = offset & 0x7FF;
		arg1Int = 1 << 10;
	}

	return opcodeInt + arg1Int + bit11 + offset;
}

/* TRAP, HALT */
int assembleFormat4(int opcodeInt, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
{
	// trapvect8
	int arg1Int = 0;
	if( !isEmpty(*pArg1) )
		arg1Int = getArgInt(*pArg1);
	if( strcmp(*pOpcode, "halt") == 0 )
	{
		if( !isEmpty(*pArg1) )
			callError(4);

		arg1Int = 0x25;
	}

	if( arg1Int < 0 || arg1Int > 255 )
		callError(3);

	return opcodeInt + arg1Int;
}

/* LEA, BR */
int assembleFormat5(int opcodeInt, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
{
	int n = 0;
	int z = 0;
	int p = 0;
	int offset = 0;
	int symbolAddress;

	// TODO: output error if symbol invalid

	// PCoffset9
	int arg1Int = 0;
	if( !isEmpty(*pArg1) )
	{
		if( strcmp(*pOpcode, "lea") == 0 )
			arg1Int = getArgInt(*pArg1) << 9;
		else
		{
			symbolAddress = getSymbolAddress(*pArg1);
			if( symbolAddress == -1 )
			{
				if( (*pArg1)[0] == 'x' || (*pArg1)[0] == '#' )
					callError(4);
				else
					callError(1);
			}

			offset = (symbolAddress - (PC+1)) >> 1;
		}
	}

	if( offset < -256 || offset > 255)
	{
		callError(3);
	}

	int arg2Int = 0;
	if( !isEmpty(*pArg2) )
	{
		symbolAddress = getSymbolAddress(*pArg2);
		if( symbolAddress == -1 )
		{
			if( (*pArg2)[0] == 'x' || (*pArg2)[0] == '#' )
				callError(4);
			else
				callError(1);
		}
		offset = (symbolAddress - origAddress - 1) / 2;
	}

	if( strcmp(*pOpcode, "lea") != 0 )
	{
		getCC(pOpcode, &n, &z, &p);
		//printf("n: %d\t z: %d\t p: %d\n", n, z, p);
	}
	
	// bootleg af
	if( offset < 0 )
	{
		offset = offset & 0xFF;
		arg1Int = 1 << 8;
	}

	//printf("%d\n", opcodeInt + n + z + p + arg1Int + offset);
	return opcodeInt + (n << 11) + (z << 10) + (p << 9) + arg1Int + offset;
}

int instCount = 0;
int assembleInstruction(char ** pLabel, char ** pOpcode, char ** pArg1, 
	char ** pArg2, char ** pArg3, char ** pArg4, int * lRet
)
{
	int opcodeInt = getOpcodeInt(*pOpcode);
	int opcodeShift = opcodeInt << 12;
	int result = -1;

	if( strcmp(*pOpcode, ".end") == 0 )
	{
		*lRet = DONE;
		return 0;
	}

	if( strcmp(*pOpcode, ".orig") == 0 )
	{
		result = toNum(*pArg1);
		return result;
	}

	if( strcmp(*pOpcode, ".fill") == 0 )
	{
		result = toNum(*pArg1);
		return result;
	}

	if( opcodeInt == -1 )
		callError(2);

	/* RTI */
	if( strcmp(*pOpcode, "rti") == 0 )
	{
		if( !isEmpty(*pArg1) )
			callError(4);
		return opcodeShift;
	}
	if( strcmp(*pOpcode, "nop") == 0 )
	{
		if( !isEmpty(*pArg1) )
			callError(4);
		return 0;
	}

	/* ADD, AND, XOR, NOT */
	if( instrFormat == INSTR_FORMAT_0 )
	{
		//printf("Format0\n");
		result = assembleFormat0( opcodeShift, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4 );
	}
	
	/* LDB, LDW, STB, STW, JMP, RET */
	if( instrFormat == INSTR_FORMAT_1 )
	{
		//printf("Format1\n");
		result = assembleFormat1( opcodeShift, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4 );
	}

	/* LSHF, RSHFL, RSHFA */
	if( instrFormat == INSTR_FORMAT_2 )
	{
		//printf("Format2\n");
		result = assembleFormat2( opcodeShift, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4 );
	}

	/* JSR, JSRR */
	if( instrFormat == INSTR_FORMAT_3 )
	{
		//printf("Format3\n");
		result = assembleFormat3( opcodeShift, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4 );
	}

	/* TRAP, HALT */
	if( instrFormat == INSTR_FORMAT_4 )
	{
		//printf("Format4\n");
		result = assembleFormat4( opcodeShift, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4 );
	}

	/* BR, LEA */
	if( instrFormat == INSTR_FORMAT_5 )
	{
		//printf("Format5\n");
		result = assembleFormat5( opcodeShift, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4 );
	}

	instrAndFill++;
	PC += 2;
	return result;

}

bool isValidLabel(char * pLabel)
{
	if (strlen(pLabel) < 1 || strlen(pLabel) > 20)
		return false;
	if (!isalpha(pLabel[0]))
		return false;
	if (pLabel[0] == 'x')
		return false;
	if (strcmp(pLabel, "in") == 0)
		return false;
	if (strcmp(pLabel, "out") == 0)
		return false;
	if (strcmp(pLabel, "getc") == 0)
		return false;
	if (strcmp(pLabel, "puts") == 0)
		return false;
	if (isOpcode(pLabel))
		return false;

	return true;
}
void firstPass(char * lLabel, char * lOpcode, char * lArg1, int * lRet)
{
	if( isOpcode(lOpcode) )
	{
		instrCount++;
		instrAndFill++;
	}

	if( strcmp(lOpcode, ".orig") == 0 )
	{	// TODO: if origin is not given a value
		strcpy(symbolTable[symbolCount].label, lOpcode);
		int address = toNum(lArg1);
		if( address < 0 || address % 2 > 0 || address == '\0')
			callError(3);
		symbolTable[symbolCount].address = address;
		origAddress = address;
		origIndex = symbolCount;
		symbolCount++;
	}
	else if( strcmp(lOpcode, ".end") == 0 )
	{
		*lRet = DONE;
	}
	else if( strcmp(lOpcode, ".fill") == 0 )
	{
		strcpy(symbolTable[symbolCount].label, lLabel);
		symbolTable[symbolCount].address = symbolTable[origIndex].address + 2*instrAndFill;
		symbolCount++;
		instrAndFill++;
	}
	else if( !isEmpty(lLabel) )
	{
		// does label already exist?
		if( getSymbolAddress(lLabel) != -1 )
			callError(4);

		if( isValidLabel(lLabel) )
		{
			strcpy(symbolTable[symbolCount].label, lLabel);
			symbolTable[symbolCount].address = (instrCount-1) * 2
												+ symbolTable[origIndex].address;
			symbolCount++;
		} else
		{
			callError(4);
		}
	}
}

void printSymbolTable()
{
	printf("*************************************************\n");
	for( int i = 0; i < symbolCount; i++)
	{
		printf("Index: %d\t Address: 0x%.4X\t Label: %s\n"
			, i, symbolTable[i].address, symbolTable[i].label);
	}
	printf("*************************************************\n");
}

void printResult()
{
	printf("*************************************************\n");
	for( int i = 0; i < instrCount; i++)
	{
		printf("Index: %d   Hex: 0x%.4X   Label: %s   Opcode: %s   Arg1: %s   Arg2: %s   Arg3: %s   Arg4: %s\n"
			, i, instrTable[i].val, instrTable[i].label, instrTable[i].op
			, instrTable[i].arg1, instrTable[i].arg2, instrTable[i].arg3, instrTable[i].arg4);
	}
	printf("*************************************************\n");
}

FILE *infile = NULL;
FILE *outfile = NULL;
int main(int argc, char* argv[]) {

	// parse the command line arguments
	char *prgName = NULL;
	char *iFileName = NULL;
	char *oFileName = NULL;

	prgName = argv[0];
	iFileName = argv[1];
	oFileName = argv[2];

	// open the input and output files
	infile = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w");
	// infile = fopen("test1.txt", "r");
	// outfile = fopen("results1.txt", "w");
	
	if (!infile) {
		printf("Error: Cannot open file %s\n", argv[1]);
		exit(4);
	}
	if (!outfile) {
		printf("Error: Cannot open file %s\n", argv[2]);
		exit(4);
	}

	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
	        *lArg2, *lArg3, *lArg4;

	int lRet;
	int instr = 0;
	bool firstPassDone = false;
	do
	{
		lRet = readAndParse( infile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE ) // DONE == 0 | OK == 1 | EMPTY_LINE == 2
		{
			if( !firstPassDone )
				firstPass(lLabel, lOpcode, lArg1, &lRet);
			else
			{
				instr = assembleInstruction( &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4, &lRet);
				
				if( lRet != DONE )
				{
					// /* Debug purposes */
					// instrTable[instrCount].val = instr;
					// strcpy(instrTable[instrCount].label, lLabel);
					// strcpy(instrTable[instrCount].op, lOpcode);
					// strcpy(instrTable[instrCount].arg1, lArg1);
					// strcpy(instrTable[instrCount].arg2, lArg2);
					// strcpy(instrTable[instrCount].arg3, lArg3);
					// strcpy(instrTable[instrCount].arg4, lArg4);
					// instrTable[instrCount].address = instrCount * 2
					// 								+ symbolTable[origIndex].address;
					
					fprintf( outfile, "0x%.4X\n", instr);
					instrCount++;
				}
			}
		}

		// if( firstPassDone && lRet == DONE)
		// {
		// 	printResult();
		// }
		if( !firstPassDone && lRet == DONE)
		{
			//printSymbolTable();
			firstPassDone = true;
			PC = origAddress;
			instrCount = 0;
			instrAndFill = 0;
			rewind(infile);
			lRet = OK;
		}
	} while( lRet != DONE );

	fclose(infile);
	fclose(outfile);

	return 0;
}

