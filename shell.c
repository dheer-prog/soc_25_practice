#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
#include<sys/wait.h>
void continuer(){
    return; 
}
void tokenize(char* cmd,char* tokenized[]){
    int i=0; 
    int  tokenized_index=0;
    while(i<1024){
        char token[49];
        int counter=0; 
        while(cmd[i]!=' ' &&  cmd[i]!='\0' && cmd[i]!='\n'){
            if(counter>48){
                tokenized[0]=NULL;
                return;
            }
            token[counter]=cmd[i];
            i++; 
            counter++; 
        }
        
        token[counter]='\0';
        tokenized[tokenized_index]=malloc(49*sizeof(char)); 
        strcpy(tokenized[tokenized_index],token);
        tokenized_index++; 
        if(cmd[i]=='\0' || cmd[i]=='\n'){
            break; 
        }
        i++;   
    }
    tokenized[tokenized_index]=NULL;
    return;
}
int main(){ 
    char cmd[1024];
    while(1){
        char curr_path[256]; 
        getcwd(curr_path,256); 
        printf("%s$",curr_path); 
        fgets(cmd,1024,stdin);
        cmd[strcspn(cmd,"\n")]='\0'; 
        if(cmd[0]=='\n' || cmd[0]=='\0'){
            continue;
        }
        if(cmd[0]=='c' && cmd[1]=='d'){
            char* tokenized[64]; 
            tokenize(cmd,tokenized); 
            int c=chdir(tokenized[1]); 
            if(c==-1){
                perror("No directory exists"); 
                fflush(stderr); 
            }
        }
        else{
            if(strncmp(cmd,"exit",5)==0){
                exit(1);
            }
            int rc=fork();
            signal(SIGINT,SIG_DFL); 
            if(rc<0){
                printf("ERROR");
            }
            else if(rc==0){
                char* tokenized[64];
                tokenize(cmd,tokenized);
                execvp(tokenized[0],tokenized);
                if(tokenized[0]==NULL){
                    perror("failed");
                    exit(1); 
                }
                perror("execvp failed");
                exit(1); 
            }
            else{
                signal(SIGINT,continuer); 
                waitpid(rc,NULL,0); 
            }
        }     
    } 
    return 0; 
}