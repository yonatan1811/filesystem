#include "BlockData.h"



/*
* this function is used to update the hardDrive either when we used a new block or we deleted a block
* input: hardDrive, and a the block we used.
* no output
*/
void updateHardDrive(hdd* hardDrive, Block* block)
{

	hardDrive->totalBlocksUsed++;
	hardDrive->freeBlocks = AMOUNT_OF_BLOCKS - hardDrive->totalBlocksUsed;
	hardDrive->blocks[hardDrive->totalBlocksUsed - 1] = block;
	restartingData(block);
}




/*
* when im using a new name we want to restart it and put 0 at each cell
*/
void restartName(iNode* file)
{
	for (int i = 0; i < NAME_SIZE; i++)
	{
		*(file->name + i) = 0;
	}
}


//this will create a file without data, wont have next
iNode* createFile(char* name, char* creator, int canReadAndWrite)
{
	iNode* newNode = (iNode*)malloc(sizeof(iNode));

	restartName(newNode);
	newNode->iBlock = NULL;
	newNode->fileNext = NULL;
	newNode->folderNext = NULL;
	newNode->isFolder = 0;
	strcpy(newNode->creator, creator);
	strncpy(newNode->name, name, strlen(name));
	newNode->canWriteAndRead = canReadAndWrite;
	return newNode;
}



//will create a block
Block* CreateBlock(int blockNum)
{
	Block* newBlock = (Block*)malloc(1 * sizeof(Block));
	newBlock->blockNumber = blockNum;
	newBlock->currIndex = 0;
	newBlock->blockFullUsed = 0;
	newBlock->next = NULL;
	return newBlock;
}







//with this function you can check and get the length of the file
int fileLen(iNode* curr)
{
	int size = 0;
	Block* firstBlock = curr->iBlock;
	Block* temp = firstBlock;
	while (temp)
	{
		size += DATA_SIZE;
		temp = temp->next;
	}

	return size;
}

//with this function u can read a file
void readFileSystem(iNode* file)
{
	Block* tempBlock;
	if (file->iBlock == NULL)
	{
		tempBlock = NULL;
	}
	else
		tempBlock = file->iBlock;
	while (tempBlock)
	{
		for (int i = 0; i < DATA_SIZE; i++)
		{
			if (tempBlock->data[i] != 0)
				printf("%c", tempBlock->data[i]);
		}
		printf("\n");
		tempBlock = tempBlock->next;
	}
}





