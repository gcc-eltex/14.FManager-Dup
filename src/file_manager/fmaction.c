#include "../../header/file_manager/shared.h"

//Делает активным правое окно
void action_winactive(unsigned index)
{
	//Если окно итак активно или не удалось перейти в директорию
	if(index == win_active || chdir(dir_patch[index]) == -1)
		return;

	//Смеа директории и сброс значений
	dir_print = 0;
	dir_inwin = 0;
	win_active = index;

	winref_all();
}

//Перемещает курсор активной папки вверх
void action_mvup()
{
	//Если дошли до начала директории
	if(dir_print + dir_inwin <= 0)
		return;
	//Если дошли до начала окна
	if(dir_inwin <= 0)
		dir_print--;
	else
		dir_inwin--;
	winref_content(win_active);
}

//Перемещает курсор активной папки вниз
void action_mvdown()
{
	int width;						//Ширина окна
	int height; 					//Высота окна

	getmaxyx(win[win_active], height, width);
	//Если пролистывать нечего
	if(dir_print + dir_inwin >= dir_count[win_active] - 1)
		return;
	//Если дошли до конца окна
	if(dir_inwin >= height - 1)
		dir_print++;
	else
		dir_inwin++;
	winref_content(win_active);
}

char ***action_split(char *name_prog, char *data, unsigned *count_prog)
{

	char ***run;
	char *ptr;
	char data_prog[16][255];
	int cparam;

	//Разбиваем, подсчитывая количество программ
	ptr = strtok(data, "|");
	for(*count_prog = 0; ptr != NULL; (*count_prog)++)
	{
		strcpy(data_prog[(*count_prog)], ptr);
		ptr = strtok(NULL, "|");
	}

	//На случай если ничего не передано
	if(strlen(data) == 0)
	{
		strcpy(data_prog[0], data);
		(*count_prog) = 1;
	}
	
	//Разбиваем для каждо запускаемой программы
	run = malloc((*count_prog) * sizeof(char **));		//Выделяем память под параметры нужного количества запускаемых программ
	for(int i = 0; i < (*count_prog); i++)
	{
		run[i] = malloc(16 * sizeof(char *)); 			//Выделяем память под максимальное количество параметров 16
		for(int j = 0; j < 16; j++)	
			run[i][j] = malloc(255 * sizeof(char));		//Выделяем память под максимальную длину параметра
		
		//Записываем параметры
		ptr = strtok(data_prog[i], " ");
		for(cparam = 0; ptr != NULL; cparam++)
		{
			if(i == 0)
				strcpy(run[i][cparam + 1], ptr);
			else
				strcpy(run[i][cparam], ptr);
			ptr = strtok(NULL, " ");
		}

		if(i == 0)
			run[i][cparam + 1] = NULL;
		else
			run[i][cparam] = NULL;
	}
	strcpy(run[0][0], name_prog);

	return run;
	//Вписываем сначала имя первого исполняемого файла, который выбрали
}

//Обработкик нажатия клавиш в окне ввода. Получает и разделяет параметры
char ***action_runinput(char *name_prog, unsigned *count_prog)
{
	int key = 0;				//Код нажатой клавиши
	char *ptr;					//Указатель используемый для strtok
	char input[255];			//Храних введенные/отображаемые данные
	unsigned cpindex = 1;		//Индекс символа в input с которого начинается вывод 
	unsigned data_len = 1;		//Длина массива input
	unsigned count_param = 0;	//Количество введенных параметров

	//Инициализируем данные и окно
	input[0] = ' ';
	input[1] = '\0';
	win_createinput();
	winref_input(input, cpindex);

	while((key = getch()) != '\n')
	{
		switch(key)
		{
			break;
			case 27:
				win_destroyinput();
				return NULL;
			break;
			case KEY_LEFT:
				if(cpindex > 1)
					cpindex--;
			break; 
			case KEY_RIGHT:
				if(cpindex < data_len)
					cpindex++;
			break;
			case KEY_BACKSPACE:
				if(data_len > 1)
				{
					data_len--;
					input[data_len] = '\0';
					if(cpindex > 1)
						cpindex--;	
				}
			break;
			default:
				if(data_len > 250)
					continue;
				input[data_len] = (char)key;
				input[data_len + 1] = '\0';
				data_len++;
				if(data_len > 26)
					cpindex++;
			break;
		}
		winref_input(input, cpindex);
	}

	//Парсим введенную строку
	return action_split(name_prog, input, count_prog);
}

