#define _CRT_SECURE_NO_WARNINGS
#include "console_app_template.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "run_task.h"
#include "print_menu.h"

void (*printf_menu[])(int n_text_line, char** p_text) = {
	NULL, printf_menu2, printf_menu7, printf_menu8, printf_menu4, printf_menu16, printf_menu17, printf_menu18 };

void (*run_menu[])() = { NULL, run_menu1, run_menu9, run_menu10,
run_menu11, run_menu12, run_menu13, run_menu14, run_menu3, run_menu15,
run_menu20, run_menu19, run_menu22, run_menu21, run_menu5, run_menu6 };

static char str2hex_byte(char* str)
{
	char temp = 0;
	temp = (str[0] >= '0' && str[0] <= '9') ? (str[0] - '0') : (str[0] - 'A' + 0xa);
	temp <<= 4;
	temp |= (str[1] >= '0' && str[1] <= '9') ? (str[1] - '0') : (str[1] - 'A' + 0xa);
	return temp;
}

char* read_config_file(char* filename, int* size)
{
	size_t lSize;
	size_t result;
	char* msg = NULL;
	FILE* p_file = fopen(filename, "rb");
	if (p_file == NULL)
	{
		return NULL;
	}
	
	fseek(p_file, 0, SEEK_END);
	lSize = ftell(p_file);
	rewind(p_file);

	// allocate memory to contain the whole file:
	msg = (char*)malloc(sizeof(char) * lSize + 1);
	if (msg == NULL)
		return NULL;
	memset(msg, 0, sizeof(char) * lSize + 1);
	if (msg == NULL)
		return NULL;

	// copy the file into the buffer:
	result = fread(msg, 1, lSize, p_file);
	if (result != lSize)
		return NULL;
	fclose(p_file);
	*size = (int)result;
	return msg;
}

struct list
{
	char* p_data;
	struct list* p_next_node;
};

void free_list_data(struct list* plist)
{
	if (plist->p_next_node)
	{
		free_list_data(plist->p_next_node);
		plist->p_next_node = NULL;
	}
	free(plist);
	plist = NULL;
}

void create_menu_table(char* msg, MENU_TABLE* p_node)
{
	char* p_msg = msg;
	int n_rackets[2] = { 0 };
	int n_line[2] = { 0 };
	int i = 0;
	struct list* p_list = NULL;
	struct list* p_list_head = NULL;
	p_list = (struct list*)malloc(sizeof(struct list));
	if (p_list == NULL)
		return;
	p_list_head = p_list;
	memset(p_list, 0, sizeof(struct list));
	
	while (1)
	{
		n_rackets[0] += (*p_msg == '{');
		n_rackets[1] += (*p_msg == '}');
		if (n_rackets[0] - n_rackets[1] == 1 && *p_msg != '{' && *p_msg != '}' && *p_msg == '\n')
		{
			*p_msg = '\0';
			p_list->p_data = (char*)(p_msg + 1);
			n_line[0] += (*p_list->p_data != '{' && *p_list->p_data != '}');
			n_line[1] += (*p_list->p_data == '{' || *p_list->p_data == '}');
			p_list->p_next_node = (struct list*)malloc(sizeof(struct list));
			if (p_list->p_next_node == NULL)
				return;
			memset(p_list->p_next_node, 0, sizeof(struct list));
			p_list = p_list->p_next_node;
		}
		
		if (n_rackets[0] == n_rackets[1])
			break;
		p_msg++;
	}

	n_line[1]--;
	p_node->n_text_line = n_line[0];
	p_node->p_text = (char**)malloc(p_node->n_text_line * sizeof(char*));
	if (p_node->p_text == NULL)
		return;
	p_node->p_next = (MENU_TABLE**)malloc(p_node->n_text_line * sizeof(MENU_TABLE*));
	if (p_node->p_next == NULL)
		return;

	memset(p_node->p_next, 0, p_node->n_text_line * sizeof(MENU_TABLE*));
	p_list = p_list_head;
	p_node->n_next_num = n_line[0];
	for (i = 0; i < n_line[0]; ++i)
	{
		char* p_msg_next_node = NULL;
		char* p_msg_id = NULL;
		p_node->p_next[i] = (MENU_TABLE*)malloc(sizeof(MENU_TABLE));
		if (p_node->p_next[i] == NULL)
		{
			return;
		}
		memset(p_node->p_next[i], 0, sizeof(MENU_TABLE));
		p_node->p_next[i]->p_pre = p_node;
		while (*p_list->p_data == '{' || *p_list->p_data == '}') 
		{
			p_msg_next_node = p_list->p_data;
			p_list = p_list->p_next_node; 
			if (*p_list->p_data == '}')
				p_msg_next_node = NULL;
		}
		p_node->p_text[i] = malloc((size_t)(strlen(p_list->p_data) + 1));
		if (p_node->p_text[i])
		{
			memset(p_node->p_text[i], 0, strlen(p_list->p_data) + 1);
			memcpy(p_node->p_text[i], p_list->p_data, strlen(p_list->p_data) - 4);
		}
		p_msg_id = p_list->p_data + strlen(p_list->p_data) - 4;
		p_node->p_next[i]->print_msg = printf_menu[str2hex_byte(p_msg_id)];
		p_node->p_next[i]->run_task = run_menu[str2hex_byte(p_msg_id + 2)];
		p_list = p_list->p_next_node;
		//printf("%s\r\n", p_node->p_text[i]);
		if (p_msg_next_node != NULL)
		{
			create_menu_table(p_msg_next_node, p_node->p_next[i - 1]);
		}
	}

	free_list_data(p_list_head);
}

