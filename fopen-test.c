#include "Headerfile.h"


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

// Check if file exists at local path
bool targetExistsLocally(char filePath[]) {
    if( access( filePath, F_OK ) != -1 ) {
        // struct stat attr;
        // stat(filePath, &attr);
        // printf("Last modified time: %s", ctime(&attr.st_mtime));
        return true;
    } else {
        return false;
    }
}

// /char urlPath[]
bool canICurlIt() {
    char curl[100] = "/usr/bin/curl";

    int link[2];
    int pid;
    char foo[4096];

    if (pipe(link)==-1)
        die("pipe");

    if ((pid = fork()) == -1)
        die("fork");

    if(pid == 0) {

        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        execlp(curl, "curl", "curl", "-s", "-i", "-X", "HEAD", "https://socialtriggers.com/perfect-blog-post/", NULL);
        die("execlp");

    } else {
        close(link[1]);
        // int nbytes = read(link[0], foo, sizeof(foo));
        read(link[0], foo, sizeof(foo));
        
        char *token;
        token = strtok(foo, "\n");
        
        while( token != NULL ) {
            printf("tkn %s\n", token );
            token = strtok(NULL, "\n");
        }

        // struct stat attr;
        // stat(foo, &attr);
        // printf("Last modified time: %s", ctime(&attr.st_mtime));
        // printf("Output: (%.*s)\n", nbytes, foo);
        // strncmp(pre, str, strlen(pre)) == 0;
        wait(NULL);

    }
    return 0;
}


// change directory else print error and return 1
int changeDirectory(char cdPath[]) {
    if(chdir(cdPath) == 0){
        // proof that directory changed
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
        printf("curled it\n");
        return 0;
    } else {
        printf("Change of Directory FAILED");
        return 1;
    }
}

int main(int argc, char *argv[]){

    char bakepath[1024];

    canICurlIt();

    if(argc < 2){
        // get cwd and open bakefile located there if no other arguments.
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            if(targetExistsLocally("bakefile.txt")) {
                printf("Target Exists Locally \n");
                readFileContents(strcpy(cwd, "bakefile.txt"));
            } else {
                printf("Target doesn't exist!");
            }
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
                if(changeDirectory(argv[index+1]) == 1){
                    printf("Path argument to -C is not valid");
                    return 1;
                }
            }
            if(strcmp(argv[index], "-f") == 0) {
                printf("-f");
                strcpy(argv[index+1], bakepath);
                printf("%s", bakepath);
                printf("execute on bake path");
                readFileContents(bakepath);
            }
        }



    }

    return 0;
}