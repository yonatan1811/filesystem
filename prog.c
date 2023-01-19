#include <stdio.h>
#include <stdlib.h>
#include "BlockData.h"

void menu();
void resName(char* input);
void myFgets(char* input, int len);

int main(void)
{
	userManage* defaultUser = createAUser("Yonatan", "123");
	



	FILE* readFile = NULL;
	FILE* writeFile = NULL;
	int saveOrLoad = 0;
	char filePath[TF] = { 0 };
	

	iNode* mainDirectory = createAFolder("C");
	iNode* folder1 = NULL;
	hdd hardDrive = Createhdd();
	restartHdd(&hardDrive);
	FM manageF = createFM();
	DBM manage = createDBM();
	iNode* file = NULL;
	userManage* currUser = NULL;
	userManage* firstUser = NULL;
	iNode* fileOrFolder = NULL;
	iNode* toReadFile = NULL;
	char userName[NAME_SIZE] = { 0 };
	int canReadAndWrite = 0;
	char passWord[NAME_SIZE] = { 0 };
	char input[20] = { 0 };
	char name[15] = { 0 };
	char path[INPUT_LEN] = { 0 };
	char folder[TF] = { 0 };
	int choice = 0;

	saveOrLoad = loadOrNew();

	if (saveOrLoad == 1)
	{
		strcpy(filePath, enterPath());
		readFile = fopen(filePath, "r");
		loadProject(readFile, mainDirectory, &hardDrive, &manage, defaultUser);
	}
	

	do
	{
		strcpy(userName, getUserName(userName));
		strcpy(passWord, getPassWord(passWord));

	} while (!isValidUser(defaultUser , userName , passWord));

	currUser = returnUser(defaultUser, userName, passWord);

	
	if (readFile != NULL)
	{
		fclose(readFile);
	}
	

	


	do
	{
		menu();
		scanf("%d", &choice);
		getchar();
		if (choice == 1)
		{
			printf("\nenter name:\n");
			myFgets(name , 10);
			printf("in which folder? ");
			myFgets(folder, 20);
			//to add a file to the folder we need the folder also	
			printf("Can other users read and write to this file? 1 - yes , 0 - no\n");
			scanf("%d", &canReadAndWrite);
			getchar();
			file = createFile(name , userName , canReadAndWrite);
			fileOrFolder = findFileOrFolder(folder, mainDirectory);
			addFileToFolder(fileOrFolder, file);
			resName(name);
			resName(folder);
		}
		else if (choice == 2)
		{
			printf("what do u want to write?\n ");
			myFgets(input, 10);

			printf("To which file to write? \n");
			myFgets(path, 20);
			file = findFileOrFolder(path, mainDirectory);
			if (file!= NULL)
			{
				if (file->canWriteAndRead ==1)
				{
					writeToFile(file, input, &hardDrive, &manage);
				}
				else
				{
					if (!(strcmp(currUser->userName , file->creator))) // just dont have a password to check to
					{
						writeToFile(file, input, &hardDrive, &manage);
					}
					else
					{
						printf("No premission!\n");
					}
				}
			}
			else
			{
				printf("This file couldnt be found\n");
			}
			
			resName(path);
			resName(input);
		}
		else if (choice == 3)
		{
			printf("Enter the path of the file");
			myFgets(path, 20);
			file = findFileOrFolder(path, mainDirectory);
			if (file!=NULL)
			{
				if (file->canWriteAndRead ==1)
				{
					readFileSystem(file);
				}
				else
				{
					if (!(strcmp(currUser->userName , file->creator)))
					{
						readFileSystem(file);
					}
					else
					{
						printf("No premission!\n");
					}
				}
				
			}
			resName(path);
		}
		else if (choice == 4)
		{
			printf("Enter the path of the folder");
			myFgets(path, 20);
			file = findFileOrFolder(path, mainDirectory);
			if (file!= NULL)
			{
				dir(file);
			}
			resName(path);
		}
		else if (choice == 5)
		{
			printf("Enter the name of the folder : \n");
			myFgets(name, 20);
			printf("in which folder? ");
			myFgets(folder, 20);
			folder1 = createAFolder(name);
			file = findFileOrFolder(folder, mainDirectory);
			addFileToFolder(file, folder1);
			resName(name);
			resName(folder);

		}
		else if (choice == 6)
		{
			printf("Enter the path of the file to delete :\n");
			fgets(path, 20, stdin);
			printf("Enter the folder its in :\n");
			fgets(folder, 10, stdin);
			folder[strcspn(folder, "\n")] = 0;
			path[strcspn(path, "\n")] = 0;
			file = findFileOrFolder(path , mainDirectory);
			fileOrFolder = findFileOrFolder(folder, mainDirectory);
			deleteFile(&hardDrive, &manage, file, fileOrFolder);
		}
		else if(choice == 7)
		{
			printf("Enter the name of the user : \n");
			fgets(userName, NAME_SIZE, stdin);
			userName[strcspn(userName, "\n")] = 0;
			printf("Enter the password of the user : \n");
			fgets(passWord, NAME_SIZE, stdin);
			passWord[strcspn(passWord, "\n")] = 0;

			currUser = createAUser(userName, passWord);
			addUser(defaultUser, currUser);
		}
		else if (choice == 8)
		{
			strcpy(path, enterPath());
			writeFile = fopen(path, "w+");
			SaveProject(writeFile, mainDirectory , defaultUser);
			fclose(writeFile);
		}

		resName(name);
	} while (choice != 0);

	

	resName(input);
	resName(name);
	resName(path);
	resName(folder);
	resName(filePath);
	
	freeingSystem(&manage, &manageF, &hardDrive);
	freeingTheMainDir(mainDirectory);
	freeingUsers(defaultUser);
	/*free(folder1);
	free(currUser);
	free(firstUser);
	free(fileOrFolder);
	free(toReadFile);*/


	printf("Leaks: %d", _CrtDumpMemoryLeaks());
	getchar();
	return 0;
}

void menu()
{
	printf("What do u want to do ?\n1- create a file\n2- write to file\n3- read a file\n4- dir\n5- create a folder\n6 - delete file\n7 - create a new user\n8 - saving a system\nchoose : ");
}


