#include "../../header/file_manager/shared.h"

//Инициальзирует все переменные и графику
void win_init()	
{
	//Инициализируем все переменные
	dir_print = 0;
	dir_inwin = 0;
	win_active = WIN_L;
	getcwd(dir_patch[WIN_L], 1024);
	getcwd(dir_patch[WIN_R], 1024);
	dir_content[WIN_L] = NULL;
	dir_content[WIN_R] = NULL;
	dir_count[WIN_L] = dir_get(dir_patch[WIN_L], &(dir_content[WIN_L]));
	dir_count[WIN_R] = dir_get(dir_patch[WIN_R], &(dir_content[WIN_R]));
	getcwd(te_path, 1024);
	strcat(te_path, "/bin/text_editor");

	//Инициализация ncurses
	initscr();						//ncurses mode
	refresh();	
	noecho();						//Отключить вывод клвиш
	curs_set(0);					//Прячем курсор
	keypad(stdscr, TRUE);			//Включаем обработку спец клавиш
	signal (SIGWINCH, win_resize);	//Вызов функции при ресайзе окна

	//Создаем окна
	win[WIN_BL] = newwin((LINES - 2), floor(COLS / 2), 0, 0);
	win[WIN_BR] = newwin((LINES - 2), COLS - floor(COLS / 2), 0, floor(COLS / 2));
	win[WIN_L] = newwin((LINES - 5), floor(COLS / 2 - 2), 2, 1);
	win[WIN_R] = newwin((LINES - 5), COLS - floor(COLS / 2) - 2, 2, floor(COLS / 2) + 1);
	win[WIN_PB] = newwin(2, COLS, LINES - 2, 0);
	win[WIN_CF] = NULL;
	//Цвета
	start_color();
	assume_default_colors(COLOR_WHITE,COLOR_BLUE);	//Цвета по умолчанию
	//Цветовые пары
	init_pair(1, COLOR_BLACK, COLOR_CYAN);			//Выбранная директори
	init_pair(2, COLOR_CYAN, COLOR_WHITE);			//Нижняя панель
	init_pair(3, COLOR_YELLOW, COLOR_BLUE);			//Заколовок таблицы
	
	winref_all();
}

//Инициальзирует только графику
void win_rerun()
{
	//Инициализация ncurses
	initscr();						//ncurses mode
	refresh();	
	noecho();						//Отключить вывод клвиш
	curs_set(0);					//Прячем курсор
	keypad(stdscr, TRUE);			//Включаем обработку спец клавиш
	signal (SIGWINCH, win_resize);	//Вызов функции при ресайзе окна

	//Создаем окна
	win[WIN_BL] = newwin((LINES - 2), floor(COLS / 2), 0, 0);
	win[WIN_BR] = newwin((LINES - 2), COLS - floor(COLS / 2), 0, floor(COLS / 2));
	win[WIN_L] = newwin((LINES - 5), floor(COLS / 2 - 2), 2, 1);
	win[WIN_R] = newwin((LINES - 5), COLS - floor(COLS / 2) - 2, 2, floor(COLS / 2) + 1);
	win[WIN_PB] = newwin(2, COLS, LINES - 2, 0);

	//Цвета
	start_color();
	assume_default_colors(COLOR_WHITE,COLOR_BLUE);	//Цвета по умолчанию
	//Цветовые пары
	init_pair(1, COLOR_BLACK, COLOR_CYAN);			//Выбранная директори
	init_pair(2, COLOR_CYAN, COLOR_WHITE);			//Нижняя панель
	init_pair(3, COLOR_YELLOW, COLOR_BLUE);			//Заколовок таблицы
	
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

    if(win[WIN_L] == NULL ||  win[WIN_R] == NULL)
    	return;
    //Очистка экрана
	clear();
	//refresh();
	
	//Изменяем размеры окон
	resizeterm(LINES, COLS);
	wresize(win[WIN_BL], (LINES - 2), floor(COLS / 2));
	wresize(win[WIN_BR], (LINES - 2), COLS - floor(COLS / 2));
	wresize(win[WIN_L], (LINES - 5), floor(COLS / 2 - 2));
	wresize(win[WIN_R], (LINES - 5), COLS - floor(COLS / 2) - 2);
	wresize(win[WIN_PB], 2, COLS);

	//Смещаем окна после изменения размера
	mvwin(win[WIN_BR], 0, floor(COLS / 2));
	mvwin(win[WIN_R], 2, floor(COLS / 2) + 1);
	mvwin(win[WIN_PB], LINES - 2, 0);
	mvwin(win[WIN_CF], (LINES - 30) / 2, (COLS - 6) / 2);

	//Обновляем 
	refresh();
	winref_all();
}

//Очистка памяти и отключение графики
void win_destroy()								
{
	clear();
	refresh();
	endwin();
	win[WIN_L] = NULL;
	win[WIN_R] = NULL;
	system("clear");
}

//Перерисовывает все
void winref_all()
{
	winref_panel(WIN_PB);
	winref_border(WIN_BL);
	winref_border(WIN_BR);
	winref_content(WIN_L);
	winref_content(WIN_R);
}

//Перерисовывает панель
void winref_panel(unsigned index)
{
	wclear(win[index]);
	mvwprintw(win[index], 0, 0, " [ESC] Exit");
	wrefresh(win[index]);
}

