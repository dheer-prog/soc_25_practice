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
        char* tokenized[64];
        tokenize(cmd,tokenized); 
        if(cmd[0]=='\n' || cmd[0]=='\0'){
            continue;
        }
        if(cmd[0]=='c' && cmd[1]=='d'){
             
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
            int number_of_cmds=0;
            char* cmd_pipe[64][64];
            int word_count=0;
            int counter=0;  
            for(int i=0;i<64;i++){
                
                if( tokenized[i]==NULL  || (strncmp(tokenized[i],"|",1)==0)){
                    cmd_pipe[number_of_cmds][counter]=NULL;
                    counter=0; 
                    number_of_cmds++; 
                    if(tokenized[i]==NULL){break;}
                }
                else{
                    cmd_pipe[number_of_cmds][counter]=tokenized[i];
                    counter++; 
                }
            }
            int pipes[number_of_cmds-1][2]; 
            for(int i=0;i<number_of_cmds-1;i++){
                if(pipe(pipes[i])==-1){perror("HAG DIYA");}
            }
            int n=number_of_cmds;
            for(int i=0;i<number_of_cmds;i++){
                int rc=fork();
                signal(SIGINT,SIG_DFL); 
                if(rc<0){
                    printf("ERROR");
                }
                else if(rc==0){
                    if(i>0){ 
                        dup2(pipes[i-1][0],0); 
                    }
                    if(i<n-1){
                        dup2(pipes[i][1],1); 
                    }
                    for(int j=0;j<n-1;j++){ 
                        close(pipes[j][0]); 
                        close(pipes[j][1]); 
                    }
                    execvp(cmd_pipe[i][0],cmd_pipe[i]);
                    if(tokenized[0]==NULL){
                        perror("failed");
                        exit(1); 
                    }
                    perror("execvp failed");
                    exit(1); 
                }
                else{
                    for(int j=0;j<i;j++){
                        close(pipes[j][0]); 
                        close(pipes[j][1]);
                    }
                    signal(SIGINT,continuer); 
                     
                }    
            }
            for(int i=0;i<n;i++){
                wait(NULL);
            }
                   
        }     
    } 
    return 0; 
}