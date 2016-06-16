#include "../../header/text_editor/shared.h"

//Инициальзирует все переменные и графику
void win_init(char *fpatch)	
{
	//Инициализируем все переменные
	ccol = 0;
	cline = 0;
	pindex = 0;
	cindex = 0;
	strcpy(patch, fpatch);
	cont_len = read_content(patch, &content);

	//Если файл не удалось открыть
	if(cont_len == ERR_OPEN)
	{
		content = malloc(sizeof(char) * 64);
		strcpy(content, "This is a new text file!");
		cont_len = strlen(content) + 1;
	}

	//Инициализация ncurses
	initscr();						//ncurses mode
	noecho();						//Отключить вывод клвиш
	keypad(stdscr, TRUE);			//Включаем обработку спец клавиш
	signal (SIGWINCH, win_resize);	//Вызов функции при ресайзе окна

	//Создаем окна
	win[WIN_BC] = newwin(LINES - 2, COLS, 0, 0);
	win[WIN_C] = subwin(win[WIN_BC], LINES - 6, COLS - 4, 2, 2);
	win[WIN_P] = newwin(2, COLS - 2, LINES - 2, 1);

	//Цвета
	start_color();
	assume_default_colors(COLOR_WHITE,COLOR_BLUE);	//Цвета по умолчанию
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);			//Имя файла
	refresh();
	//Отображение
	winref_all();
}

//Действие при изменении размера терминала
void win_resize()								
{
	struct winsize size;

	//Получаем новые размеры терминала
	ioctl(0, TIOCGWINSZ, &size);
	COLS = size.ws_col;
	LINES = size.ws_row;
	
	ccol = 0;
	cline = 0;
	pindex = 0;
	cindex = 0;
	//Изменяем размеры окон
	resizeterm(LINES, COLS);
	wresize(win[WIN_BC], LINES - 2, COLS);
	wresize(win[WIN_C], LINES - 6, COLS - 4);
	wresize(win[WIN_P], 2, COLS - 2);

	//Смещаем окна после изменения размера
	mvwin(win[WIN_P], LINES - 2, 1);

	//Перерисовка
	winref_all();
}

//Очистка памяти и отключение графики
void win_destroy()								
{

	endwin();
}

//Перерисовка всего
void winref_all()
{
	clear();
	winref_border(WIN_BC);
	winref_panel(WIN_P);
	winref_content(WIN_C);
}

//Перерисовка рамки
void winref_border(unsigned index)
{
	wclear(win[index]);
	box(win[index], 0, 0);
	mvwprintw(win[index], 0, (COLS - strlen(patch)) / 2, patch);

	//Обновление
	refresh();
	wrefresh(win[index]);
}

void winref_content(unsigned index)
{
	wclear(win[index]);
	mvwprintw(win[index], 0, 0, content + pindex);
	mvwprintw(win[WIN_C], cline, ccol, "");
	wrefresh(win[index]);

	scrollok(win[WIN_C], TRUE);
	wscrl(win[WIN_C], +3);
	scrollok(win[WIN_C], FALSE);
}

//Перерисовка панели
void winref_panel(unsigned index)
{
	wclear(win[index]);
	mvwprintw(win[index], 0, 0, "[ESC] - Exit  [F2] - Save");
	wrefresh(win[index]);
}
