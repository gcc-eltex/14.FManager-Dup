#include "../../header/text_editor/shared.h"

int read_content(char *fpath, char **cont)
{
	FILE *fd;
	unsigned fsize;

	fd = fopen(fpath, "r");

	if(fd == NULL)
		return ERR_OPEN;

	//Получаем размер файла
	fseek(fd, 0, SEEK_END);
	fsize = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	*cont = malloc(sizeof(char) * fsize + 1);
	fread(*cont, sizeof(char), fsize, fd);
	fclose(fd);
	(*cont)[fsize] = '\0';
	return fsize + 1;
}

int write_content(char *fpath, char *cont)
{
	FILE *fd;
	fd = fopen(fpath, "w");

	if(fd == NULL)
		return ERR_OPEN;

	fwrite(cont, sizeof(char), strlen(cont), fd);
	fclose(fd);
	return 0;
}