//with this function we will write to a file
void writeToFile(iNode* file, char* input, hdd* hardDrive, DBM* manage)
{
	hdd hard = *(hardDrive);
	int i = 0;
	int j = 0;
	Block* curr = NULL;

	bool blockUsed = true;
	int saveIndex = 0;
	Block* temp = NULL;
	int currIndex = 0;
	int inputLen = strlen(input);
	float floatBlock = inputLen / (float)DATA_SIZE;
	int blocksNeeded = ceil(floatBlock);
	int index = 0;
	int length = 0;

	if (file->iBlock != NULL)
	{
		j = file->iBlock->currIndex;
		curr = file->iBlock;
		do
		{
			if (curr->blockFullUsed == 0)
			{
				blockUsed = false;
			}
			else
			{
				blockUsed = true;
			}
			if (curr->next != NULL)
			{
				curr = curr->next;
			}

		} while (curr->next);

	}


	if (blockUsed == 1 && curr && curr->next == NULL)
	{
		while (blocksNeeded > 0)
		{
			if (strlen(input) - index < DATA_SIZE + 1)
			{
				length = strlen(input) - index;
			}
			else
			{
				length = DATA_SIZE;
			}

			Block* temp = CreateBlock(hard.totalBlocksUsed);
			updateHardDrive(&hard, temp);

			for (i = 0; i < length; i++, index++)
			{
				temp->data[i] = input[index];
			}
			curr->next = temp;
			curr = curr->next;
			blocksNeeded--;
		}
		curr->currIndex = i;
	}
	else if (blockUsed == 0)
	{
		if (strlen(input) < (DATA_SIZE + 1) - curr->currIndex)
		{
			length = strlen(input);
			blocksNeeded = 0;
		}
		else
		{
			length = DATA_SIZE - curr->currIndex;
		}

		for (i = curr->currIndex; i < length + curr->currIndex; i++, index++)
		{
			curr->data[i] = input[index];
		}

		if (index % DATA_SIZE == 0)
		{
			currIndex = 9;
		}
		else
		{
			currIndex = index % DATA_SIZE;
		}
		curr->currIndex = currIndex;
		floatBlock = (strlen(input) - index) / (float)DATA_SIZE;
		blocksNeeded = (int)ceil(floatBlock);


		while (blocksNeeded > 0)
		{
			if (strlen(input) - index < DATA_SIZE + 1)
			{
				length = strlen(input) - index;
			}
			else
			{
				length = DATA_SIZE;
			}

			if (manage->freeDeletedBlocks > 0)
			{
				printf("we used a deleted block!");
				temp = manage->deletedBlocks[manage->freeDeletedBlocks - 1];
				updateDeletedBlocks(manage);
			}
			else
			{
				temp = CreateBlock(hard.totalBlocksUsed);
			}

			updateHardDrive(hardDrive, temp);

			for (i = 0; i < length; i++, index++)
			{
				temp->data[i] = input[index];
			}
			curr->next = temp;
			curr = curr->next;
			blocksNeeded--;
		}
		curr->currIndex = i;
	}
	else if (file->iBlock == NULL)
	{
		if (strlen(input) - index < DATA_SIZE + 1)
		{
			length = strlen(input) - index;
		}
		else
		{
			length = DATA_SIZE;
		}
		while (blocksNeeded > 0)
		{
			if (manage->freeDeletedBlocks > 0)
			{
				printf("we used a deleted block!");
				temp = manage->deletedBlocks[manage->freeDeletedBlocks - 1];
				updateDeletedBlocks(manage);
			}
			else
			{
				temp = CreateBlock(hardDrive->totalBlocksUsed);
				updateHardDrive(hardDrive, temp);
			}
			for (i = 0; i < length; i++, index++)
			{
				temp->data[i] = input[index];
			}
			temp->currIndex = i;
			if (file->iBlock == NULL)
			{
				file->iBlock = temp;
				curr = file->iBlock;
			}
			else
			{
				curr->next = temp;
				curr = curr->next;
			}
			blocksNeeded--;



		}
	}
	updateBlockIfUsed(file);

}


//we will create a harddrive
hdd Createhdd()
{
	hdd hardDrive;
	hardDrive.freeBlocks = AMOUNT_OF_BLOCKS;
	hardDrive.totalBlocks = AMOUNT_OF_BLOCKS;
	hardDrive.totalBlocksUsed = 0;
	return hardDrive;
}

//this function will get the field of the data and will put 0 in each cell
void restartingData(Block* block)
{
	for (int i = 0; i < DATA_SIZE; i++)
	{
		block->data[i] = 0;
	}
}


void updateBlockIfUsed(iNode* file)
{
	Block* temp = file->iBlock;
	while (temp)
	{
		if (temp->currIndex == DATA_SIZE - 1)
			temp->blockFullUsed = 1;
		temp = temp->next;
	}
}

//this file role is to "delete files" and put the blocks of the file into an array of deleted blocks
void deleteFile(hdd* hardDrive, DBM* w, iNode* file, iNode* folder)
{
	Block* currBlock = file->iBlock;
	int i = 0;
	iNode* curr = folder->fileNext;
	iNode* beforeCurr = folder;
	bool finish = false;
	while (currBlock)
	{
		restartingData(currBlock);
		currBlock->currIndex = 0;
		currBlock->blockFullUsed = 0;
		w->deletedBlocks[w->freeDeletedBlocks] = currBlock;
		w->freeDeletedBlocks++;
		hardDrive->freeBlocks++;
		hardDrive->totalBlocksUsed--;



		currBlock = currBlock->next;


	}
	while (curr&& finish != true)
	{
		if (!(strcmp(curr->name, file->name)))
		{
			finish = true;
			if (i > 0)
			{
				beforeCurr->folderNext = curr->folderNext;
			}
			else
			{
				beforeCurr->fileNext = curr->folderNext;

			}
		}
		else
		{
			beforeCurr = curr;
			curr = curr->folderNext;
		}
		i++;
	}
	file = NULL;



}


