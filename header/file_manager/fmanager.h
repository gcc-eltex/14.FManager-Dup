//Индексы окон
#define WIN_L 	0	//Левое окно
#define WIN_R 	1	//Правое окно
#define WIN_BL 	2	//Граница левого окна
#define WIN_BR 	3	//Граница правого окна
#define WIN_PB 	4	//Нижняя панель
#define WIN_CF  5	//Окно для ввода информации

struct dline			//Структура в которой хранится строковое представление информации о директории/файле, которая будет выводится
{
	char name[128];		//Имя файла/директории
	char cdate[32];		//Дата модификации
	char size[16];		//Размер
	unsigned type;		//Тип
};

//Глобальные переменные
WINDOW *win[5];					//Окна
char dir_patch[2][1024];		//Текущие дериктории в которых мы находимся
char te_path[1024];				//Путь к текстовому редактору

struct dline *dir_content[2];	//Содержимое текущих дерикторий
unsigned dir_count[2];			//Размер массива с содержимым директорий

unsigned win_active;			//Активное окно
unsigned dir_inwin;				//Индекс выбраного фали/папки в активном окне
unsigned dir_print;				//Индекс фала/папки в массиве с содержимым директории активного окна с которого начинается вывод

//window.c
void win_init();							//Инициальзирует все переменные и графику
void win_rerun();							//Инициальзирует только графику
void win_resize();							//Действие при изменении размера терминала
void win_destroy();							//Очистка памяти и отключение графики
void win_createinput();						//Инициализирует окно для ввода параметров
void win_destroyinput();					//Удаляет окно для ввода параметров

void winref_all();							//Перерисовывает все
void winref_panel(unsigned winindex);		//Перерисовывает панель
void winref_content(unsigned winindex);		//Перерисовывает окно c содержимым
void winref_border(unsigned winindex);		//Перерисовывает рамку
void winref_input(char *data, int cpindex);	//Перерисовывает окно для ввода парамеров

//action.c
void action_open();							//Открывает директорию/файл
void action_mvup();							//Перемещает курсор активной папки вверх
void action_mvdown();						//Перемещает курсор активной папки вниз
void action_winactive(unsigned winindex);	//Делает активным правое окно
char ***action_split(char *name_prog, char *data, unsigned *count_prog);
char ***action_runinput(char *name_prog, unsigned *count_prog);		//Обработкик нажатия клавиш в окне ввода. Получает и разделяет параметры

//fsystem.c
unsigned dir_get(char *name, struct dline **content);