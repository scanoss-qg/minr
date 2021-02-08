/*
// SPDX-License-Identifier: GPL-2.0-or-later
 * @file trie.h
 * @description Functions to create and search for keywords related to the
 * 	use of cryptographic alorithms
 * @date January 29th 2021
 * @author Quique
 */

#ifndef TRIE_H_
#define TRIE_H_
#include "malloc.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#define SINIT 0
#define SWORD 1

struct T_TrieNode;
struct T_TrieNode{
	int type;
	int ocurrences;
	unsigned short coding;
	struct T_TrieNode * next;
	struct T_TrieNode * nodos[39];
	char algorithmName[10];
};

FILE 		*fp;
char		currentName[50];
unsigned short	currentCode=0;
extern struct 	T_TrieNode * root;
char currWord[50];
unsigned char currWordLen=0;
unsigned char defState = SINIT;
int isValidChar(char c){
	if(((c>='a')&& (c<='z'))||((c>='A')&& (c<='Z')))
		return 1;
	else return 0;
}

int isValidJoiningChar(char c){
	if(((c=='-')&& (c=='.'))||((c=='_')))
		return 1;
	else return 0;
}
/**
@brief Returns the index for a specific valid character
*/
char indexOf(char c){
if ((c >= 'A') && (c <= 'Z')) c = c + 32;
if((c>='a')&&(c<='z')) return c-'a';
char sig = 'z'-'a';
switch(c){
	case '*':return sig+1;
	case '.':return sig+2;
	case '_':return sig+3;
	case '-':return sig+4;
	case '0':return sig+5;
	case '1':return sig+6;
	case '2':return sig+7;
	case '3':return sig+8;
	case '4':return sig+9;
	case '5':return sig+10;
	case '6':return sig+11;
	case '7':return sig+12;
	case '8':return sig+13;
	case '9':return sig+14;
	default: 
	return -1;
	}
}
/**
@brief inserts a keyword into the seach structure
@description Insert a keyword by following the branches of the tree until a \0 is found. 
*/


void insert(char *token, struct T_TrieNode *currNode,char* algorithmName, unsigned short coding){
	
	if(token[0]=='\0') {
		sprintf(currNode->algorithmName,"%s",algorithmName);
		currNode->type=1;
		currNode->coding=coding;
		return;
	}

	unsigned int realIndex=indexOf(token[0]);
	
	if((realIndex<0)||(realIndex >39)) {
		return;
	}
	if(currNode->nodos[realIndex]==0){
		struct T_TrieNode *newLeaf;
		newLeaf=(struct T_TrieNode *) malloc (sizeof (struct T_TrieNode));
		for(int i =0;i<39;i++)
				newLeaf->nodos[i]=0;
		newLeaf->ocurrences=0;
		newLeaf->type = -1;
		currNode->nodos[realIndex]=newLeaf;
	}
	insert(&token[1],currNode->nodos[realIndex],algorithmName,coding);

}
/**
* @brief Searches a token on the index
* @description Given a T_TrieNode, searches recursively the (sub) token
*		deep into the structure. If the whole token manages to
*		reach a leaf (type!=-1), the keyword token belongs to an
*		encryption algorithm.
*		Otherwise, is just a sub string of an algorithm 
*		(coincidence) */
struct T_TrieNode * searchAlgorithm(char *token, struct T_TrieNode *node){
	if(token[0]=='\0') {
		if(node->type==-1) 
			return NULL; 
		else 
			return node;
	}
	int indexValid=0;
	while(indexOf(token[indexValid])==-1){
		indexValid++;
		
		}
	unsigned char index = indexOf(token[indexValid]);
	if((index<0)||(index >40)) {
		return NULL;
	}
	if(node->nodos[index]==0)
		return NULL;
	return searchAlgorithm(&token[1],node->nodos[index]);

}

/**
* @brief Normalizes the text to lower case. 
* @note Expects a \0 to end the normalization.
* It lowers only letters from a to z
*/