void updateDeletedBlocks(DBM* manage)
{
	manage->deletedBlocks[manage->freeDeletedBlocks] = NULL;
	manage->freeDeletedBlocks--;
}


DBM createDBM()
{
	DBM manage;
	manage.freeDeletedBlocks = 0;
	for (int i = 0; i < 10; i++)
	{
		manage.deletedBlocks[i] = 0;
	}
	return manage;
}

FM createFM()
{
	FM manage;
	manage.totalFiles = 0;
	for (int i = 0; i < TF; i++)
	{
		manage.filesArr[i] = 0;
	}
	return manage;
}

void updateFM(FM* manage, iNode* file)
{
	manage->totalFiles++;
	manage->filesArr[manage->totalFiles - 1] = file;
}

//this function will create a folder
iNode* createAFolder(char* name)
{
	iNode* folder = (iNode*)malloc(sizeof(iNode));
	folder->fileNext = NULL;
	folder->folderNext = NULL;
	folder->isFolder = 1;
	strcpy(folder->name, name);
	return folder;
}

//when we add a file to a folder we need to put the folder at the next field
void addFileToFolder(iNode* folder, iNode* file)
{
	iNode* curr = folder;
	iNode* temp;
	if (folder->isFolder == 1)
	{
		if (folder->fileNext == NULL)
		{
			folder->fileNext = file;
		}
		else
		{
			temp = folder->fileNext;
			while (temp->folderNext)
			{
				temp = temp->folderNext;
			}
			temp->folderNext = file;
		}
	}
	else
	{
		if (folder->folderNext == NULL)
		{
			folder->folderNext = file;
		}
		else
		{
			temp = folder->folderNext;
			while (temp->folderNext)
			{
				temp = temp->folderNext;
			}
			temp->folderNext = file;
		}
	}
	
}



//this function will print out all the folders and files the folder has
void dir(iNode* folder)
{
	iNode* curr = folder->fileNext;
	printf("folder's name : %s contains : \n", folder->name);
	while (curr)
	{
		if (curr->isFolder != 1)
		{
			printf("file's name : %s\n", curr->name);
		}
		else
		{
			printf("folder's name : %s\n", curr->name);
		}
		curr = curr->folderNext;
	}
}

//this function will get input like : c:\custom\i and then we will get to this file and read it , and if its a folder we print that it has been found
iNode* findFileOrFolder(char* input, iNode* mainDir)
{
	int j = 0;
	bool finish = false;
	iNode* currFileOrFolder = mainDir;
	int length = 0;
	int fullLength = 0;
	char* currName = (char*)malloc(20); // we will have 20 letters for the name
	currName = getCurrName(input, 0);
	length = strlen(currName)+1;
	while (currFileOrFolder && !finish)
	{
		if (!(strcmp(currName , currFileOrFolder->name)))
		{
			currName = getCurrName(input, length);
			if (currName != NULL)
			{
				length += strlen(currName) + 1;
				fullLength = strlen(currName);
			}
			if (currName == 0)
			{
				finish = true;
			}
			else if (currFileOrFolder->isFolder == 1)
			{
				if (currFileOrFolder->fileNext != NULL)
				{
					currFileOrFolder = currFileOrFolder->fileNext;
				}
				else
				{
					finish = true;
				}
			}
			
		}
		else
		{
			currFileOrFolder = currFileOrFolder->folderNext;
		}

		
	}
	if (currFileOrFolder != NULL)
	{
		if (currFileOrFolder->isFolder == 1)
		{
			return currFileOrFolder;
		}
		else
		{
			return currFileOrFolder;
		}
	}
	else
	{
		printf("File or folder not found, sorry");
		return NULL;
	}
	printf("\n");

}

