#include "../../header/file_manager/shared.h"

unsigned dir_get(char *name, struct dline **content)
{
	DIR *dir;
	struct stat infodir;
	struct dirent *subdir;
	int i;

	//Очистка памяти, если была выделена
	if(*content != NULL)
		free(*content);

	//Cчитаем количество элементов в директории и выделяем память
	dir = opendir(name);
	subdir = readdir(dir);
	for(i = 1; subdir != NULL; subdir = readdir(dir), i++);
	*content = malloc(sizeof(struct dline) * i - 1);
	closedir(dir);

	//Получаем содержимое директории
	dir = opendir(name);
	subdir = readdir(dir);
	for(i = 1; subdir != NULL; subdir = readdir(dir))
	{
		stat(subdir->d_name, &infodir);
		//Если это ссылка на себя, то игнорируем
		if(!strcmp(subdir->d_name,"."))
			continue;
		//Если ссылка на родителя, то ставим в начало в нужном формате
		if(!strcmp(subdir->d_name,".."))
		{
			strcpy((*content)[0].name, subdir->d_name);
			strcpy((*content)[0].size, "-UP-");
			strftime((*content)[0].cdate, 32, "%d.%m %H:%M", gmtime(&(infodir.st_ctime)));
			(*content)[0].type = subdir->d_type;
		}
		//Иначе пишем попорядку
		else
		{
			strcpy((*content)[i].name, subdir->d_name);
			sprintf((*content)[i].size, "%d", (int)infodir.st_size);
			strftime((*content)[i].cdate, 32, "%d.%m %H:%M", gmtime(&(infodir.st_ctime)));
			(*content)[i].type = subdir->d_type;
			i++;
		}
	}
	closedir(dir);
	return i;
}
