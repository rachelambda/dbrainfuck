#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

#define byte unsigned char

byte hex_to_char(char* inpstr){
	byte outbyte;
	char input[3] = {inpstr[0], inpstr[1], '\0'};
	sscanf(input, "%x", &outbyte);
	return outbyte;
}

void expand(void* array, unsigned char byte_size, unsigned int* length){
	++*length;
	array = realloc(array, *length * byte_size);
}

void contract(void* array, unsigned char byte_size, unsigned int* length){
	*length--;
	array = realloc(array, *length * byte_size);
}

void handle_flags(int argc, char** argv){
	for(int argn = argc; argn > 0; argn--){
	}
}

int main(int argc, char** argv){
	// Handle args
	if(argc < 2){
		printf("dbrainfuck, not enough args given. Usage: dbrainfuck <path to program>\n");
		return -1;
	} else if(argc > 2){
		handle_flags(argc, argv);
	}

	// Load program into memory
	FILE* instruction_fp = fopen(argv[1], "rb");
	char* instarr = NULL;
	size_t instlen;
	instlen = getdelim(&instarr, &instlen, '\0', instruction_fp);
	fclose(instruction_fp);

	// Filter whitespace

	// Create variables for the program instructions
	unsigned int instoffset = 0;
	unsigned int instretarrsize = 0;
	unsigned int* instretarr = malloc(instretarrsize * sizeof(unsigned int));
	instlen -= 2; // Remove newline and 0x0 from end of file

	// Save val
	byte savedval = 0x0;

	// Create varibales for storing pointer positions
	typedef struct mempos_struct{
		unsigned int memoffset;
		byte* memptr;
	} mempos_struct;
	unsigned int memoffsetstoresize = 0;
	mempos_struct* memoffsetstore = malloc(memoffsetstoresize);

#ifdef DEBUG
	printf("instlen: %d\n", instlen);
#endif

	// Initialize variables for the memory
	unsigned int memsize = 64;
	unsigned int memoffset = 0;

	byte* mem = malloc(memsize);
	byte* mempointer = mem;
	for(int n = 0; n < memsize; n++)
		mem[n] = 0x0;

#ifdef DEBUG
	printf("'\n%s'\n", instarr);
#endif

	// Needed for nested loops
	unsigned int leftbrackets;
	unsigned int rightbrackets;

	while(instoffset <= instlen){
		switch(instarr[instoffset]){

			// Increment pointer value
			case '>':
				if(memoffset == memsize){
					expand(mem, 1, &memsize);
				}
				memoffset++;
				mempointer++;
				break;

			// Lower pointer value
			case '<':
				if(memoffset == 0){
					printf("Error at instruction #%d: '%c'. Already at the start of memory.\n", instoffset, instarr[instoffset]);
					return -1;
				}
				memoffset--;
				mempointer--;
				break;

			// Increment byte value
			case '+':
				++*mempointer;
				break;

			// Lower byte value
			case '-':
				--*mempointer;
				break;

			// Read byte
			case '.':
				putchar(*mempointer);
				break;

			// Print byte
			case ',':
				*mempointer = getchar();
				break;

			// Start of while
			case '[':
				if(*mempointer){
					expand(instretarr, 4, &instretarrsize);
					instretarr[instretarrsize - 1] = instoffset;
				} else {
					leftbrackets = 1;
					rightbrackets = 0;
					for(int n = instoffset + 1;; n++){ 
						if(!n < instlen){
							printf("Error at instruction #%d: '%c'. No matching ']'.\n", instoffset, instarr[instoffset]);
							return -1;
						}
						if(instarr[n] == '[')
							leftbrackets++;
						else if(instarr[n] == ']')
							rightbrackets++;
						if(leftbrackets == rightbrackets){
							instoffset = n; // Found right bracket, will add one though so just set it to the adress of it for now
						}
					}
					contract(instretarr, 4, &instretarrsize);
				}

				break;

			// Jump back from loop
			case ']':
				if(*mempointer){
					instoffset = instretarr[instretarrsize - 1];
				}else{
					contract(instretarr, 4, &instretarrsize);
				}
				break;

			// Store the current pointer address
			case '(':
				expand(memoffsetstore, 8, &memoffsetstoresize);
				memoffsetstore[memoffsetstoresize - 1].memoffset = memoffset;
				memoffsetstore[memoffsetstoresize - 1].memptr = mempointer;
				break;

			// Load the latest pointer address
			case ')':
				if(memoffsetstoresize < 1){
					printf("Error at instruction #%d: '%c'. No matching '('.\n", instoffset, instarr[instoffset]);
					return -1;
				}
				memoffset = memoffsetstore[memoffsetstoresize - 1].memoffset;
				mempointer = memoffsetstore[memoffsetstoresize - 1].memptr;
				break;

			// Pop loaded pointer address
			case 'p':
				if(memoffsetstoresize < 1){
					printf("Error at instruction #%d: '%c'. No '(' to pop.\n", instoffset, instarr[instoffset]);
					return -1;
				}
				contract(memoffsetstore, 8, &memoffsetstoresize);
				break;

			// Read value of byte and save
			case 'r':
				savedval = *mempointer;
				break;

			// Load saved value of byte
			case 'l':
				*mempointer = savedval;
				break;
			
			// Set value of byte
			case '^':
				*mempointer = hex_to_char(&instarr[instoffset + 1]);
				instoffset += 2; // Cause of the two hex letters
				break;

			// Exit loop (think break) or exit program
			case '\\':
				leftbrackets = 1;
				rightbrackets = 0;
				for(int n = instoffset + 1;; n++){ 
					if(!n < instlen){
						exit(*mempointer);
					}
					if(instarr[n] == '[')
						leftbrackets++;
					else if(instarr[n] == ']')
						rightbrackets++;
					if(leftbrackets == rightbrackets){
						instoffset = n; // Found right bracket, will add one though so just set it to the adress of it for now
					}
				}
				contract(instretarr, 4, &instretarrsize);
				break;
			
			// Handle white space
			case '\n':
				break;
			case '\t':
				break;
			case ' ':
				break;

			// Error if char is not recognized
			default:
				printf("Error at instruction #%d: '%c'. Instruction not recognized.\n", instoffset, instarr[instoffset]);
				return -1;
		}
		instoffset++;
	}
	return 1;
}