char* getCurrName(char* input , int i )
{
	char currName[20] = { 0 };
	int j = 0;
	if (!(i >= strlen(input)))
	{
		while (*(input + i) != '%' && *(input + i) != 0)
		{
			currName[j] = *(input + i);
			i++;
			j++;
		}
		return currName;
	}
	return 0;
	
}


//just getting the name from the user
char* getUserName(char name[])
{
	printf("Please enter your username : \n");
	fgets(name, NAME_SIZE, stdin);
	name[strcspn(name, "\n")] = 0;
	return name;

}

//just getting the password from the user
char* getPassWord(char passWord[])
{
	printf("Please enter your password : \n");
	fgets(passWord, NAME_SIZE, stdin);
	passWord[strcspn(passWord, "\n")] = 0;
	return passWord;
}


//checking if the password and user name is acctually are correct and valid
bool ValidUser(char* userName, char* passWord, char* constName, char* constPassWord)
{
	if (!(strcmp(userName , constName)) && !(strcmp(passWord , constPassWord)))
	{
		return true;
	}
	return false;
}



void addUser(userManage* firstUser, userManage* currUser)
{
	userManage* temp = firstUser;
	if (firstUser == NULL)
	{
		firstUser = currUser;
	}
	else
	{
		while (temp->next)
		{
			temp = temp->next;
		}
		temp->next = currUser;
	}
}

int loadOrNew()
{
	int num = 0;
	printf("Do u want to load a file system or do you want to start a new one? \n");
	printf("1- yes\n2 - no\n");
	scanf("%d", &num);
	getchar();
	return num;
}

char* enterPath()
{
	char path[TF] = { 0 };
	printf("Please enter the path of the file to load/save from:\n");
	fgets(path, TF, stdin);
	path[strcspn(path, "\n")] = 0;
	for (int i = 0; i < strlen(path); i++)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}
	return path;
}



/*
* this function will be able to save a project using different symbols to present different data saving stuff
* input: the file we want to save in , and the maindir, the C directory
* output : none
*/
void SaveProject(FILE* file, iNode* mainDir, userManage* defualt)
{
	//this useful string will help us writing to the file the path of the file
	char path[NAME_SIZE] = { 0 };
	iNode* temp = mainDir;
	strcpy(path, temp->name);
	SaveFolder(file, temp, path);

	SaveUsers(defualt->next, file);
	

	resName(path);
}


//this function will help us saving multiply folders in a folder for example 
void SaveFolder(FILE* file, iNode* folder , char* folderPath)
{
	//the first file of the folder
	iNode* temp = NULL;
	if (folder->fileNext != NULL)
	{
		temp = folder->fileNext;
	}
	

	//simple string which helps us writing
	char path[NAME_SIZE] = { 0 };
	int len = 0;
	
	//copying the path of the folder to the string
	strncpy(path, folderPath, strlen(folderPath));

	//checking the len!! very important
	path[strlen(path)] = 0;
	strcat(path, "%");
	len = strlen(path);
	char data[TF] = { 0 };
	while (temp)
	{
		strcat(path, temp->name);
		if (temp->isFolder != 1)
		{
			strcpy(data, readingDataAndSave(temp));
			data[strcspn(data, "\n")] = 0;
			fprintf(file, "$%s$&%s&{%s{}%s}-%d-\n" , temp->name , path , temp->creator , data , temp->canWriteAndRead);
		}
		else
		{
			fprintf(file, "^%s^&%s&\n", temp->name, path);
			if (temp->fileNext !=NULL)
			{
				SaveFolder(file, temp, path);
			}
		}
		temp = temp->folderNext;
		
		path[len] = 0;
	}
	resName(path);
	resName(data);
}