//Перерисовывает окно с содержимым
void winref_content(unsigned index)
{
	int width;						//Ширина окна
	int height; 					//Высота окна
	char content[256];				//Строка для вывода
	unsigned lname, lsize, ldate;	//Длина каждого параметра
	char fname[32], fsize[32], fdate[32], fline[32];//Формат вывода каждого параметра

	//Очстка
	wclear(win[index]);

	//Определение формата вывода и размера каждого поля
	getmaxyx(win[index], height, width);
	lname = width * 0.5 + 1; 
	lsize = width * 0.2;
	ldate = width - lname - lsize;
	sprintf(fname, "%%-%d.%ds", lname, lname);
	sprintf(fsize, "%%%d.%ds", lsize, lsize);
	sprintf(fdate, "%%%d.%ds", ldate, ldate);

	//Вывод всего содержимого
	for(int i = 0, j = dir_print; i < height; i++, j++)
	{
		if(i < dir_count[index])	//Если есть что выводить
		{
			//Из-за проблем с КОДИРОВКОЙ пришлось выводить каждое поле отдельно :(
			mvwprintw(win[index], i, 0, fname, dir_content[index][j].name);
			mvwprintw(win[index], i, lname, fsize, dir_content[index][j].size);
			mvwprintw(win[index], i, lname + lsize, fdate, dir_content[index][j].cdate);
		}
		//Дорисовка разделительных линий столбцов
		mvwaddch(win[index], i, lname, ACS_VLINE);
		mvwaddch(win[index], i, lname + lsize, ACS_VLINE);
	}

	//Если обновляется активное окно, то выделяем выбраную директорию
	if(index == win_active)
	{
		wattron(win[index],COLOR_PAIR(1));
		//Закрашиваю фон всей строки опять же из за кодировки :(
		sprintf(fline, "%%%d.%ds", width, width);
		mvwprintw(win[index], dir_inwin, 0, fline, " ");

		//Вывожу поверх данные выделенной директории
		mvwprintw(win[index], dir_inwin, 0, fname, dir_content[index][dir_inwin + dir_print].name);
		mvwprintw(win[index], dir_inwin, lname, fsize, dir_content[index][dir_inwin + dir_print].size);
		mvwprintw(win[index], dir_inwin, lname + lsize, fdate, dir_content[index][dir_inwin + dir_print].cdate);
		
		//Дорисовка горизонтальных линий
		mvwaddch(win[index], dir_inwin, lname, ACS_VLINE);
		mvwaddch(win[index], dir_inwin, lname + lsize, ACS_VLINE);
		wattroff(win[index],COLOR_PAIR(1));
	}
	//Обновление экрана
	wrefresh(win[index]);
}

//Перерисовывает рамку
void winref_border(unsigned index)
{
	int width;						//Ширина окна
	int height; 					//Высота окна
	char format[32];				//Формат вывода заголовка
	unsigned lname, lsize, ldate;	//Длина каждого параметра

	//Очстка 
	wclear(win[index]);

	//Определение формата вывода заголовка
	getmaxyx(win[index], height, width);
	width = width - 2;
	lname = width * 0.5; 
	lsize = width * 0.2 - 2;
	ldate = width - lname - lsize - 5;
	sprintf(format, " %%-%d.%ds|%%%d.%ds |%%%d.%ds ", lname, lname, lsize, lsize, ldate, ldate);

	//Вывод заголовка
	wattron(win[index],COLOR_PAIR(3));
	mvwprintw(win[index], 1, 1, format, "File name", "Size", "Modify");
	wattroff(win[index],COLOR_PAIR(3));
	box(win[index], 0, 0); 

	//Выводим путь (занимает половину окна)
	lsize = strlen(dir_patch[index - 2]);
	if(lsize > width / 1.5)		//Если длина пути больше чем есть мета, то урезаем
		mvwprintw(win[index], 0, 2, "...%s", dir_patch[index - 2] + lsize - (int)(width / 1.5));
	else
		mvwprintw(win[index], 0, 2, "%s", dir_patch[index - 2]);
	wrefresh(win[index]);
}

//Перерисовывает окно для ввода парамеров
void winref_input(char *data, int cpindex)
{
	wclear(win[WIN_CF]);

	//Оформление
	box(win[WIN_CF], 0, 0);
	mvwprintw(win[WIN_CF] , 1, 6, "ENTER THE OPTIONS");
	mvwprintw(win[WIN_CF] , 5, 1, "[ESC - CANCEL]  [ENTER - OK]");
	wmove(win[WIN_CF], 3, 2);		//На случай если выводить нечего и не зайдет в цикл

	//Вывод с указанной позиции
	for(int i = 0, j = cpindex; i < 26 && data[j] !='\0'; i++, j++)
		mvwaddch(win[WIN_CF], 3, i + 2, data[j]);
	wrefresh(win[WIN_CF]);
}

//Инициализирует окно для ввода параметров
void win_createinput()
{
	win[WIN_CF] = newwin(7, 30, (LINES - 7) / 2, (COLS - 30) / 2);
	refresh();
	wattron(win[WIN_CF] ,COLOR_PAIR(3));	//Меняем цвет
	curs_set(1);							//Включаем курсор
}

//Удаляет окно для ввода параметров
void win_destroyinput()
{
	if(win[WIN_CF] != NULL)
		delwin(win[WIN_CF]);

	wattroff(win[WIN_CF] ,COLOR_PAIR(3));	//Отключаем цвет
	curs_set(0);							//Скрываем курсор
	winref_all();
}