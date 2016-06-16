#include "../../header/text_editor/shared.h"

//Перемещает курсор вправо
void action_mvright()
{
	//Условие конца файла
	if(content[cindex] == '\n' || content[cindex] == '\0' || ccol >= COLS - 4)
		return;

	if(content[cindex] == '\t')
		ccol += 8/*8 - ccol % 8*/;
	else
		ccol++;

	cindex++;
	/*mvwprintw(win[WIN_C], cline, ccol, "");
	wrefresh(win[WIN_C]);*/
	winref_content(WIN_C);
}

//Перемещает курсор влево
void action_mvleft()
{
	//Условие начала файла
	if(cindex == 0 || ccol <= 0)
		return;

	cindex--;
		if(content[cindex] == '\t')
			ccol -= 8/*(ccol - 1) % 8 + 1*/;
		else 
			ccol--;

	/*mvwprintw(win[WIN_C], cline, ccol, "");
	wrefresh(win[WIN_C]);*/
	winref_content(WIN_C);
}

//Перемещает курсов вверх
void action_mvup()
{
	/*if(cline > 0 )
		cline--;
	ccol = 0;


	winref_content(WIN_C);*/
}

//Перемещает курсор вниз
void action_mvdown()
{
	int new_index;

	new_index = get_nextdown(cindex, ccol);
	if(new_index == cindex)
		return;
	cindex = new_index;
	ccol = 0;
	if(cline < LINES - 7)
		cline++;
	else
		pindex = get_nextdown(pindex, 0);

	winref_content(WIN_C);
}


int get_nextdown(int start, int current)
{
	int indpos = start;		//Позиция в файле
	int indcol = current;	//Позиция курсора на окне

	for(; content[indpos] != '\0'; indpos++, indcol++)
		if(content[indpos] == '\n' || indcol >= COLS - 4)
			return indpos + 1;
	return start;
}

int get_nextup(int start)
{
	;
}

void action_insert(char ch)
{
	char *buffer = NULL;

	//Выделяем память на 1 элемент больше
	cont_len++;
	buffer = malloc(sizeof(char) * cont_len);
	//Вставляем символ
	strncpy( buffer, content, cindex);
	buffer[cindex] = ch;
	for(int i = cindex; i < cont_len - 1; i++ )
		buffer[i + 1] = content[i];

	//Освобождаем память из под старых данных
	free(content);
	content = buffer;

	//Смещаем курсор
	if(ccol < COLS - 4)
		action_mvright();
	else
		action_mvdown();
}

void action_backspace()
{
	cont_len--;
	//Если курсов в сама начале и стирать нечего
	if(cindex == 0)
		return;

	//Смещаем курсор
	if(ccol > 0)
		action_mvleft();
	else
		action_mvup();

	//Копируем со смещением на 1 символ(чтобы его удалить)
	for(int i = cindex; i < cont_len ; i++)
		content[i] = content[i + 1];

	winref_content(WIN_C);
}
