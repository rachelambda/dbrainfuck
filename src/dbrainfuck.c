#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define byte unsigned char

char* helpmsg = \
		  "dbrainfuck, usage:\n"\
		  "\n"\
		  "dbrainfuck <arg> [flags]\n"\
		  "\n"\
		  "flags:\n"\
		  "\n"\
		  "-g enable debug"\
		  "-a use arg as code and not filename";

byte debug = 0;
byte readarg = 0;

byte hex_to_char(char* inpstr){
	byte outbyte;
	char input[3] = {inpstr[0], inpstr[1], '\0'}; sscanf(input, "%x", &outbyte);
	return outbyte;
}

void expand(void** array, unsigned char byte_size, unsigned int* length){
	++*length;
	*array = realloc(*array, *length * byte_size);
}

void contract(void** array, unsigned char byte_size, unsigned int* length){
	--*length;
	*array = realloc(*array, *length * byte_size);
}

void empty(void** array, unsigned int* length){
	*length = 0;
	free(*array);
	*array = NULL;
}

void handle_flags(int argc, char** argv){
	for(int n = 0; n < argc; n++){
		if(argv[n][0] == '-' && argv[n][1] == 'g')
			debug = 1;
		if(argv[n][0] == '-' && argv[n][1] == 'a')
			readarg = 1;
	}
}

