#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h> 

 
char cli_options[6][2] = {"-C", "-f", "-i", "-n", "-p", "-s"};
char cwd[200];

void readFileContents(char fileName[]){
     
    FILE *file_to_read = fopen(fileName, "r" );
    //FILE *file_to_write = fopen("newFile.txt", "w");
 
    if(file_to_read == NULL){
        printf("One of the files failed. :(");
    }
 
    char line[120];
 
    while(fgets(line, 120, file_to_read) != NULL){
        // ignore comments
        if(line[0] == '#'){
          continue;
        }
        printf("%s", line);
    }
 
    fclose(file_to_read);
}

// change directory else print error and return 1
int changeDirectory(char cdPath[]) {
    if(chdir(cdPath) == 0){
        // proof that directory changed
        // if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // printf("%s\n", cwd);
        // }
        return 0;
    } else {
        printf("Change of Directory FAILED");
        return 1;
    }
}

int main(int argc, char *argv[]){

    if(argc < 2){
        // get cwd and open bakefile located there if no other arguments.
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            readFileContents(strcpy(cwd, "bakefile.txt"));
        } else {
            printf("Fail to find a Bakefile...\n");
            printf("Please provied a path to your Bakefile or create one in this directory\n");
            perror("getcwd() error");
            return 1;
        }
        return 1;
    } else {
        
        // if change directory command then CD
        for(int index = 1; index < argc; index++) {
            if(strcmp(argv[index], "-C") == 0) {
                // if CD succeds leave loop, else end program
                if(changeDirectory(argv[index+1]) == 0){
                    break;
                } else {
                    return 1;
                }
            }
        }



    }

 
    return 0;
}