//this function will return all of the data the file has
char* readingDataAndSave(iNode* file)
{
	char data[100] = { 0 };
	Block* currBlock = file->iBlock;
	while (currBlock)
	{
		strcat(data, currBlock->data);
		currBlock = currBlock->next;
	}
	return data;
}

//this function will open a file and get the data from there and will create all the files and folders.
void loadProject(FILE* readFile, iNode* mainDir, hdd* hardDrive, DBM* manage, userManage* defaultUser)
{
	char ch = 0;
	char name[TF] = { 0 };
	iNode* folder = NULL;
	iNode* file = NULL;	
	iNode* folderOrfile = NULL;	
	char path[TF] = { 0 };
	char creator[TF] = { 0 };
	int canReadAndWrite = 0;
	char passWord[15] = { 0 };
	int len = 0;
	char pathToAdd[TF] = { 0 };
	char data[TF] = { 0 };
	int i = 0;
	int j = 0;
	int isFolder = 2;
	while (ch != EOF)
	{
		ch = fgetc(readFile);
		if (ch == '^')
		{
			ch = fgetc(readFile);
			while (ch != '^')
			{
				name[i] = ch;
				i++;
				ch = fgetc(readFile);
			}
			//this will give the next symbol
			ch = fgetc(readFile);
			//this is the start of the path
			ch = fgetc(readFile);
			while (ch != '&')
			{
				path[j] = ch;
				j++;
				ch = fgetc(readFile);
			}
			isFolder = 1;
			i = 0;
			j = 0;
			strcpy(pathToAdd, findPath(path));

		}
		else if (ch == '$')
		{
			//this is a file -> $name$
			ch = fgetc(readFile); // $

			//this is for the name of the file
			while (ch != '$')
			{
				name[j] = ch;
				j++;
				ch = fgetc(readFile);
			}
			ch = fgetc(readFile);// will be start of path ->& (next symbol)
			ch = fgetc(readFile);//litterly the start ->path
			//this is for the path:
			while (ch != '&')
			{
				path[i] = ch;
				i++;
				ch = fgetc(readFile);
			}
			strcpy(pathToAdd, findPath(path));
			j = 0;
			i = 0;
			ch = fgetc(readFile);//for the next symbol : '{' (creator symbol)
			ch = fgetc(readFile); //start of the creator
			//this is for the creator
			while (ch != '{')
			{
				creator[j] = ch;
				j++;
				ch = fgetc(readFile);
			}
			ch = fgetc(readFile);//for the next symbol : '}' (data symbol)
			ch = fgetc(readFile); // start of data

			while (ch != '}')
			{
				data[i] = ch; // maximum 100 cells to read and write to a file (:
				i++;
				ch = fgetc(readFile);
			}
			ch = fgetc(readFile);// start of the verifications
			ch = fgetc(readFile);
			while (ch!= '-')
			{
				canReadAndWrite = ch - '0';
				ch = fgetc(readFile);
			}
			isFolder = 0;
			i = 0;
			j = 0;
		}
		else if (ch == ';')
		{
			//;userName;'password'\n
			ch = fgetc(readFile);
			while (ch != ';')
			{
				creator[i] = ch;
				i++;
				ch = fgetc(readFile);
			}
			ch = fgetc(readFile);
			ch = fgetc(readFile);
			while (ch != '\'')
			{
				passWord[j] = ch;
				j++;
				ch = fgetc(readFile);
			}
			ch = fgetc(readFile); //for the end of the line

			isFolder = 3;
			i = 0;
			j = 0;
		}
		if (isFolder == 1)
		{
			folderOrfile = createAFolder(name);
			folder = findFileOrFolder(pathToAdd, mainDir);
			addFileToFolder(folder, folderOrfile);
		}
		else if (isFolder == 0)
		{
			file = createFile(name , creator , canReadAndWrite);
			if (data[0] != 0)
			{
				writeToFile(file, data, hardDrive, manage);
			}
			folder = findFileOrFolder(pathToAdd, mainDir);
			addFileToFolder(folder, file);

		}
		else if(isFolder == 3)
		{
			userManage* temp = (userManage*)malloc(sizeof(userManage));
			temp = createAUser(creator, passWord);
			addUser(defaultUser , temp);
			//free(temp);
		}

		resName(name);
		resName(path);
		resName(pathToAdd);
		resName(creator);
		resName(passWord);
		isFolder = 2;
	}

}