void toLower(char *strToLow){
	int index=0;
	int i=strlen(strToLow);
	while(strToLow[index]!='\0'&& i<index){
		if ((strToLow[index] >= 'A') && (strToLow[index] <= 'Z'))
			strToLow[index]=strToLow[index] + 32 ;
		index++;
	}
}

void trimAndLow(char *strToLow){
	int index=0;
		while(strToLow[index]!='\0'){
		if((indexOf(strToLow[index])<0)
		 ||(indexOf(strToLow[index])>39)) {
			index++;
			break;
		}
		if ((strToLow[index] >= 'A') && (strToLow[index] <= 'Z'))
			strToLow[index]=strToLow[index] + 32 ;
		index++;
	}
	char *aux = (char*) malloc (index+1);
	strcpy(aux,strToLow);

	aux[index]='\0';
	strToLow=aux;
}

int parseChar(char currChar){

	switch (defState){
	case SINIT: 
		if(isValidChar(currChar)) {
			currWord[currWordLen++]=currChar;
			defState=SWORD;
		}
		break;
	case SWORD:
		if(isValidChar(currChar)) {
			currWord[currWordLen++]=currChar;
		} else { 
			toLower(currWord);
			defState = SINIT;
			currWordLen = 0;
			return 1;
		} break;
 	} 
 	return 0;

}


 /**
 * @brief Loads algotithm definitions from a file
 * @param path Scanning directory
 * @param fileName name of the file to be scanned. 
 * @note The absolute path is formed by path/filename. File content is as follows:
 * name=<algorithm Name>. Spaces are allowed. Ends with <cr><lf>
 coding=<algorithm strenght> a number in bits. There is no validation on this number
 */
void parseFile(char *path,char *fileName){char include[50];
	char absolutePath[256];
	sprintf(absolutePath,"%s/%s",path,fileName);
	//printf("scanning %s\r\n",absolutePath);
	if ((fp = fopen(absolutePath,"r")) == NULL){
	       printf("Error! crypto definitions file");
	       exit(1);
	}

	while(!feof(fp)){
		char auxName[50];
		char auxLn[50];
		
		unsigned short auxCode=0;
		char match;
		//memset(auxName,0,50);
		fscanf(fp,"%s\r\n",auxLn);
		match=sscanf(auxLn,"name=%s",auxName);
		if(match==1){
			memset(currentName,0,50);
			
			/*memcpy(currentName,auxName,10);
			memset(auxName,0,10);*/
			//strcpy(currentName,auxName);
			sprintf(currentName,"%s",auxName);
			//printf("Name es: %s\r\n",auxName);
			
		} else {
			match=sscanf(auxLn,"coding=%hd\r\n",&auxCode);
			//match=fscanf(fp,"coding=%hd\r\n",&auxCode);
			//printf("coding: %d\r\n",match);
		
			if(match==1){
			//	printf("code: %d\r\n",auxCode);
		
				currentCode=auxCode;
			} else {
			
				match=sscanf(auxLn,"%s",include);
			//	printf("Token: %d\r\n",match);
		
				if(match==-1) break;
				if(match==1){
				//printf("token es: %s\r\n",include);
		
				
					toLower(include);
					insert(include,root,currentName,currentCode);
				}
			}
		}
	}
	
	fclose(fp);
}
/**
@brief Parses a system directory looking for files containing
*	cryptographic keywords
*/
void parseDirectory(char* root){
	struct dirent *de;  // Pointer for directory entry
    DIR *dr = opendir(root);
    if (dr == NULL) {
	        printf("Could not open current directory" );
	        return ;
	}


    while ((de = readdir(dr)) != NULL){
    	if(de->d_name[0]=='.')
    		continue;
	parseFile(root,de->d_name);
    }
    closedir(dr);
   // All files have been parsed
}



#endif /* TRIE_H_ */