//Открывает директорию/файл
void action_open()
{
	struct dline newdir;	//Структура с инфорацией об открываемом обьекте
	char path[1024];		//Полный петь к новому обьекту

	//К текущей директории прибавляем имя директории в которую переходим
	newdir = dir_content[win_active][dir_print + dir_inwin];
	sprintf(path, "%s/%s", dir_patch[win_active], newdir.name);

	//Если это папка, то переходим в нее
	if(newdir.type == DT_DIR)
	{
		//Изменение рабочей директории
		if(chdir(path) == -1)
			return;
		getcwd(dir_patch[win_active], 1024);
		
		//Получение ее содержимого и вывод
		dir_count[win_active] = dir_get(path, &(dir_content[win_active]));
		dir_print = dir_inwin = 0;
		winref_all();
	}
	else	//Если это файл
	{
		struct stat st;		//Структура с информацией о файле
		pid_t pid;			//id нового процесса
		char ***params;		//Массив парамеров для exec
		int status = 0;		//Для wait
		int count_prog;		//Количество запускаемых процессов(программ)
		int **fd;			//Пайпы
		char ch;

		char bbb[1024];
		stat(newdir.name, &st);

		//Если файл может быть запущен, пытаемся это сделать
		if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH)
		{
			//Открываем окно для ввода параметров запуска и получаем распарсенную строку
			params = action_runinput(newdir.name, &count_prog);

			//Если пользователь отменил запуск
			if(params == NULL)
			{
				winref_all();
				return;
			}

			//Отключаем графику и выделяем память под пайпы
			win_destroy();
			fd = malloc(count_prog * sizeof(int *));
			for(int i = 0; i < count_prog - 1; i++)
			{
				fd[i] = malloc(2 * sizeof(int));
				pipe(fd[i]);
			}

			//Если запускается единственная программа
			if( count_prog == 1)
			{
				pid = fork();
				if(pid == 0)
				{

					execv(params[0][0], params[0]);
					printf("Error start for program:%s\n", params[0][0]);
					_exit(EXIT_FAILURE);
				}
				wait(&status);
				count_prog = 0;
			}

			//Если больше одной, то поочередно загружаем каждую программу подменяя стандартные потоки пайпами
			for(int i = 0; i < count_prog; i++)
			{
				pid = fork();
				if(pid == 0)
				{
					if((i % 2) == 0)				//Если первый процесс, перенаправляем только вывод
						dup2(fd[i][1], 1);
					else if(i == count_prog - 1)	//Если последний, перенаправляем только ввод
						dup2(fd[i - 1][0], 0);
					else 							//Иначе и ввод и вывод
					{
						dup2(fd[i][1], 1);
						dup2(fd[i - 1][0], 0);
					}
					execv(params[i][0], params[i]);
					printf("Error start for program:%s\n", params[i][0]);
					_exit(EXIT_FAILURE);
				}
				waitpid(pid, &status, 0);
			}

			//Очищаем память и зыкрываем пайпы
			for(int i = 0; i < count_prog - 1; i++)
			{
				close(fd[i][0]);
				close(fd[i][1]);
				free(fd[i]);
			}
			free(fd);
			for(int i = 0; i < count_prog; i++)
			{
				for(int j = 0; j < 16; j++)
					free(params[i][j]);
				free(params[i]);
			}
			free(params);


			//Просим что-нибудь ввести, чтобы увидеть результат работы программ
			printf("\nThe program is completed. Type any character to continue:");
			scanf("%c%*[^\n]", &ch);
		}
		//Если это текстовый файл
		else
		{
			win_destroy();
			if((pid = fork()) == 0)
			{
				execl( te_path, "text_editor", path, (char*)0 );
				_exit(EXIT_FAILURE);
			}
			waitpid(pid, &status, 0);
		}
		win_rerun();
	}
}