char* findPath(char* input)
{
	int len = 0;
	len = strlen(input);
	int i = len;
	int j = 0;
	char path[TF] = { 0 };
	while (*(input +i)!= '%')
	{
		i--;
	}
	for (j = 0; j < i; j++)
	{
		path[j] = *(input + j);
	}
	return path;
}

void resName(char* input)
{
	int i = strlen(input) - 1;
	for (; i >= 0; i--)
	{
		*(input + i) = 0;
	}
}


void myFgets(char* input, int len)
{
	fgets(input, len, stdin);
	input[strcspn(input, "\n")] = 0;
}

void freeingSystem(DBM* deletedBlock, FM* files, hdd* hardDrive)
{
	for (int i = 0; i < 10; i++)
	{
		free(deletedBlock->deletedBlocks[i]);
		
	}
	for (int i = 0; i < 100; i++)
	{
		
		free(files->filesArr[i]);

	}
	for (int i = 0; i < AMOUNT_OF_BLOCKS; i++)
	{
		free(hardDrive->blocks[i]);
		
	}


}

void restartHdd(hdd* hardDrive)
{
	for (int i = 0; i < AMOUNT_OF_BLOCKS; i++)
	{
		hardDrive->blocks[i] = NULL;
	}
}

/*
* a recursive function that will free the whole project
*/
void freeingTheMainDir(iNode* mainDir)
{
	if (mainDir == NULL)
	{
		printf("END of folder\n");
	}
	else
	{
		if (mainDir->isFolder == 1 && mainDir->fileNext != NULL)
		{
			freeingTheMainDir(mainDir->fileNext);
		}
		freeingTheMainDir(mainDir->folderNext);
		free(mainDir);
	}
	
}


/*
* a function for creating a new user, in our file system we can only have 2 users, one defult and another one for the user
*/
userManage* createAUser(char* name, char* password)
{
	
	userManage* newUser = (userManage*)malloc(sizeof(userManage));
	newUser->userName = (char*)malloc(sizeof(strlen(name) + 1));
	newUser->passWord = (char*)malloc(sizeof(strlen(password) + 1));
	strcpy(newUser->userName, name);
	strcpy(newUser->passWord, password);
	newUser->next = NULL;
	return newUser;

	
}


void freeingUsers(userManage* defualt)
{
	userManage* next = defualt->next;
	while (next)
	{
		free(defualt->passWord);
		free(defualt->userName);
		free(defualt);
		defualt = next;
		next = next->next;
	}
	free(defualt->passWord);
	free(defualt->userName);
	free(defualt);
}

//this function is for the users
void SaveUsers(userManage* defualt, FILE* file)
{
	if (defualt != NULL)
	{
		while (defualt)
		{
			fprintf(file, ";%s;'%s'\n", defualt->userName, defualt->passWord);
			defualt = defualt->next;
		}
	}

}


bool isValidUser(userManage* first, char* userName, char* password)
{
	if (first == NULL)
	{
		return false;
	}
	if ((!strcmp(first->userName , userName)) &&(!strcmp(first->passWord , password)))
	{
		return true;
	}
	return isValidUser(first->next, userName, password);
}

userManage* returnUser(userManage* first, char* userName, char* password)
{
	if (first == NULL)
	{
		return NULL;
	}
	if ((!strcmp(first->userName, userName)) && (!strcmp(first->passWord, password)))
	{
		return first;
	}
	return returnUser(first->next, userName, password);

}