int main(int argc, char** argv){
	// Handle args
	if(argc < 2){
		puts(helpmsg);
		return -1;
	} else if(argc > 2){
		handle_flags(argc, argv);
	}

	char* instarr = NULL;
	size_t instlen = 0;
	if (!readarg) {
		// Load program into memory
		FILE* instruction_fp = fopen(argv[1], "rb");
		instlen = getdelim(&instarr, &instlen, '\0', instruction_fp);
		fclose(instruction_fp);
	} else {
		instarr = argv[1];
		printf("instarr: %s\n", instarr);
		instlen = strlen(instarr);
		printf("instlen: %d\n", instlen);
	}
	printf("instlen: %d\n", instlen);

	// Filter whitespace and comments
	char* cpinst = malloc(instlen);
	unsigned int cpinstfill = 0;
	unsigned char comment = 0;
	for(int n = 0; n <= instlen; n++){
		if(instarr[n] == '#'){
			comment = 1;
		} else if(instarr[n] == '\n'){
			comment = 0;
		} else if(!(instarr[n] == ' ' || instarr[n] == '\n' || instarr[n] == '\t' || comment)){
			cpinst[cpinstfill] = instarr[n];
			cpinstfill++;
		}
	}
	if (!readarg)
		free(instarr);
	instarr = cpinst;
	instlen = cpinstfill;

	instarr = realloc(instarr, instlen);

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

if(debug){
	printf("instlen: %d\n", instlen);
}

	// Initialize variables for the memory
	unsigned int memsize = 64;
	unsigned int memoffset = 0;

	byte* mem = malloc(memsize);
	byte* mempointer = mem;
	for(int n = 0; n < memsize; n++)
		mem[n] = 0x0;

if(debug){
	printf("'%s'\n", instarr);
}

	// Needed for nested loops
	unsigned int leftbrackets;
	unsigned int rightbrackets;

	while(instoffset <= instlen){
		switch(instarr[instoffset]){

			// Increment pointer value
			case '>':
				if(memoffset == memsize){
					expand((void**)&mem, 1, &memsize);
					mem[memsize-1] = 0x0;
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
					expand((void**)&instretarr, 4, &instretarrsize);
					instretarr[instretarrsize - 1] = instoffset;
if(debug){
	printf("[ *mempointer: %u, expanding\n", *mempointer);
}
				} else {
if(debug){
	printf("[ *mempointer: %u\n", *mempointer);
}
					leftbrackets = 1;
					rightbrackets = 0;
					for(int n = instoffset + 1;; n++){ 
						if(n > instlen){
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
					contract((void**)&instretarr, 4, &instretarrsize);
				}

				break;

			// Jump back from loop
			case ']':
				if(instretarrsize <= 0){
if(debug){
					printf("] *mempointer: %u, skipping\n", instoffset, instarr[instoffset]);
}
				}
				if(*mempointer){
if(debug){
					printf("] *mempointer: %u\n", *mempointer);
}
					instoffset = instretarr[instretarrsize - 1];
				}else{
if(debug){
					printf("] *mempointer: %u, contracting\n", *mempointer);
}
					contract((void**)&instretarr, 4, &instretarrsize);
				}
				break;

			// Store the current pointer address
			case '(':
				expand((void**)&memoffsetstore, sizeof(memoffsetstore), &memoffsetstoresize);
				memoffsetstore[memoffsetstoresize - 1].memoffset = memoffset;
				memoffsetstore[memoffsetstoresize - 1].memptr = mempointer;
				break;

			// Load the latest pointer address
			case ')':
if(debug){
				printf("Before ) values: memoffset: %d, memptr: %u\n", memoffset, mempointer);
}
				if(memoffsetstoresize < 1){
					printf("Error at instruction #%d: '%c'. No matching '('.\n", instoffset, instarr[instoffset]);
					return -1;
				}
				memoffset = memoffsetstore[memoffsetstoresize - 1].memoffset;
				mempointer = memoffsetstore[memoffsetstoresize - 1].memptr;
if(debug){
				printf("After ( values: memoffset: %d, memptr: %u\n", memoffset, mempointer);
}
				break;

			// Pop loaded pointer address
			case '/':
				if(memoffsetstoresize < 1){
					printf("Error at instruction #%d: '%c'. No '(' to pop.\n", instoffset, instarr[instoffset]);
					return -1;
				}
				contract((void**)&memoffsetstore, 8, &memoffsetstoresize);
				break;

			// Read value of byte and save
			case '$':
				savedval = *mempointer;
				break;

			// Load saved value of byte
			case '@':
				*mempointer = savedval;
				break;
			
			// Set value of byte
			case '^':
				*mempointer = hex_to_char(&instarr[instoffset + 1]);
				instoffset += 2; // Cause of the two hex letters
				break;

			// Exit loop (think break) or exit program
			case '\\':
				if(instretarrsize == 0){
					exit(*mempointer);
				}
				leftbrackets = 0;
				rightbrackets = 0;
if(debug){
				printf("%d\n", instretarr[0]);
}
				for(int n = instretarr[0];; n++){ 
if(debug){
					printf("instarr[%d] = %c\n", n, instarr[n]);
}
					if(instarr[n] == '['){
						leftbrackets++;
if(debug){
						printf("Found leftbracket!\n");
}
					}
					else if(instarr[n] == ']'){
						rightbrackets++;
if(debug){
						printf("Found leftbracket!\n");
}
					}
					if(leftbrackets == rightbrackets){
						instoffset = n; 
						break;
if(debug){
						printf("Equal brackets\n");
}
					}
				}
				empty((void**)&instretarr, &instretarrsize);
				break;

			// Move mempointer *mempointer to the left
			case '{':
				if(memoffset - *mempointer < 0){
					printf("Error at instruction #%d: '%c'. Attempting to move to index `%d`.\n", instoffset, instarr[instoffset], memoffset - *mempointer);
					return -1;
				}
				memoffset -= *mempointer;
				mempointer -= *mempointer;
				break;
		
			// Move mempointer *mempointer to the right
			case '}':
				if(memoffset + *mempointer >= memsize){
					int oldmemsize = memsize;
					memsize = memoffset + *mempointer - 1;
					expand((void**)&mem, 1, &memsize);
					for(int n = oldmemsize; n < memsize; n++)
						mem[n] = 0x0; 
				}
				memoffset += *mempointer;
				mempointer += *mempointer;
				break;	  

			// Compare values
			case '=':
				savedval = (savedval == *mempointer);
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