int theme_msg(char** msg)
{
	int i = 0;
	int j = 0;
	int n_size = 0;
	char* p_msg = *msg;
	char* p_msg_theme = NULL;
	memset(p_msg_theme, 0, n_size);
	while (*p_msg != '{') { p_msg++; };
	n_size = (int)strlen(p_msg);
	p_msg_theme = (char*)malloc(n_size);
	if (p_msg_theme == NULL)
		return -1;
	memset(p_msg_theme, 0, n_size);
	for (; i < n_size; ++i)
	{
		if (p_msg[i] != '\t' && p_msg[i] != '\r')
		{
			p_msg_theme[j] = p_msg[i];
			if (p_msg_theme[j - 1] == '\n' && p_msg_theme[j] == ' ')
			{
				p_msg_theme[j] = '\0';
				continue;
			}

			if (p_msg_theme[j] == '\n' && (p_msg_theme[j - 1] == ' ' || p_msg_theme[j - 1] == '\n'))
			{
				p_msg_theme[j] = '\0';
				p_msg_theme[--j] = '\n';
			}
			j++;
		}
	}
	free(*msg);
	*msg = p_msg_theme;
	return 0;
}

static void printf_main_menu(int n_text_line, char** p_text)
{
	int i = 0;
	for (; i < n_text_line; ++i)
	{
		printf("%s\r\n", p_text[i]);
	}
}

void scanf_msg(int n_text_line, struct menu_table* p)
{
	int n_selt = 0;
	printf("pelease choose the item:");
	scanf("%d", &n_selt);
	if (n_selt == n_text_line)
	{
		p = p->p_pre;
		p = p->p_pre;
		return;
	}
}

MENU_TABLE* init_menu_table()
{
	int n_file_size;
	char* msg = NULL;
	msg = read_config_file("config.txt", &n_file_size);
	theme_msg(&msg);
	//printf("%s", msg);
	MENU_TABLE* p_node = NULL;
	p_node = (MENU_TABLE*)malloc(sizeof(MENU_TABLE));
	if (p_node)
	{
		memset(p_node, 0, sizeof(MENU_TABLE));
		p_node->print_msg = printf_main_menu;
		create_menu_table(msg, p_node);
	}
	free(msg);
	return p_node;
}

void run_menu_table(MENU_TABLE* p)
{
	int n_selt = 0;
	MENU_TABLE* ptemp = p;
	while (ptemp != NULL)
	{
		if (ptemp->print_msg != NULL)
			ptemp->print_msg(ptemp->n_text_line, ptemp->p_text);
		printf("pelease choose the item:");
		scanf("%d", &n_selt);

		if (n_selt == ptemp->n_text_line)
		{
			ptemp = ptemp->p_pre;
			continue;
		}

		if (ptemp->p_next[n_selt - 1]->run_task != NULL)
			ptemp->p_next[n_selt - 1]->run_task();
		else
			ptemp = ptemp->p_next[n_selt - 1];
	}
}

void free_menu_table(MENU_TABLE* p)
{
	MENU_TABLE* ptemp = p;
	if (ptemp == NULL)
		return;

	if (ptemp->n_next_num)
	{
		int i = 0;

		for (; i < ptemp->n_next_num; ++i)
		{
			if (ptemp->p_next[i])
			{
				free_menu_table(ptemp->p_next[i]);
				free(ptemp->p_next[i]);
				ptemp->p_next[i] = NULL;
			}	
		}
		free(ptemp->p_next);
		ptemp->p_next = NULL;
		ptemp->n_next_num = 0;
	}

	if (ptemp->n_text_line)
	{
		int i = 0;
		for (; i < ptemp->n_text_line; ++i)
		{
			free(ptemp->p_text[i]);
			ptemp->p_text[i] = NULL;
		}
		free(ptemp->p_text);
		ptemp->p_text = NULL;
		ptemp->n_text_line = 0;
	}

	if (ptemp->p_pre == NULL)
	{
		free(ptemp);
		ptemp = NULL;
	}
}