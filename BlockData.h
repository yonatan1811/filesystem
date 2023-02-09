#ifndef BlockData
#define BlockData

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define DATA_SIZE 9
#define AMOUNT_OF_BLOCKS 100
#define TF 100
#define INPUT_LEN 35
#define NAME_SIZE 20
#define MyEOF '^'
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>


typedef struct Block_t
{
	int blockNumber;
	int blockFullUsed;
	char data[DATA_SIZE];
	struct Block_t* next;
	int currIndex;
	

}Block;





//will be a file or a folder , if its a folder it wont have blocks
typedef struct iNode_t
{
	char name[NAME_SIZE];
	int isFolder;
	struct iNode_t* folderNext;
	struct iNode_t* fileNext;
	char creator[NAME_SIZE];
	int canWriteAndRead;
	Block* iBlock;


}iNode;

typedef struct FM_t
{
	int totalFiles;
	iNode* filesArr[100];

}FM;


typedef struct SuperBlock_t
{
	iNode* firstNode;
}SuperBlock;

typedef struct hdd_t
{
	int totalBlocksUsed;
	int freeBlocks;
	int totalBlocks;
	Block* blocks[AMOUNT_OF_BLOCKS];
}hdd;

typedef struct dManage_t
{
	int freeDeletedBlocks;
	Block* deletedBlocks[10];
}DBM;

typedef struct userManage_t
{
	char* userName;
	char* passWord;
	struct userManage_t* next;
}userManage;


void updateHardDrive(hdd* hardDrive, Block* block);


//this file role is to "delete files" and put the blocks of the file into an array of deleted blocks
void deleteFile(hdd* hardDrive, DBM* w, iNode* file, iNode* folder);




//this will create a file without data, wont have next
iNode* createFile(char* name , char* creator ,  int canReadAndWrite);

//will create a block
Block* CreateBlock(int blockNum);

void restartName(iNode* file);





//with this function you can check and get the length of the file
int fileLen(iNode* curr);

//with this function u can read a file
void readFileSystem(iNode* file);



//with this function we will write to a file
void writeToFile(iNode* file, char* input , hdd* hardDrive , DBM* manage);


void updateDeletedBlocks(DBM* manage);


//we will create a harddrive
hdd Createhdd();

//this function will get the field of the data and will put 0 in each cell
void restartingData(Block* block);

void updateBlockIfUsed(iNode* file);

DBM createDBM();

FM createFM();

void updateFM(FM* manage, iNode* file);

//this function will create a folder
iNode* createAFolder(char* name);

//when we add a file to a folder we need to put the folder at the next field
void addFileToFolder(iNode* folder , iNode* file);



//this function will print out all the folders and files the folder has
void dir(iNode* folder);

//this function will get input like : c:\custom\i and then we will get to this file and read it , and if its a folder we will print that its found
iNode* findFileOrFolder(char* input, iNode* mainDir);

char* getCurrName(char* input , int i);

char* getUserName(char name[]);

char* getPassWord(char passWord[]);

bool ValidUser(char* userName , char* passWord , char* constName , char* constPassWord);


void addUser(userManage* firstUser, userManage* currUser);

int loadOrNew();

char* enterPath();

/*
* this function will be able to save a project using different symbols to present different data saving stuff
* input: the file we want to save in , and the maindir, the C directory
* output : none
*/
void SaveProject(FILE* file, iNode* mainDir , userManage* defualt);

//this function will help us saving multiply folders in a folder for example 
void SaveFolder(FILE* file, iNode* folder , char* folderPath);

//this function will return all of the data the file has
char* readingDataAndSave(iNode* file);

//this function will open a file and get the data from there and will create all the files and folders.
void loadProject(FILE* readFile, iNode* mainDir, hdd* hardDrive, DBM* manage, userManage* defaultUser);


char* findPath(char* input);

//a simple function to restart a name
void resName(char* input);

//a simple function for fgets with an upgrade
void myFgets(char* input, int len);


void freeingSystem(DBM* deletedBlock, FM* files, hdd* hardDrive);

void restartHdd(hdd* hardDrive);


/*
* a recursive function that will free the whole project
*/
void freeingTheMainDir(iNode* mainDir);

/*
* a function for creating a new user, in our file system we can only have 2 users, one defult and another one for the user
*/
userManage* createAUser(char* name, char* password);


void freeingUsers(userManage* defualt);

//this function is for the users
void SaveUsers(userManage* defualt, FILE* file);

bool isValidUser(userManage* first, char* userName, char* password);

userManage* returnUser(userManage* first, char* userName, char* password);

#endif