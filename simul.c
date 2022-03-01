#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

int found = 0;

typedef struct Dir
{
	char *name;
	struct Dir *parent;
	struct File *head_children_files;
	struct Dir *head_children_dirs;
	struct Dir *next;
} Dir;

typedef struct File
{
	char *name;
	struct Dir *parent;
	struct File *next;
} File;

Dir *createDir(char *name)
{
	Dir *new_dir = (Dir *)malloc(sizeof(Dir));

	new_dir->name = strdup(name);
	new_dir->parent = NULL;
	new_dir->head_children_files = NULL;
	new_dir->head_children_dirs = NULL;
	new_dir->next = NULL;

	return new_dir;
}

File *createFile(char *name)
{
	File *new_file = (File *)malloc(sizeof(File));

	new_file->name = strdup(name);
	new_file->next = NULL;
	new_file->parent = NULL;

	return new_file;
}

void freeFile(File *f)
{
	free(f->name);
	free(f);
	return;
}

void freeDir(Dir *d)
{
	free(d->name);
	return;
}

void deleteNode(Dir **list, char *elem)
{
	Dir *curr = *list;
	Dir *prec = *list;

	if (curr != NULL && strcmp(curr->name, elem) == 0)
	{
		*list = curr->next;

		free(curr->name);
		free(curr);
		return;
	}

	while (curr != NULL && strcmp(curr->name, elem) != 0)
	{

		prec = curr;
		curr = curr->next;
	}

	if (curr == NULL)
		return; // daca nu a fost gasit elemntul

	prec->next = curr->next;
	free(curr->name);

	free(curr);
	return;
}

void deleteNodeFiles(File **list, char *elem)
{
	File *curr = *list;
	File *prec = *list;

	if (curr != NULL && strcmp(curr->name, elem) == 0)
	{
		*list = curr->next;

		free(curr->name);
		free(curr);
		return;
	}

	while (curr != NULL && strcmp(curr->name, elem) != 0)
	{

		prec = curr;
		curr = curr->next;
	}

	if (curr == NULL)
		return; // daca nu a fost gasit elemntul

	prec->next = curr->next;
	free(curr->name);
	free(curr);
	return;
}

void touch(Dir *parent, char *name)
{

	if (parent->head_children_files == NULL)
	{

		File *file = createFile(name);
		file->parent = parent;

		file->parent->head_children_files = file;
		file->next = NULL;
		return;
	}
	else
	{

		File *file = createFile(name);
		file->parent = parent;
		File *iter = file->parent->head_children_files;

		while (iter)
		{

			if (strcmp(iter->name, name) == 0)
			{
				printf("File already exists\n");
				free(file->name);
				free(file);
				return;
			}

			if (iter->next == NULL)
				break;

			iter = iter->next;
		}

		iter->next = file;
		file->next = NULL;
	}
	return;
}

void mkdir(Dir *parent, char *name)
{

	if (parent->head_children_dirs == NULL)
	{
		Dir *dir = createDir(name);
		dir->parent = parent;

		dir->parent->head_children_dirs = dir;
		dir->next = NULL;
	}
	else
	{
		Dir *dir = createDir(name);
		dir->parent = parent;

		Dir *iter = dir->parent->head_children_dirs;

		while (iter)
		{

			if (strcmp(iter->name, name) == 0)
			{
				printf("Directory already exists\n");

				free(dir->name);
				free(dir);

				return;
			}

			if (iter->next == NULL)
				break;

			iter = iter->next;
		}

		iter->next = dir;
		dir->next = NULL;
	}
	return;
}

void ls(Dir *parent)
{
	Dir *iterDir = NULL;
	File *iterFile = NULL;

	iterDir = parent->head_children_dirs;
	while (iterDir)
	{

		printf("%s\n", iterDir->name);
		iterDir = iterDir->next;
	}

	iterFile = parent->head_children_files;

	while (iterFile)
	{

		printf("%s\n", iterFile->name);
		iterFile = iterFile->next;
	}

	return;
}

