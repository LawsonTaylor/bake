/* CITS2002 Project 2018
 Name(s): Lawson Taylor
 Student number(s): 21813593
 */

#include "Headerfile.h"


char cli_options[6][2] = {"-C", "-f", "-i", "-n", "-p", "-s"};
char cwd[200];
char trueline[1000];
int tlindex = 0;


struct Target {
   char *name;
   struct target *dependencies[100];
   struct target *parents[10];
   bool build;
   char *actions[100];
   long modificationDate;
};

struct Target TargetList[100];
int noTargets = 0;
char *varName[256];
char *varValues[256];
int varCount = 0;

bool targetIsRemote(char targetName[]){
    if(strstr(targetName, "file://") != NULL) {
        return true;
    } 
    if(strstr(targetName, "http://") != NULL) {
        return true;
    } 
    if(strstr(targetName, "https://") != NULL) {
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

    if (pipe(link)==-1){
        printf("die Pipe");
        die("pipe");
    }

    pid = fork();

    if (pid == -1){
        printf("pid = %d", pid);
        printf("die fork");
        die("fork");
    }

    if (pid < 0) {
        printf("pid = %d", pid);
        fprintf(stderr, "fork() failed: %s\n", strerror(errno));
    }

    if(pid == 0) {
        printf("CID = %d", pid);
        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        printf("/usr/bin/curl");
        execlp(curl, "curl", "curl", "-s", "-i", "-X", "HEAD", urlPath, NULL);
        die("execlp");

    } else {
        printf("PID: %d", pid);
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
                        printf("there was an error!");
                    } else {
                        // TODO: Store the timestamp within the Target Struct....
                        printf("seconds since the Epoch: %ld\n", (long) t);
                        TargetList[noTargets].modificationDate = (long) t;

                    }
                }
            }
            token = strtok(NULL, "\n");
        }

        wait(NULL);

    }
    return 0;
}

// returns index to splice to for target if exists, else -1;
int isTargetLine(char line[]){
    int i = 0;
    while(line[i] != '\0') {
        // FIX THIS SO : REQUIRES SPACE LEFT AND RIGHT TO ACCOUNT FOR URLS, ETC
        if(line[i] == ':' && line[i-1] == ' ' && line[i+1] == ' '){
            return i-1;
        }
        i++;
    }
    if(line[i] == '\0'){
        return -1;
    }
    return -1;
}


// Checks if file exists at local path
bool targetExistsLocally(char filePath[]) {
    if( access( filePath, F_OK ) != -1 ) {
        // File exist
        struct stat attr;
        stat(filePath, &attr);
        TargetList[noTargets].modificationDate = attr.st_mtime;
        // printf("Name: %s \n", TargetList[noTargets].name);
        // printf("Last modified time: %ld \n", TargetList[noTargets].modificationDate);
        return true;
    } else {
        // File dosen't exist - set build true and modification date -1;
        TargetList[noTargets].modificationDate = -1;
        TargetList[noTargets].build = true;
        // printf("Not file Name: %s \n", TargetList[noTargets].name);
        // printf("Last modified time: %ld \n", TargetList[noTargets].modificationDate);
        return false;
    }
}

int buildTarget(int i, char line[]) {
    // Get target name
    char targetName[80];
    int x = 0;
    while(x != i){
        targetName[x] = line[x];
        x++;
    }
    // Create Struct
    struct Target ptr = {NULL};
    // // add name
    printf("%s \n", strdup(targetName));
    ptr.name = strdup(targetName);
    memset(targetName, 0, 80);
    TargetList[noTargets] = ptr;
    // printf("Got the target Name: %s \n", ptr->name);
    // check if it exists and get date modified
    if(targetIsRemote(ptr.name)){
        printf("Is a remote!\n");
        dateModifiedOfRemotePath(ptr.name); // dosen't fucking fork.
        // printf("%")
    } else {
        printf("Not a remote!\n");
        targetExistsLocally(ptr.name);
    }
    noTargets++;
    return 0;
}

int isVariabeDef(char ln[]){
    int vli = 0;
    while(ln[vli] != '\0'){
        if(ln[vli] == '='){
            return vli;
        }
        vli++;
    }
    vli = 0;
    return -1;
}

void stripWhiteSpaceNpush(char st[], int s, int e, int v) {
    int startVar = s;
    int endVar = e;
    char var[56];
    while(st[startVar] == ' '){
        startVar++;
    }
    while(st[endVar] == ' '){
        endVar--;
    }
    int y = 0;
    for(int x = startVar; x <= endVar; x++){
        var[y] = st[x];
        y++;
    }
    var[y+1] = '\0';

    if(v == 0){
        varName[varCount] = strdup(var); 
        printf("This is a variable name: %s\n", varName[varCount]);
    } else {
        varValues[varCount] = strdup(var); 
        printf("This is a value: %s\n", varValues[varCount]);

    }
}

void getVariable(char stor[], int eq){
    //  ADD ERROR HANDELING....
    // Add Variable
    stripWhiteSpaceNpush(stor, 0, eq-1, 0);
    // Add Value
    stripWhiteSpaceNpush(stor, eq + 1, strlen(stor) -1, 0);
    // increment number of variables;
    varCount++;
}

// Var Replace
char *varReplace(char *line, char *var, char *val)
{
  static char buffer[1024];
  char *p;

  if(!(p = strstr(line, var))){
    return line;
  }

  strncpy(buffer, line, p-line);
  buffer[p-line] = '\0';

  sprintf(buffer+(p-line), "%s%s", val, p+strlen(var));

  return buffer;
}



void readFileContents(char fileName[]){
     
    FILE *file_to_read = fopen(fileName, "r" );
    //FILE *file_to_write = fopen("newFile.txt", "w");
 
    if(file_to_read == NULL){
        printf("One of the files failed. :(");
    }
 
    char line[1024];
    bool iscont = false;
    char stor[1024];   // init the size of array you need

    while(fgets(line, 1024, file_to_read) != NULL){
        
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

        int var = isVariabeDef(stor);

        if(var != -1){
            // Add error handeling....
            getVariable(stor, var);
            continue;
        }
        // split line and check if variables are in line....
        char *tkn, *str, *fr33;

        fr33 = str = strdup(stor);
        while ((tkn = strsep(&str, " "))){
            int x = 0;
            while(x != varCount){
                if(strcmp(varName[x], tkn) != 0) {
                    
                }
                x++;
            };
        };
        free(fr33);
        

        // printf("%s \n", stor);
        stor[0] = '\0'; // set first postition to null byte

    }

    fclose(file_to_read);
}

int secondRead() {

    // Will be moved to second read //
    // if newTarget set globalTarget as newTarget
    //      read line, check 
    //      if dependency exits { add dependency by pointer }
    //      else { create dependency } => create dependency() { modification date, name/path }
    // if action line
    //      add action to target
    // int spliceTargetIndex = isTargetLine(line);
    // // is a new Target
    // if(spliceTargetIndex != -1){
    //     buildTarget(spliceTargetIndex, line);
    // } 
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
