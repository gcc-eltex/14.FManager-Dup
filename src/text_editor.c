#include "../header/text_editor/shared.h"

int main(int argc, char *argv[])
{
	int key = 0;

	if(argc != 2)
	{
		printf("Error: invalid parameters\n");
		exit(ERR_OPEN);
	}
	win_init(argv[1]);
	while((key = getch()) != 27)
	{
		switch(key)
		{
			case KEY_RIGHT:
				action_mvright();
			break;
			case KEY_LEFT:
				action_mvleft();
			break;
			case KEY_DOWN:
				action_mvdown();
			break;
			case KEY_UP:
				action_mvup();
			break;
			case KEY_F(2):
				write_content(patch, content);
			break;
			case KEY_RESIZE:
				;
			break;
			case KEY_BACKSPACE:
				action_backspace();
			break;
			case '\n':
				action_insert(key);
				action_mvdown();
			break;
			default:
				action_insert((char)key);
			break;
		}
	}
	win_destroy();
	return 0;
}