void rm(Dir *parent, char *name)
{

	if (parent->head_children_files == NULL)
	{
		printf("Could not find the file\n");
		return;
	}

	File *iter = parent->head_children_files;
	File *prev = parent->head_children_files;

	if (iter == parent->head_children_files &&
		strcmp(iter->name, name) == 0) // daca e primul din lista
	{

		parent->head_children_files = iter->next;

		freeFile(iter);

		return;
	}

	while (iter && strcmp(iter->name, name))
	{

		prev = iter;
		iter = iter->next;
	}

	// daca nu am gasit niciun element cu numele dat
	if (iter == NULL)
	{
		printf("Could not find the file\n");
		return;
	}

	prev->next = iter->next;

	freeFile(iter);
	return;
}

long length(Dir *list)
{

	Dir *currentNode;
	currentNode = list;

	int count = 0;
	while (currentNode != NULL)
	{
		count++;
		currentNode = currentNode->next;
	};
	return count;
}

void rmdir(Dir *parent, char *name)
{

	if (parent == NULL)
		return;

	if (strcmp(parent->name, "home") == 0 && parent->head_children_dirs == NULL)
	{
		printf("Could not find the dir\n");
		return;
	}

	if (strcmp(parent->name, name) == 0)
	{
		found = 1;
		Dir *iterMainList = parent->head_children_dirs;
		Dir *auxDir;
		while (iterMainList)
		{
			auxDir = iterMainList->next;
			rmdir(iterMainList, iterMainList->name);
			iterMainList = auxDir;
		}

		File *iterFile = parent->head_children_files;
		File *aux2;
		while (iterFile) // sterg files
		{
			aux2 = iterFile->next;
			free(iterFile->name);
			free(iterFile);
			iterFile = aux2;
		}

		deleteNode(&parent->parent->head_children_dirs, parent->name);
		return;
	}

	Dir *iter = parent->head_children_dirs;
	Dir *auxIter;
	while (iter)
	{

		auxIter = iter->next;
		rmdir(iter, name);
		iter = auxIter;
	}

	if (found == 0 && parent->next == NULL && parent->parent != NULL)
		printf("Could not find the dir\n");

	return;
}

void cd(Dir **target, char *name)
{

	if (strcmp(name, "..") == 0)
	{
		if ((*target)->parent == NULL)
			return;
		(*target) = (*target)->parent;
		return;
	}
	else
	{
		Dir *iter = (*target)->head_children_dirs;

		while (iter && strcmp(name, iter->name))
		{
			iter = iter->next;
		}

		if (iter == NULL)
		{
			printf("No directories found!\n");
			return;
		}

		(*target) = iter;
		return;
	}
	return;
}

char *pwd(Dir *target)
{
	Dir *iter = target;
	char *path = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));
	char *aux = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));

	sprintf(path, "/%s", iter->name);
	strcat(aux, path);

	iter = iter->parent;
	while (iter)
	{
		sprintf(aux, "/%s", iter->name);
		strcat(aux, path);
		strcpy(path, aux);
		iter = iter->parent;
	}
	free(path);
	return aux;
}

void stop(Dir *target)
{
	while (strcmp(target->name, "home") != 0)
	{
		cd(&target, "..");
	}

	Dir *iter = target->head_children_dirs;

	while (iter)
	{
		Dir *aux = iter->next;
		rmdir(iter, iter->name);
		iter = aux;
	}

	File *iterFile = target->head_children_files;
	File *aux2;
	while (iterFile) // sterg files
	{
		aux2 = iterFile->next;
		free(iterFile->name);
		free(iterFile);
		iterFile = aux2;
	}
	free(target->name);
	free(target);
	return;
}

void tree(Dir *target, int level)
{
	if (target == NULL)
		return;

	if (strcmp(target->name, "home") != 0)
	{
		int aux = level;
		while (aux > 0)
		{
			printf("    ");
			aux--;
		}
		printf("%s\n", target->name);
	}

	tree(target->head_children_dirs, level + 1);

	Dir *iterDir = target;
	if (target)
	{
		File *iterFile = target->head_children_files;
		while (iterFile)
		{
			int aux = level;
			while (aux >= 0)
			{
				printf("    ");
				aux--;
			}

			printf("%s\n", iterFile->name);
			iterFile = iterFile->next;
		}
	}

	return;
}

