#ifndef _CONSOLE_APP_TEMPLATE_H
#define _CONSOLE_APP_TEMPLATE_H

struct menu_table
{
	struct menu_table* p_pre;
	struct menu_table** p_next;
	int n_next_num;
	char** p_text;
	int n_text_line;
	void (*run_task)();
	void (*print_msg)(int n_text_line, char** p_text);
	void (*scanf_msg)(int* n_selt, int n_text_line, struct menu_table* p);
};

typedef struct menu_table MENU_TABLE;

char* read_config_file(char* filename, int* size);
MENU_TABLE* init_menu_table();

void run_menu_table(MENU_TABLE* p);

void free_menu_table(MENU_TABLE* p);

#endif
