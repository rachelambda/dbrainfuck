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

	void* cparr = malloc(*length * byte_size);
	for(int n = 0; n < *length * byte_size; n++){
		((byte*)cparr)[n] = ((byte*)array)[n];
	}

	array = cparr;
}

void empty(void* array, unsigned int* length){
	*length = 0;
	free(array);
	array = malloc(0);
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
					contract(instretarr, 4, &instretarrsize);
				}

				break;

			// Jump back from loop
			case ']':
				if(*mempointer){
#ifdef DEBUG
					printf("] *mempointer: %d\n", *mempointer);
#endif
					instoffset = instretarr[instretarrsize - 1];
				}else{
#ifdef DEBUG
					printf("] *mempointer: %d, contracting\n", *mempointer);
#endif
					contract(instretarr, 4, &instretarrsize);
				}
				break;

			// Store the current pointer address
			case '(':
				expand(memoffsetstore, sizeof(memoffsetstore), &memoffsetstoresize);
				memoffsetstore[memoffsetstoresize - 1].memoffset = memoffset;
				memoffsetstore[memoffsetstoresize - 1].memptr = mempointer;
				break;

			// Load the latest pointer address
			case ')':
#ifdef DEBUG
				printf("Before ) values: memoffset: %d, memptr: %d\n", memoffset, mempointer);
#endif
				if(memoffsetstoresize < 1){
					printf("Error at instruction #%d: '%c'. No matching '('.\n", instoffset, instarr[instoffset]);
					return -1;
				}
				memoffset = memoffsetstore[memoffsetstoresize - 1].memoffset;
				mempointer = memoffsetstore[memoffsetstoresize - 1].memptr;
#ifdef DEBUG
				printf("After ( values: memoffset: %d, memptr: %d\n", memoffset, mempointer);
#endif
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
			case '$':
				savedval = *mempointer;
				break;

			// Load saved value of byte
			case '`':
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
#ifdef DEBUG
				printf("%d\n", instretarr[0]);
#endif
				for(int n = instretarr[0];; n++){ 
#ifdef DEBUG
					printf("instarr[%d] = %c\n", n, instarr[n]);
#endif
					if(instarr[n] == '['){
						leftbrackets++;
#ifdef DEBUG
						printf("Found leftbracket!\n");
#endif
					}
					else if(instarr[n] == ']'){
						rightbrackets++;
#ifdef DEBUG
						printf("Found leftbracket!\n");
#endif
					}
					if(leftbrackets == rightbrackets){
						instoffset = n; 
						break;
#ifdef DEBUG
						printf("Equal brackets\n");
#endif
					}
				}
				empty(instretarr, &instretarrsize);
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
					memsize += memoffset + *mempointer;
					expand(mem, 1, &memsize);
				}
				memoffset += *mempointer;
				mempointer += *mempointer;
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
