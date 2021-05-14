/*
   linker.c
   Assignment #6 BACKUP
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct nament {
	char name[26];  /* Name of label */
	int addr;      /* line number of label */
	struct nament *next; /* next name in symbol table */
} SYMTABENTRY;

void add_symbol(char *, int); //ok
void generate_code(int); //ok
void print_first_pass(void);//ok
void append_table(void);
int get_sym_val(char *);//ok

SYMTABENTRY *symtab = NULL;
FILE *p1, *p2;
char cstr_12[13];

int main(int argc, char* argv[]) {
	int i, start, pc_offset = 0, pc = 0;
	int linum = 0, object_file = 0;
	int line_number, new_pc;
	char instruction[18];
	char symbol[26];

	if (argc > 1 && (strcmp(argv[1], "-o") == 0)) {
		object_file = 1;
	}

	if (object_file == 1) start = 2;
	else start = 1;


	p1 = fopen("/tmp/daedalus", "w+");
	unlink("/tmp/daedalus");

	for (i = start; i < argc; i++) {
		if ((p2 = fopen(argv[i], "r")) == NULL) {
			printf("ERROR: cannot open file %s\n", argv[i]);
			exit(6);
		}

		while (fscanf(p2, "%d %s", &pc, instruction) != EOF) {
			if (pc == 4096) break;
			new_pc = pc + pc_offset;
			symbol[0] = '\0';
			if (instruction[0] == 'U') {
				fscanf(p2, "%s", symbol);
			}

			fprintf(p1, " %d %s %s \n", new_pc, instruction, symbol);
		}
		while (fscanf(p2, "%s %d", symbol, &line_number) != EOF) {
			add_symbol(symbol, line_number + pc_offset);
		}

		pc_offset = new_pc + 1;
		fclose(p2);
	}

	/* if this option was on do it */
	if (object_file) {
		print_first_pass();
		printf("4096 x\n");
		append_table();
		return 0;
	}

	/* do this option if it was on */
	if (linum) {
		print_first_pass();
	}

	generate_code(linum);

	return 0;

}

void print_first_pass() {

	char inbuf[81];

	rewind(p1);
	while (fgets(inbuf, 80, p1) != NULL) {
		printf(" %s", inbuf);
	}

}

void generate_code(int linum) {

	char instr[18];
	int line_number;
	int pc, mask, sym_val, i, j, old_pc, diff;
	char symbol[26];

	line_number = old_pc = 0;
	rewind(p1);


	while (fscanf(p1, "%d %s", &pc, instr) != EOF) {
		if ((diff = pc - old_pc) > 1) {
			for (j = 1; j < diff; j++) {
				printf("1111111111111111 \n");
			}
		}
		old_pc = pc;


		if (instr[0] == 'U') {
			fscanf(p1, "%s", symbol);
			sym_val = get_sym_val(symbol);
			if (sym_val == -1) {
				printf("Error no symbol");
				exit(3);
			}

			for (i = 0; i < 12; i++) {
				cstr_12[i] = '0';
			}

			cstr_12[12] = '\0';
			mask = 2048;

			for (i = 0; i < 12; i++) {
				if (sym_val & mask) {
					cstr_12[i] = '1';
				}
				mask >>= 1;
			}

			for (i = 0; i < 12; i++) {
				instr[i + 5] = cstr_12[i];
			}

			printf("%s\n", &instr[1]);

		}
		else {
			printf("%s\n", instr);
		}
	}
	return;

}

/* add to symbol table */
void add_symbol(char *symbol, int line_number) {

	SYMTABENTRY *newSymbol;

	newSymbol = (SYMTABENTRY *)malloc(sizeof(SYMTABENTRY));

	/* Have symtab point to the newest symbol */
	if (symtab == NULL) {
		symtab = newSymbol;
		newSymbol->next = NULL;
	}
	else {
		newSymbol->next = symtab;
		symtab = newSymbol;
	}

	strcpy(newSymbol->name, symbol);
	newSymbol->addr = line_number;

	return;
}

/* Traverse list print every node */
void append_table() {

	while (symtab != NULL) {
		printf("%s \t\t\t%d\n", symtab->name, symtab->addr);
		symtab = symtab->next;
	}

	return;
}

/* Return address of symbol to resolve code */
/* Traverse list looking for symbol */
int get_sym_val(char *symbol) {

	int cmp;
	SYMTABENTRY *list = symtab;

	while (list != NULL) {
		cmp = strcmp(list->name, symbol);
		if (cmp == 0) {
			return list->addr;
		}
		else {
			list = list->next;
		}
	}
	return -1;
}