void mv(Dir *parent, char *oldname, char *newname)
{
	Dir *iterDir = parent->head_children_dirs;
	Dir *foundDir = NULL;
	File *iterFile = parent->head_children_files;
	File *foundFile = NULL;
	int found_dir = 0, found_file = 0;

	while (iterDir)
	{
		if (strcmp(iterDir->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		if (strcmp(iterDir->name, oldname) == 0)
			foundDir = iterDir;
		iterDir = iterDir->next;
	}

	while (iterFile)
	{
		if (strcmp(iterFile->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		if (strcmp(iterFile->name, oldname) == 0)
			foundFile = iterFile;
		iterFile = iterFile->next;
	}

	if (foundDir && foundFile == NULL)
	{

		Dir *newDir = createDir(newname);
		newDir->head_children_dirs = foundDir->head_children_dirs;
		newDir->head_children_files = foundDir->head_children_files;
		newDir->parent = foundDir->parent;
		Dir *iterDir2 = parent->head_children_dirs;

		while (iterDir2->next)
		{
			iterDir2 = iterDir2->next;
		}
		iterDir2->next = newDir;
		newDir->next = NULL;

		deleteNode(&parent->head_children_dirs, foundDir->name);
		return;
	}
	else if (foundFile == NULL)
	{
		printf("File/Director not found\n");
		return;
	}

	if (foundFile && foundDir == NULL)
	{

		File *newFile = createFile(newname);

		newFile->parent = foundFile->parent;

		File *iterFile2 = parent->head_children_files;

		while (iterFile2->next)
		{
			iterFile2 = iterFile2->next;
		}
		iterFile2->next = newFile;
		newFile->next = NULL;

		deleteNodeFiles(&parent->head_children_files, foundFile->name);
		return;
	}

	else if (foundDir == NULL)
	{
		printf("File/Director not found\n");
		return;
	}

	return;
}

int main(int argc, char *argv[])
{
	FILE *f1 = fopen("input2", "r");

	char *comm = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));
	char *arg = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));
	char *arg2 = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));

	Dir *home = createDir("home");

	do
	{
		/*
	Summary:
			Reads from stdin a string and breaks it down into command and in
			case it needs into a name.
	*/

		if (strcmp(comm, "touch") == 0)
		{
			fscanf(f1, "%s", arg);

			touch(home, arg);
		}
		if (strcmp(comm, "mkdir") == 0)
		{
			fscanf(f1, "%s", arg);

			mkdir(home, arg);
		}
		if (strcmp(comm, "ls") == 0)
		{
			ls(home);
		}
		if (strcmp(comm, "rm") == 0)
		{
			fscanf(f1, "%s", arg);

			rm(home, arg);
		}
		if (strcmp(comm, "rmdir") == 0)
		{

			fscanf(f1, "%s", arg);
			rmdir(home, arg);

			if (found == 1)
				found = 0;
		}
		if (strcmp(comm, "cd") == 0)
		{
			fscanf(f1, "%s", arg);

			cd((&home), arg);
		}
		if (strcmp(comm, "tree") == 0)
		{
			tree(home, -1);
		}
		if (strcmp(comm, "pwd") == 0)
		{
			char *outputPath;
			outputPath = pwd(home);
			printf("%s\n", outputPath);
			free(outputPath);
		}
		if (strcmp(comm, "mv") == 0)
		{
			fscanf(f1, "%s %s", arg, arg2);
			mv(home, arg, arg2);
		}
		if (strcmp(comm, "stop") == 0)
		{
			stop(home);
			break;
		}

	} while (fscanf(f1, "%s", comm) != EOF);

	free(comm);
	free(arg);
	free(arg2);
	fclose(f1);
	return 0;
}