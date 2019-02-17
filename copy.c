/*
(8 puncte) Program care primeste ca argumente in linia de comanda doua
 directoare d1 si d2 (d1 exista, d2 nu) si copiaza recursiv toata
 arborescenta cu originea in d1 intr-o arborescenta cu originea in d2
 (intre cele doua arborescente difera doar numele directorului origine).
*/
#include<stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include<sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int testType(const char *path){

        struct stat s;
        if(stat(path, &s) == -1)
                return -1;

        if(!((s.st_mode & S_IFMT) ^ S_IFDIR))
                return 0;

        return 1;

}
int copyFile(const char* file1, const char* file2){

        FILE *fpr, *fpw;
        int ch;

        if((fpr = fopen(file1, "rb")) == NULL){
                return 1;
        }

	if((fpw = fopen(file2, "wb")) == NULL){
		fclose(fpr);
                return 1;
        }

	while((ch = getc(fpr))!= EOF){
		putc(ch, fpw);
	}

	fclose(fpr);
	fclose(fpw);

        return 0;

}

int copyRecursive(const char *path1, const char *path2){

        DIR *pd; struct dirent *pde;
        struct stat s;
	char *cale1, *cale2, *specificator1, *specificator2;;

        if(stat(path1, &s) == -1)
                return 1;


        if(!((s.st_mode & S_IFMT) ^ S_IFDIR)) {

                if((pd = opendir(path1)) == NULL)
                        return 1;

                if(mkdir(path2, s.st_mode) == -1){
			closedir(pd);
                        return 1;
                }

                cale1 = (char*)malloc(strlen(path1) + 2);

                if(cale1 == NULL){
			closedir(pd);
                        errno = ENOMEM;
			
                        return 1;

                }

                cale2 = (char*)malloc(strlen(path2) + 2);

                if(cale2 == NULL){
			free(cale1);
			closedir(pd);	
                        errno = ENOMEM;
			
			
                        return 1;
                }

                strcpy(cale1, path1);
		if(cale1[strlen(path1) - 1] != '/')
                        strcat(cale1,"/");

                strcpy(cale2, path2);
		if(cale2[strlen(path2) - 1] != '/')
                        strcat(cale2,"/");

                while((pde = readdir(pd)) != NULL) {


                        specificator1 = (char*)malloc(strlen(cale1)+ strlen(pde->d_name) + 1);
                        if(specificator1 == NULL){
                                
				closedir(pd);
				free(cale1);
                                free(cale2);
				errno = ENOMEM;
                                return 1;
                        }
                        strcpy(specificator1, cale1);
                        strcat(specificator1, pde->d_name);

                        specificator2 = (char*)malloc(strlen(cale2) + strlen(pde->d_name) + 1);
                        if(specificator2 == NULL){
                                
				closedir(pd);
				free(cale1); free(cale2);
				free(specificator1);
				errno = ENOMEM;
                                return 1;
                        }
                        strcpy(specificator2, cale2);
                        strcat(specificator2, pde->d_name);

                        if(testType(specificator1) == 1){
                                if(copyFile(specificator1, specificator2) == 1){
                                       // perror("copyFile");
				closedir(pd);
				free(cale1);
                                free(cale2);
				free(specificator1);
				free(specificator2);
                                        return 1;
                                }
                        }
                        else if(testType(specificator1) == 0){
                                if(strcmp(pde->d_name,".") && strcmp(pde->d_name,"..")){
                                        if(copyRecursive(specificator1,specificator2) == 1){
                                                closedir(pd);
                                                free(cale1);
                                                free(cale2);
                                                free(specificator1);
						free(specificator2);
                                                return 1;

                                        }

                                }

                        } else{
				closedir(pd);
				free(cale1); free(cale2);
				free(specificator1);free(specificator2);
                                return 1;
                        }
                free(specificator1);
                free(specificator2);
                }

                closedir(pd);
                free(cale1);
                free(cale2);

        }
        return 0;
}
int main(int argv, char** argc){

	if(argv != 3){
                fprintf(stderr, "Call format: %s existentFolder newFolder\n",argc[0]);
		return 1;
	}

          if(copyRecursive(argc[1],argc[2])){
                perror("copyRecursive");
                return 1;
        }
          return 0;
}
