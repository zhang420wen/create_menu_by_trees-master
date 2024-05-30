#define _CRT_SECURE_NO_WARNINGS
#include "console_app_template.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

int main()
{
	MENU_TABLE* pmenu_table = init_menu_table();
	run_menu_table(pmenu_table);
	free_menu_table(pmenu_table);
	//free(pmenu_table);
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif 
	return 0;
}