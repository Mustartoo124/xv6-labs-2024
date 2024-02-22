#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int match(char *re, char *text)
{
    if(re[0] == '^')
        return matchhere(re+1, text);
    do{  // must look at empty string
        if(matchhere(re, text))
            return 1;
    }while(*text++ != '\0');
    return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
    if(re[0] == '\0')
        return 1;
    if(re[1] == '*')
        return matchstar(re[0], re+2, text);
    if(re[0] == '$' && re[1] == '\0')
        return *text == '\0';
    if(*text!='\0' && (re[0]=='.' || re[0]==*text))
        return matchhere(re+1, text+1);
    return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
    do{  // a * matches zero or more instances
        if(matchhere(re, text))
            return 1;
    }while(*text!='\0' && (*text++==c || c=='.'));
    return 0;
}

void find(char * path,char * filename){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            // if (match(filename, path)) {
            //       printf("%s\n",path);
            // }
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            if(*(p-1)!='/'){
                *p++ = '/';
            }
            // *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if (!strcmp(de.name, filename)){
                    // if (match(filename, de.name)) {
                    printf("%s\n",buf);
                }
                if (strcmp(de.name,".")&&strcmp(de.name,"..")){
                    find(buf, filename);
                }
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){

    if (argc!=3){
        printf("Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}