char cli_options[6][2] = {"-C", "-f", "-i", "-n", "-p", "-s"};
char cwd[200];

void readFileContents(char fileName[]){
     
    FILE *file_to_read = fopen(fileName, "r" );
    //FILE *file_to_write = fopen("newFile.txt", "w");
 
    if(file_to_read == NULL){
        printf("One of the files failed. :(");
    }
 
 char line[120];
    bool iscont = false;
    char stor[360];   // init the size of array you need
    char swap[90][90];
    int d = 0; //index for swap

    while(fgets(line, 120, file_to_read) != NULL){
        
        if(line[strlen(line)-2] == 92) { // escape the '\' char
            line[strlen(line)-2] = ' '; // replace '\' with a space
            strtok(line, "\n"); // remove newline char
            strcat(stor, line); //string cat here
            iscont = true;
            continue;
        }
        if (iscont == true) {
            strcat(stor, line); // string cat
            iscont = false;
        } else {
            strcpy(stor, line);       
        }
        // ignore comments
        if(stor[0] == '#'){
            continue;
        }
        //The swap function
	char pre[90];
	char post[90];
	
	//Adding the pre and post values to swap
	if (isInArray(61, line) == true) {
            int index = (line.indexOf(61) >= 0);
	    int c = 0;
            while (c < index) {
               pre[c] = string[c];
               c++;
            }
            while (c < strlen(line)) {
	       post[c] = string[c];
	       c++;
	    }
            char arr[d][d] = pre,post; //This doesn't look right to me
	    d++;
	}

        //Actively replacing pre with post
       While (e = 0, e =< d, e++;) {
           for (char* p = line; p = strchr(p, swap[e][]); ++p) {
             *p = swap[][e];
           }
	   //I'm not sure if thats how you pull values out of a 2x array
	   //also i dont think this works at all so i'll rewrite it with iterating
	   //through the positions and stuff
       }

        printf("%s", stor);
        stor[0] = '\0'; // set first postition to null byte
    }

    fclose(file_to_read);
}


void buildTargetStruct(){
    printf("build target struct!");
}

// Checks if file exists at local path
bool targetExistsLocally(char filePath[]) {
    if( access( filePath, F_OK ) != -1 ) {
        struct stat attr;
        stat(filePath, &attr);
        printf("Last modified time: %ld \n", attr.st_mtime);
        return true;
    } else {
        return false;
    }
}

/* int dateModifiedOfRemotePath(struct target t)
path from the target struct is used as a url to get the remote file.
process if forked, curl is called in the child process with flags that send response headers to stdout.
Output is piped back into parent process and copied into a variable 'data'
Function searches for the 'last-modified' header.
The 'last-modified' header line is stripped and is converted from string -> time structs -> epoch timestamp
and finally stored in the target struct ready for compaison to other targets.
return 0 on sucess, else -1;
TODO: 
    - change input argument to a pointer to target struct,
    - add robust error handling and add ignore on global 'ignore error flags'
    - store timestamp in struct.
*/
int dateModifiedOfRemotePath(char urlPath[]) {
    char curl[] = "/usr/bin/curl";

    int link[2];
    int pid;
    char data[4096];

    if (pipe(link)==-1)
        die("pipe");

    if ((pid = fork()) == -1)
        die("fork");

    if(pid == 0) {

        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        execlp(curl, "curl", "curl", "-s", "-i", "-X", "HEAD", urlPath, NULL);
        die("execlp");

    } else {
        close(link[1]);
        read(link[0], data, sizeof(data));
        
        char modified[] = "last-modified";
        char dateTime[36];

        char *token;
        token = strtok(data, "\n");
        
        // Find 'last modified' amongst response data
        while( token != NULL ) {
            if(strstr(token, modified) != NULL) {
                // if found strip string down to datetime string
                int todex = 0;
                int tidex = 0;
                while(token[todex] != ',') {
                    todex++;
                }

                while(token[todex] != '\0') {    
                    todex++;
                    dateTime[tidex] = token[todex];
                    tidex++;
                }
                // test to see datetime from 'last modified'
                //printf("%s\n", dateTime);

                // Create a time struct from the string...
                struct tm tm;
                int t;

                if (strptime(dateTime, "%d %b %Y %H:%M:%S", &tm) == 0){
                    // There was a error converting time to a strcut.
                    printf("there was an error!");
                } else {
                    // if no errors with string to struct conversion, create an epoch timestamp.
                    t = mktime(&tm);
                    if (t == -1) {
                        // There was an error converting time to a seconds 
                    } else {
                        // TODO: Store the timestamp within the Target Struct....
                        printf("seconds since the Epoch: %ld\n", (long) t);

                    }
                }
            }
            token = strtok(NULL, "\n");
        }

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

    dateModifiedOfRemotePath("https://stackoverflow.com/questions/15334558/compiler-gets-warnings-when-using-strptime-function-c");

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
