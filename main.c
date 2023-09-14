// Copyright 2018 Adam Thompson <adam@serialphotog.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial 
// portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>

// Forward declaration for the disassemble routine.
int disassemble(unsigned char*, long);
FILE *f1;
// Entry point
// First argument is treated as the path to the file to disassemble
int main(int argc, char **argv)
{
	FILE *f = fopen(argv[1], "rb");
	if (f == NULL)
	{
		printf("Error: couldn't open %s\n", argv[1]);
		exit(1);
	}

	// Read the file into a memory buffer
	fseek(f, 0L, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);

	unsigned char *buffer = malloc(fsize);
	fread(buffer, fsize, 1, f);
	fclose(f);
	f1 = fopen("test.txt", "wb");
	
	// Perform disassembly
	long pc = 0;
	while (pc < fsize)
	{
		pc += disassemble(buffer, pc);
	}

	free(buffer);
	fclose(f1);
	return 0;
}

// Performs the actual disassembly of an instruction
//
// Params:
//		unsigned char *buffer - The buffer containing the machine code
//		int pc - The program counter. Used to track our place in the buffer
//
// Returns:
//		int - The new program counter location. The program counter is incremented to track
//			  where the next instruction is, particularly after a multi-byte instruction is
//			  encountered.
int disassemble(unsigned char *buffer, long pc)
{
	unsigned char *opcode = &buffer[pc];
	int opbytes = 1;
	short is64 = 0;
	fprintf(f1, "0x%08x, 0x%02x  ", pc, opcode[0]);
if(opcode[0] == 0x48){
	is64 = 1;
	*opcode++;
}
	switch (*opcode)
	{
		case 0x00:
		case 0x08:
		case 0x10:
		case 0x18:
		case 0x28:
		case 0x38:
		case 0xcb:
		case 0xd9:
		case 0xdd:
		case 0xed:
		case 0x90:
			fprintf(f1, "NOP");
			break;
		case 0x89:
		if(is64 == 1) {
			if(opcode[1] == 0xf0)
				fprintf(f1, "MOV\tRAX, RSI");
			else
				fprintf(f1, "MOV\t0x%02x", opcode[1]);
			opbytes = 2;
			break;
		}
		default:
			// We encountered an unknown instruction
			fprintf(f1, "Unknown Instruction: 0x%02x", *opcode);
			break;
	}

	fprintf(f1, "\n");
	return opbytes;
}