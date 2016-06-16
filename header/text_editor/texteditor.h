//Коды ошибок
#define ERR_OPEN -1

//Индексы окон
#define WIN_C 	0	//Окно с содержимым файла (Область ввода и редакторивания)
#define WIN_BC	1	//Рамка вокруг содержимого
#define WIN_P 	2	//Нижняя панель


//Глобальные переменные
WINDOW *win[3];				//Окна
char patch[1024];			//Имя файла или путь к нему

char *content;			//Содержимое файла
unsigned cont_len;		//Количество символов в тексте
unsigned pindex;		//Индекс в массиве content с которого начинается вывод
unsigned cindex;		//Индекс в массиве content с соответствующий положению курсора

unsigned ccol;			//Столбец в тексте на котором находится курсор
unsigned cline;			//Строка в тексте на которой находится курсор

//window.c
void win_init(char *fpatch);			//Инициальзирует все переменные и графику
void win_resize();						//Действие при изменении размера терминала
void win_destroy();						//Очистка памяти и отключение графики

void winref_all();						//Перерисовка всего
void winref_panel(unsigned windex);		//Перерисовка панели
void winref_border(unsigned windex);	//Перерисовка рамки
void winref_content(unsigned index);	//Перерисовка контента


//fsystem.c
int read_content(char *fpath, char **cont);		//Считывает все содержимое в cont из файла fpath
int write_content(char *fpath, char *cont);		//Записывает содержимое cont в файл fpath

//action.c
void action_mvright();					//Перемещает курсор вправо
void action_mvleft();					//Перемещает курсор влево
void action_mvdown();					//Перемещает курсор вниз
void action_mvup();						//Перемещает курсор вверх
void action_insert(char ch);
void action_backspace();

int get_nextdown(int start, int current);
int get_nextup(int start);