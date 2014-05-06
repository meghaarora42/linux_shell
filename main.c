#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include<signal.h>
#include <fcntl.h>
#define hist_size 1024 
char *hist[hist_size];
int f=0;   //to save change in directory
int head=0, filled=0;

void parse(char *word,char **argv)
{
 	int count=0;
	memset(argv, 0, sizeof(char*) * (64));    // copies 0 to the first 64 characters of the string pointed to by the argument argv.
      	char *lefts=NULL;
	const char *split=" ";  //setting delimeter
	  while(1==1)	
	{	
	char *p=strtok_r(word , split, &lefts);
	if(p==NULL){
		break;}
       argv[count]=p;
	word=lefts;
	count++;
	}
	if (strcmp(argv[0], "exit") == 0)  
         exit(0);
        else if(strcmp(argv[0],"cd")==0)
    	 {
		int ch = chdir(argv[1]); 
		if(ch<0)
		{
			printf("Directory does not exist");
		}
		f=1;
	}
}

char * trim(char *string)   //remove extra spaces
{
 int i=0;
 int j=0;		
	char *ptr=malloc(sizeof(char*)*strlen(string));
 for(i=0;string[i]!='\0';i++)
	if(string[i]!=' ')
        {
		ptr[j]=string[i];
		j++;
	}
	ptr[j]='\0';
 	string=ptr;
	return string;
//	printf("%s",string);
 }
void  execute(char **argv)
{
     pid_t  pid;
     int    status;
     if ((pid = fork()) < 0) 
     {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0)
     {          
          if(execvp(argv[0],argv)<0)
            	printf("**error in exec");
	exit(0);
          }
     
     else
     {                                  
          while (wait(&status) != pid)      
               ;
     }
}

void  execute_file(char **argv,char *output)  //output : file in which we need to print the output
{
  //printf("output %s \n",output);
  //printf("argv %s \n",*argv);
  pid_t  pid;
  int status,flag;
  char *file=NULL;
   if ((pid = fork()) < 0) 
    {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);//fclose(fd1);
     }
     else if (pid == 0)
     {
	
	if(strstr(output,">")>0){  //returns a pointer to first occurence after > or null
		//printf("here i am \n");
		
		
		char *p=strtok_r(output ,">", &file);
		output +=1;   //change2
		file=trim(file);
		flag=1;
		printf("file : %s output : %s \n",*argv,output);
		int old_stdout = dup(1);
		//printf("here");
  		FILE *fp1= freopen(output,"a+",stdout);
		//printf("here");
		execute_file(argv,file);
		fclose(stdout);
		 FILE *fp2 = fdopen(old_stdout, "w");
		 *stdout = *fp2;	
    		exit(0);
		}    
	if(strstr(output,"<")>0)
           {
		//printf("here \n");
		
		
		char *p=strtok_r(output ,"<", &file);
		file=trim(file);
		flag=1;
		int fd = open(file, O_RDONLY);
		if(fd<0){
			printf("File does not exist");
			exit(0);
			}	
    		//exit(0);
		}  
	if(strstr(output,"|")>0)
        {
		fflush(stdout);printf("here");fflush(stdout);
		char *p=strtok_r(output ,"|", &file);
		file=trim(file);
		flag=1;
		//fflush(stdout);printf("%s",file);fflush(stdout);
		
			//fflush(stdout);printf("%s",file);	fflush(stdout);
			char *args[64];		        		
			parse(file,args);
			execute(args);
		
			
    		//exit(0);
		}     	 
	int old_stdout = dup(1);
  	FILE *fp1= freopen(output,"a+",stdout);
          if(execvp(argv[0],argv)<0)//{
            	printf("**error in exec");
	    fclose(stdout);
     FILE *fp2 = fdopen(old_stdout, "w");
     *stdout = *fp2;
	exit(0);
          }
     
     else 
     {  
          while (wait(&status) != pid)  ;    
     }
	
}
void  execute_input(char **argv,char *output)
{
  pid_t  pid;
	int fd;
	char *file;int flag=0;
  int    status;
   if ((pid = fork()) < 0)
    {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {      
	if(strstr(output,"<")>0)

        {
 		 char *p=strtok_r(output ,"<", &file);
		file=trim(file);
		flag=1;
		//printf("file : %s output : %s \n",file,output);
		  fd = open(output, O_RDONLY);
		if(fd<0){
			printf("File does not exist");
			exit(0);
			}
  		
		output=file;
		}
	if(strstr(output,">")>0)
        {
 		 char *p=strtok_r(output ,">", &file);
		file=trim(file);
		flag=1;
		fflush(stdout);printf("file : %s output : %s \n",file,output);fflush(stdout);
		int old_stdout = dup(1);
		
  		FILE *fp1= freopen(file,"a+",stdout);
		
		execute_input(argv,output);
		fclose(stdout);
		 FILE *fp2 = fdopen(old_stdout, "w");
		 *stdout = *fp2;	
    		exit(0);		 
  		
	//	output=file;
		}
if(strstr(output,"|")>0)
 {
 		 char *p=strtok_r(output ,"|", &file);
		file=trim(file);
		flag=1;
		char *args[64];		        		
			parse(file,args);
			int pfds[2];
    pid_t pid,pid2;
    int status,status2;
    pipe(pfds);
	int fl=0;
	if ((pid = fork()) < 0)
       {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
	if ((pid2 = fork()) < 0 ) 
     {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
    if (pid==0 && pid2!=0) 
    {
        close(1);     
        dup(pfds[1]);  
        close(pfds[0]); 
	close(pfds[1]);
	fd = open(output, O_RDONLY);
  			close(0);
  			dup(fd);
        if(execvp(argv[0],argv)<0){
		close(pfds[0]);
		close(pfds[1]);
    	    printf("**error in exec");
		fl=1;	exit(0);
    	}
	close (fd);
        exit(0);
     } 
	else if(pid2==0 && pid !=0 && fl!=1)
      {
	
        close(0);       
        dup(pfds[0]);  
        close(pfds[1]);
	close(pfds[0]);
        if(execvp(args[0],args)<0)
        {
		close(pfds[0]);
		close(pfds[1]);
    	    
   	    printf("**error in exec");
	exit(0);
   	 }
	}
	else
	{
		close(pfds[0]);
		close(pfds[1]);
		while (wait(&status) != pid)  ;
		while (wait(&status2) != pid2)  ;
			}
	exit(0);   
	 }		 

	   fd = open(output, O_RDONLY);
  			close(0);
  			dup(fd);

        if(execvp(argv[0],argv)<0){
    	    printf("**error in exec");
    	}
        close (fd);
        exit(0);
      }
     else 
     {  
          while (wait(&status) != pid)  ;    
     }
    
}	
void execute_pipe(char **argv,char *output)
{

    int pfds[2],pf[2],flag;
	char *file;
    pid_t pid,pid2,pid3;
    int status,status2,old_stdout;
    pipe(pfds);int blah=0;
	char *args[64];
	char *argp[64];
	int fl=0;
	if ((pid = fork()) < 0)
       {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
	if ((pid2 = fork()) < 0 )
       {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
 
    if (pid==0 && pid2!=0)
    {
	
        close(1);     
        dup(pfds[1]);  
        close(pfds[0]); 
	close(pfds[1]);
        if(execvp(argv[0],argv)<0){
		close(pfds[0]);
		close(pfds[1]);
    	    printf("**error in exec here");
			fl=1; kill(pid2, SIGUSR1);exit(0);
		
    	}
    } 
	else if(pid2==0 && pid !=0){
		if(fl==1){exit(0);}
	if(strstr(output,"<")>0){
 		 char *p=strtok_r(output ,"<", &file);
		file=trim(file);
		flag=1;
		parse(output,args);
		execute_input(args,file);
		close(pfds[0]);
		close(pfds[1]);
  		exit(0); 
		}
	if(strstr(output,">")>0){
 		 char *p=strtok_r(output ,">", &file);
		file=trim(file);
		flag=1;
		//fflush(stdout);printf("file : %s output : %s \n",file,output);fflush(stdout);
		parse(output,args);
		blah =1;
		}
	
	else{
		//printf("here");
		parse(output,args);
		}
	//if(blah!=1){
        close(0);       
        dup(pfds[0]);  
        close(pfds[1]);
	close(pfds[0]);
	if(blah==1){ old_stdout = dup(1);
  		FILE *fp1= freopen(file,"a+",stdout);}
		
        if(execvp(args[0],args)<0){
		fflush(stdout);	
   	    printf("**error in exec %d",pid);
	kill (pid, SIGUSR1);
		close(pfds[0]);
		close(pfds[1]);
	 
  	//exit(0);
   	}
	fflush(stdout);
	printf("HERE");	
		//kill (pid, SIGUSR1);
	if(blah==1){
		    fclose(stdout);
     FILE *fp2 = fdopen(old_stdout, "w");
     *stdout = *fp2;}
//	exit(0);
	}
	//char X;
	//X:
	else{
	//	char X;
	//X:

		close(pfds[0]);
		close(pfds[1]);
		while (wait(&status) != pid)  ;
		while (wait(&status2) != pid2)  ;
			}
}
void execute_pipe2(char **argv,char **args,char **argp)
{

 int status;
  int i;


  int pipes[4];
  pipe(pipes); 
  pipe(pipes + 2); 
 
  
  if (fork() == 0)
    {
     

      dup2(pipes[1], 1);


      close(pipes[0]);
      close(pipes[1]);
      close(pipes[2]);
      close(pipes[3]);

      if(execvp(argv[0],argv)<0){
	
   	   		fflush(stdout);
   	    printf("**error in exec");
		fflush(stdout);
		 close(pipes[0]);
  		close(pipes[1]);
  		close(pipes[2]);
 		 close(pipes[3]);
		exit(1);
	}
    }
  else
    {

      if (fork() == 0)
	{
	  
	  dup2(pipes[0], 0);


	  dup2(pipes[3], 1);


	  close(pipes[0]);
	  close(pipes[1]);
	  close(pipes[2]);
	  close(pipes[3]);

	 
      if(execvp(args[0],args)<0){
	
   	  		fflush(stdout);
   	    printf("**error in exec");
		fflush(stdout);
		 close(pipes[0]);
  		close(pipes[1]);
  		close(pipes[2]);
 		 close(pipes[3]);
		exit(1);
	}
	}
      else
	{

	  if (fork() == 0)
	    {

	      dup2(pipes[2], 0);


	      close(pipes[0]);
	      close(pipes[1]);
	      close(pipes[2]);
	      close(pipes[3]);

	      
      if(execvp(argp[0],argp)<0){
		fflush(stdout);
   	    printf("**error in exec");
		fflush(stdout);
		 close(pipes[0]);
  		close(pipes[1]);
  		close(pipes[2]);
 		 close(pipes[3]);
		exit(1);
	}
	    }
	}
    }
      
  
  close(pipes[0]);
  close(pipes[1]);
  close(pipes[2]);
  close(pipes[3]);

  for (i = 0; i < 3; i++)
    wait(&status);

}

void  main(void)
{
     char line[1024];
     char *argv[64];
     char *args[64];
     char *left;
     size_t size=0;
     char ch;
     int count=0;
     char *tri;
     char *second;
     char *file;
     int i;
     for(i = 0; i< hist_size ;i++)
     {
		hist[i] = (char *)malloc(150);
     }

    while (1)
    {
        count=0;
        int flag=0;
        char *word=NULL;
        char *abc[]={"pwd",NULL};
        //parse(abc,argv);
        fflush(stdout);
        printf("\n\nOurShell -> ");
        fflush(stdout);
        execute(abc); //prints pwd
        printf("$");
        //getchar();
        int len=getline(&word,&size, stdin);
        word[len-1]='\0';
        char *file =NULL;
        int i=0;
        //printf("0");
        char *temp = (char *)malloc(150);
        strcpy(temp,word);
        parse(temp,argv);
        //printf("SEEEE HERE %s",argv[0]);

        //fflush(stdout);
        if( strcmp (argv[0],"history") == 0)
        {
            //printf("2");
            //fflush(stdout);
        }
        else
        {
            strcpy(hist[(head + 1) % hist_size], word);   //storing an entry in history
            head = (head+ 1) % hist_size;
            filled = filled + 1;//printf("3");
            //printf("is it printing? %s",hist[1]);
        }
        for(i=0;word[i]!='\0';i++)
        {

            if(word[i]=='>')
            {
                //	printf("!%s\n",word);   //has the initial command
                char *p=strtok_r(word ,">", &file);
                file=trim(file);
                //printf("1file : %s\n",file);
                //printf("3%s\n",p);
                flag=1;
                break;
            }
            else if(word[i]=='<')
            {
                char *p=strtok_r(word ,"<", &file);
                file=trim(file);
                //printf("file : %s",file);
                flag=2;
                break;
            }
            else if(word[i]=='|')
            {
                char *p=strtok_r(word ,"|", &left);
                flag=3;

                break;
            }
        }
        if(strcmp(word,"\0")==0)   //empty command
		{
		    continue;
        }
        else if (strcmp(word, "exit") == 0)  //exit
        {
             exit(0);
        }
        /*if(strcmp(word,"\0")==0)
		{
            continue;
        }
        else if (strcmp(word, "exit") == 0)
        {
             exit(0);
        }*/

        if(flag==1)
        {
            parse(word,argv);   //parsed command stored in argv
            //printf("At the right place");
            execute_file(argv,file);
        }
        else if(flag==2)
        {
            parse(word,argv);
            execute_input(argv,file);
        }
        else if(flag==3)
        {
            char *argp[64];
            char *output,*file;
            if(strstr(left,"|")>0)
            {
                char *p=strtok_r(left ,"|", &file);
                //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                parse(word,argv);
                parse(left,args);
                parse(file,argp);
                execute_pipe2(argv,args,argp);
                //blah =1;
            }
            else
            {
                //if(strlen(word)
                parse(word,argv);
                execute_pipe(argv,left);
                //	exit(0);
                //break;
            }
        }
        else

        {
            parse(word,argv);

             if(strcmp(argv[0],"history") == 0)
            {     //printf("eeeeeeeeeee\n%s\n%s\n",word,argv[0],argv[1]);
                int position = atoi(argv[1]);
                if(filled < hist_size && position >= head)
                {
                    printf("Command Doesn't exist.\n");

                }
                else if(position  > hist_size -1)
                {
                    printf("History buffer exceeded.\n");
                }
                /*else if(hist[(hist_size + head - position)%hist_size] == NULL)
                {
                    printf("Error.\n");
                }*/

                else
                {
                    //fflush(stdout);
                    char *temp= (char *)malloc(150);
		    char *temp2 = (char *)malloc(150);
                    strcpy(temp,hist[(hist_size + head - position)%hist_size]);
			strcpy(temp2,hist[(hist_size + head - position)%hist_size]);
			parse(temp2,argv);
                    for(i=0;temp[i]!='\0';i++)
                    {

                        if(temp[i]=='>')
                        {
                            //	printf("!%s\n",word);   //has the initial command
                            char *p=strtok_r(temp ,">", &file);
                            file=trim(file);
                            //printf("1file : %s\n",file);
                            //printf("3%s\n",p);
                            flag=4;
                            break;
                        }
                        else if(temp[i]=='<')
                        {
                            char *p=strtok_r(temp ,"<", &file);
                            file=trim(file);
                            //printf("file : %s",file);
                            flag=5;
                            break;
                        }
                        else if(temp[i]=='|')
                        {
                            char *p=strtok_r(temp ,"|", &left);
                            flag=6;

                            break;
                        }
                    }
                    if(strcmp(temp,"\0")==0)   //empty command
                    {
                        continue;
                    }
                    else if (strcmp(temp, "exit") == 0)  //exit
                    {
                            exit(0);
                    }
                    if(flag==4)
                    {
                        parse(temp,argv);   //parsed command stored in argv
                        //printf("At the right place");
                        execute_file(argv,file);
                    }
                    else if(flag==5)
                    {
                        parse(temp,argv);
                        execute_input(argv,file);
                    }
                    else if(flag==6)
                    {
                        char *argp[64];
                        char *output,*file;
                        if(strstr(left,"|")>0)
                        {
                            char *p=strtok_r(left ,"|", &file);
                            //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                            parse(temp,argv);
                            parse(left,args);
                            parse(file,argp);
                            execute_pipe2(argv,args,argp);
                            //blah =1;
                        }
                        else
                        {
                            //if(strlen(word)
                            parse(temp,argv);
                            execute_pipe(argv,left);
                            //	exit(0);
                            //break;
                        }
                    }
                  
            
            else if(strcmp(argv[0],"t_display") == 0)   //get a list of threads for the processes. When a process hangs, we might need 									//to identify the list of threads running for a particular process
	    {
		if(argv[1] != NULL)
		{
			printf("This command requires no arguements");
		}
		else
		{
                    strcpy(word,"ps -C java -L -o pid,tid,pcpu,state,nlwp,args");
		    parse(word,argv);
		    execute(argv);
		}

	    }
	    else if(strcmp(argv[0], "p_tree") == 0) //display the process Id and commands in a hierarchy
	    {
		 if(argv[1] != NULL)
		{
			printf("This command requires no arguements");
		}
		else
		{
                    strcpy(word,"ps -e -o pid,args --forest");
		    parse(word,argv);
		    execute(argv);
		}
	    }
	    else if(strcmp(argv[0], "p_time") == 0) 
	    {
		 if(argv[1] == NULL)
		{
			printf("This command requires arguements");
		}
		else
		{
		    //printf("kwerc");
		    //fflush(stdout);
		    char *use = (char *)malloc(150);
                    strcpy(use,"ps -p ");
		    strcat(use,argv[1]);
		    strcat(use," -o pid,etime=");
		    //	(strcat("ps -p ",argv[1])));
		    //  (strcat(use," -o pid,etime=")) ;
		    //printf("%s",use);
	            //fflush(stdout);
		    parse(use,argv);
		    execute(argv);
		}
	    }
	    else if(strcmp(argv[0], "mem_hog") == 0) 
	    {
		 if(argv[1] != NULL)
		{
			printf("This command doesn't require arguements");
		}
		else
		{
		    strcpy(temp,"ps aux | sort -nk +4 | tail");
		    for(i=0;temp[i]!='\0';i++)
                    {

                        if(temp[i]=='>')
                        {
                            //	printf("!%s\n",word);   //has the initial command
                            char *p=strtok_r(temp ,">", &file);
                            file=trim(file);
                            //printf("1file : %s\n",file);
                            //printf("3%s\n",p);
                            flag=4;
                            break;
                        }
                        else if(temp[i]=='<')
                        {
                            char *p=strtok_r(temp ,"<", &file);
                            file=trim(file);
                            //printf("file : %s",file);
                            flag=5;
                            break;
                        }
                        else if(temp[i]=='|')
                        {
                            char *p=strtok_r(temp ,"|", &left);
                            flag=6;

                            break;
                        }
                    }
                    if(strcmp(temp,"\0")==0)   //empty command
                    {
                        continue;
                    }
                    else if (strcmp(temp, "exit") == 0)  //exit
                    {
                            exit(0);
                    }
                    if(flag==4)
                    {
                        parse(temp,argv);   //parsed command stored in argv
                        //printf("At the right place");
                        execute_file(argv,file);
                    }
                    else if(flag==5)
                    {
                        parse(temp,argv);
                        execute_input(argv,file);
                    }
                    else if(flag==6)
                    {
                        char *argp[64];
                        char *output,*file;
                        if(strstr(left,"|")>0)
                        {
                            char *p=strtok_r(left ,"|", &file);
                            //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                            parse(temp,argv);
                            parse(left,args);
                            parse(file,argp);
                            execute_pipe2(argv,args,argp);
                            //blah =1;
                        }
                        else
                        {
                            //if(strlen(word)
                            parse(temp,argv);
                            execute_pipe(argv,left);
                            //	exit(0);
                            //break;
                        }
                    }
                    else
                    {
                        if(f==1)
                        {
                            f=0;
                            continue;
                        }
			parse(temp,argv);
                        execute(argv);
                    }
		}
	    }
	    else if(strcmp(argv[0], "hist_freq") == 0) 
	    {
		 if(argv[1] != NULL)
		{
			printf("This command doesn't require arguements");
		}
		else
		{
		    strcpy(temp,"history | awk ‘{a[$2]++}END{for(i in a){print a[i] ” ” i}}’ | sort -rn");
		    for(i=0;temp[i]!='\0';i++)
                    {

                        if(temp[i]=='>')
                        {
                            //	printf("!%s\n",word);   //has the initial command
                            char *p=strtok_r(temp ,">", &file);
                            file=trim(file);
                            //printf("1file : %s\n",file);
                            //printf("3%s\n",p);
                            flag=4;
                            break;
                        }
                        else if(temp[i]=='<')
                        {
                            char *p=strtok_r(temp ,"<", &file);
                            file=trim(file);
                            //printf("file : %s",file);
                            flag=5;
                            break;
                        }
                        else if(temp[i]=='|')
                        {
                            char *p=strtok_r(temp ,"|", &left);
                            flag=6;

                            break;
                        }
                    }
                    if(strcmp(temp,"\0")==0)   //empty command
                    {
                        continue;
                    }
                    else if (strcmp(temp, "exit") == 0)  //exit
                    {
                            exit(0);
                    }
                    if(flag==4)
                    {
                        parse(temp,argv);   //parsed command stored in argv
                        //printf("At the right place");
                        execute_file(argv,file);
                    }
                    else if(flag==5)
                    {
                        parse(temp,argv);
                        execute_input(argv,file);
                    }
                    else if(flag==6)
                    {
                        char *argp[64];
                        char *output,*file;
                        if(strstr(left,"|")>0)
                        {
                            char *p=strtok_r(left ,"|", &file);
                            //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                            parse(temp,argv);
                            parse(left,args);
                            parse(file,argp);
                            execute_pipe2(argv,args,argp);
                            //blah =1;
                        }
                        else
                        {
                            //if(strlen(word)
                            parse(temp,argv);
                            execute_pipe(argv,left);
                            //	exit(0);
                            //break;
                        }
                    }
                    else
                    {
                        if(f==1)
                        {
                            f=0;
                            continue;
                        }
			parse(temp,argv);
                        execute(argv);
                    }
		}
	    }
	    else if(strcmp(argv[0], "local_ws") == 0) 
	    {
		 if(argv[1] != NULL)
		{
			printf("This command requires no arguements");
		}
		else
		{
                    strcpy(word,"python -m SimpleHTTPServer");
		    parse(word,argv);
		    execute(argv);
		}
	    }
	    else if(strcmp(argv[0], "hide_id") == 0) 
	    {
		 if(argv[1] == NULL)
		{
			printf("This command requires arguement");
		}
		else
		{
                    strcpy(temp,"echo ");
		    strcat(temp,argv[1]);
		    strcat(temp," | tr a-z@. n-za-m.@");
		    for(i=0;temp[i]!='\0';i++)
                    {

                        if(temp[i]=='>')
                        {
                            //	printf("!%s\n",word);   //has the initial command
                            char *p=strtok_r(temp ,">", &file);
                            file=trim(file);
                            //printf("1file : %s\n",file);
                            //printf("3%s\n",p);
                            flag=4;
                            break;
                        }
                        else if(temp[i]=='<')
                        {
                            char *p=strtok_r(temp ,"<", &file);
                            file=trim(file);
                            //printf("file : %s",file);
                            flag=5;
                            break;
                        }
                        else if(temp[i]=='|')
                        {
                            char *p=strtok_r(temp ,"|", &left);
                            flag=6;

                            break;
                        }
                    }
                    if(strcmp(temp,"\0")==0)   //empty command
                    {
                        continue;
                    }
                    else if (strcmp(temp, "exit") == 0)  //exit
                    {
                            exit(0);
                    }
                    if(flag==4)
                    {
                        parse(temp,argv);   //parsed command stored in argv
                        //printf("At the right place");
                        execute_file(argv,file);
                    }
                    else if(flag==5)
                    {
                        parse(temp,argv);
                        execute_input(argv,file);
                    }
                    else if(flag==6)
                    {
                        char *argp[64];
                        char *output,*file;
                        if(strstr(left,"|")>0)
                        {
                            char *p=strtok_r(left ,"|", &file);
                            //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                            parse(temp,argv);
                            parse(left,args);

                            parse(file,argp);
                            execute_pipe2(argv,args,argp);
                            //blah =1;
                        }
                        else
                        {
                            //if(strlen(word)
                            parse(temp,argv);
                            execute_pipe(argv,left);
                            //	exit(0);
                            //break;
                        }
                    }
                    else
                    {
                        if(f==1)
                        {
                            f=0;
                            continue;
                        }
			parse(temp,argv);
                        execute(argv);
                    }
		}
	    }
           
	    else if(strcmp(argv[0], "man_x") == 0) //display the process Id and commands in a hierarchy
	    {
		 if(argv[1] != NULL)
		{
		    printf("\nThe following ten commands have been explicitly implemented in our shell\n\n");
		    printf("t_display  :  Returns a list of threads for the processes. When\n\t\ta process hangs, we might need to identify the list\n\t\tof threads running for a particular process\n\n");
		    printf("p_tree     :  Returns the process Id and commands in a hierarchy.\n\t\t From this tree, we can identify which is the parent\n\t\t process and the child processes it forked in a recursive manner.\n\n");
		    printf("p_time x   :  Returns the elapsed time for the process/processes. \n\t\tThe elapsed time is in the form [[dd-]hh:]mm:ss.\n\t\t'x' can be a single process id or a list of process ids.\n\n");
		    printf("history x  :  Executes the xth most recently used command. 'history 0'\n\t\t will execute the last command you typed. History\n\t\t commands are not recorded in history.\n\n");
                    printf("mem_hog    :  Returns the 10 most resource hogging processes which are\n\t\tconsuming big share of the memory.\n\n");
		    printf("hist_freq  :  Returns the commands in history with their frequency so\n\t\tthat you know the most frequently and least\n\t\tfrequently used commands.\n\n");
		    printf("hide_id x  :  Convert an e-mail address x in a way that there is no\n\t\tscope of it being misused by spammers. This commands\n\t\t returns an encrypted emaid-id. This id when used again\n\t\t in the command returns the actual id.\n\n");
	            printf("local_ws   :  Start a basic HTTP web server on port 8000 of the system.\n\t\tCan be used to share a file or folder from current\n\t\t directory in the local network by http://localhost:8000/.\n\n");
		}
		else
		{
		    printf("\nThe following ten commands have been explicitly implemented in our shell\n\n");
		    printf("t_display  :  Returns a list of threads for the processes. When\n\t\ta process hangs, we might need to identify the list\n\t\tof threads running for a particular process\n\n");
		    printf("p_tree     :  Returns the process Id and commands in a hierarchy.\n\t\t From this tree, we can identify which is the parent\n\t\t process and the child processes it forked in a recursive manner.\n\n");
		    printf("p_time x   :  Returns the elapsed time for the process/processes. \n\t\tThe elapsed time is in the form [[dd-]hh:]mm:ss.\n\t\t'x' can be a single process id or a list of process ids.\n\n");
		    printf("history x  :  Executes the xth most recently used command. 'history 0'\n\t\t will execute the last command you typed. History\n\t\t commands are not recorded in history.\n\n");
		    printf("mem_hog    :  Returns the 10 most resource hogging processes which are\n\t\tconsuming big share of the memory.\n\n");
		    printf("hist_freq  :  Returns the commands in history with their frequency so\n\t\tthat you know the most frequently and least\n\t\tfrequently used commands.\n\n");
		
		    printf("hide_id x  :  Convert an e-mail address x in a way that there is no\n\t\tscope of it being misused by spammers. This commands\n\t\t returns an encrypted emaid-id. This id when used again\n\t\t in the command returns the actual id.\n\n");
		    printf("local_ws   :  Start a basic HTTP web server on port 8000 of the system.\n\t\tCan be used to share a file or folder from current\n\t\t directory in the local network by http://localhost:8000/.\n\n");
	    }
            }
		else if(f==1)
            {
                f=0;
                continue;
            }
            else
            {
                execute(argv);
            }
            } }
//////////*****************HISTORY ENDS*****************////////////////////////////////////////
	    else if(strcmp(argv[0],"t_display") == 0)   //get a list of threads for the processes. When a process hangs, we might need 									//to identify the list of threads running for a particular process
	    {
		if(argv[1] != NULL)
		{
			printf("This command requires no arguements");
		}
		else
		{
                    strcpy(word,"ps -C java -L -o pid,tid,pcpu,state,nlwp,args");
		    parse(word,argv);
		    execute(argv);
		}

	    }
	    else if(strcmp(argv[0], "p_tree") == 0) //display the process Id and commands in a hierarchy
	    {
		 if(argv[1] != NULL)
		{
			printf("This command requires no arguements");
		}
		else
		{
                    strcpy(word,"ps -e -o pid,args --forest");
		    parse(word,argv);
		    execute(argv);
		}
	    }
	    else if(strcmp(argv[0], "p_time") == 0) 
	    {
		 if(argv[1] == NULL)
		{
			printf("This command requires arguements");
		}
		else
		{
		    //printf("kwerc");
		    //fflush(stdout);
		    char *use = (char *)malloc(150);
                    strcpy(use,"ps -p ");
		    strcat(use,argv[1]);
		    strcat(use," -o pid,etime=");
		    //	(strcat("ps -p ",argv[1])));
		    //  (strcat(use," -o pid,etime=")) ;
		    //printf("%s",use);
	            //fflush(stdout);
		    parse(use,argv);
		    execute(argv);
		}
	    }
	    else if(strcmp(argv[0], "mem_hog") == 0) 
	    {
		 if(argv[1] != NULL)
		{
			printf("This command doesn't require arguements");
		}
		else
		{
		    strcpy(temp,"ps aux | sort -nk +4 | tail");
		    for(i=0;temp[i]!='\0';i++)
                    {

                        if(temp[i]=='>')
                        {
                            //	printf("!%s\n",word);   //has the initial command
                            char *p=strtok_r(temp ,">", &file);
                            file=trim(file);
                            //printf("1file : %s\n",file);
                            //printf("3%s\n",p);
                            flag=4;
                            break;
                        }
                        else if(temp[i]=='<')
                        {
                            char *p=strtok_r(temp ,"<", &file);
                            file=trim(file);
                            //printf("file : %s",file);
                            flag=5;
                            break;
                        }
                        else if(temp[i]=='|')
                        {
                            char *p=strtok_r(temp ,"|", &left);
                            flag=6;

                            break;
                        }
                    }
                    if(strcmp(temp,"\0")==0)   //empty command
                    {
                        continue;
                    }
                    else if (strcmp(temp, "exit") == 0)  //exit
                    {
                            exit(0);
                    }
                    if(flag==4)
                    {
                        parse(temp,argv);   //parsed command stored in argv
                        //printf("At the right place");
                        execute_file(argv,file);
                    }
                    else if(flag==5)
                    {
                        parse(temp,argv);
                        execute_input(argv,file);
                    }
                    else if(flag==6)
                    {
                        char *argp[64];
                        char *output,*file;
                        if(strstr(left,"|")>0)
                        {
                            char *p=strtok_r(left ,"|", &file);
                            //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                            parse(temp,argv);
                            parse(left,args);
                            parse(file,argp);
                            execute_pipe2(argv,args,argp);
                            //blah =1;
                        }
                        else
                        {
                            //if(strlen(word)
                            parse(temp,argv);
                            execute_pipe(argv,left);
                            //	exit(0);
                            //break;
                        }
                    }
                    else
                    {
                        if(f==1)
                        {
                            f=0;
                            continue;
                        }
			parse(temp,argv);
                        execute(argv);
                    }
		}
	    }
	    else if(strcmp(argv[0], "hist_freq") == 0) 
	    {
		 if(argv[1] != NULL)
		{
			printf("This command doesn't require arguements");
		}
		else
		{
		    strcpy(temp,"history | awk ‘{a[$2]++}END{for(i in a){print a[i] ” ” i}}’ | sort -rn");
		    for(i=0;temp[i]!='\0';i++)
                    {

                        if(temp[i]=='>')
                        {
                            //	printf("!%s\n",word);   //has the initial command
                            char *p=strtok_r(temp ,">", &file);
                            file=trim(file);
                            //printf("1file : %s\n",file);
                            //printf("3%s\n",p);
                            flag=4;
                            break;
                        }
                        else if(temp[i]=='<')
                        {
                            char *p=strtok_r(temp ,"<", &file);
                            file=trim(file);
                            //printf("file : %s",file);
                            flag=5;
                            break;
                        }
                        else if(temp[i]=='|')
                        {
                            char *p=strtok_r(temp ,"|", &left);
                            flag=6;

                            break;
                        }
                    }
                    if(strcmp(temp,"\0")==0)   //empty command
                    {
                        continue;
                    }
                    else if (strcmp(temp, "exit") == 0)  //exit
                    {
                            exit(0);
                    }
                    if(flag==4)
                    {
                        parse(temp,argv);   //parsed command stored in argv
                        //printf("At the right place");
                        execute_file(argv,file);
                    }
                    else if(flag==5)
                    {
                        parse(temp,argv);
                        execute_input(argv,file);
                    }
                    else if(flag==6)
                    {
                        char *argp[64];
                        char *output,*file;
                        if(strstr(left,"|")>0)
                        {
                            char *p=strtok_r(left ,"|", &file);
                            //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                            parse(temp,argv);
                            parse(left,args);
                            parse(file,argp);
                            execute_pipe2(argv,args,argp);
                            //blah =1;
                        }
                        else
                        {
                            //if(strlen(word)
                            parse(temp,argv);
                            execute_pipe(argv,left);
                            //	exit(0);
                            //break;
                        }
                    }
                    else
                    {
                        if(f==1)
                        {
                            f=0;
                            continue;
                        }
			parse(temp,argv);
                        execute(argv);
                    }
		}
	    }
	    else if(strcmp(argv[0], "local_ws") == 0) 
	    {
		 if(argv[1] != NULL)
		{
			printf("This command requires no arguements");
		}
		else
		{
                    strcpy(word,"python -m SimpleHTTPServer");
		    parse(word,argv);
		    execute(argv);
		}
	    }
	    else if(strcmp(argv[0], "hide_id") == 0) 
	    {
		 if(argv[1] == NULL)
		{
			printf("This command requires arguement");
		}
		else
		{
                    strcpy(temp,"echo ");
		    strcat(temp,argv[1]);
		    strcat(temp," | tr a-z@. n-za-m.@");
		    for(i=0;temp[i]!='\0';i++)
                    {

                        if(temp[i]=='>')
                        {
                            //	printf("!%s\n",word);   //has the initial command
                            char *p=strtok_r(temp ,">", &file);
                            file=trim(file);
                            //printf("1file : %s\n",file);
                            //printf("3%s\n",p);
                            flag=4;
                            break;
                        }
                        else if(temp[i]=='<')
                        {
                            char *p=strtok_r(temp ,"<", &file);
                            file=trim(file);
                            //printf("file : %s",file);
                            flag=5;
                            break;
                        }
                        else if(temp[i]=='|')
                        {
                            char *p=strtok_r(temp ,"|", &left);
                            flag=6;

                            break;
                        }
                    }
                    if(strcmp(temp,"\0")==0)   //empty command
                    {
                        continue;
                    }
                    else if (strcmp(temp, "exit") == 0)  //exit
                    {
                            exit(0);
                    }
                    if(flag==4)
                    {
                        parse(temp,argv);   //parsed command stored in argv
                        //printf("At the right place");
                        execute_file(argv,file);
                    }
                    else if(flag==5)
                    {
                        parse(temp,argv);
                        execute_input(argv,file);
                    }
                    else if(flag==6)
                    {
                        char *argp[64];
                        char *output,*file;
                        if(strstr(left,"|")>0)
                        {
                            char *p=strtok_r(left ,"|", &file);
                            //fflush(stdout);printf("file : %s output : %s\n",file,left);fflush(stdout);
                            parse(temp,argv);
                            parse(left,args);

                            parse(file,argp);
                            execute_pipe2(argv,args,argp);
                            //blah =1;
                        }
                        else
                        {
                            //if(strlen(word)
                            parse(temp,argv);
                            execute_pipe(argv,left);
                            //	exit(0);
                            //break;
                        }
                    }
                    else
                    {
                        if(f==1)
                        {
                            f=0;
                            continue;
                        }
			parse(temp,argv);
                        execute(argv);
                    }
		}
	    }
	    else if(strcmp(argv[0], "man_x") == 0) //display the process Id and commands in a hierarchy
	    {
		 if(argv[1] != NULL)
		{
		    printf("\nThe following ten commands have been explicitly implemented in our shell\n\n");
		    printf("t_display  :  Returns a list of threads for the processes. When\n\t\ta process hangs, we might need to identify the list\n\t\tof threads running for a particular process\n\n");
		    printf("p_tree     :  Returns the process Id and commands in a hierarchy.\n\t\t From this tree, we can identify which is the parent\n\t\t process and the child processes it forked in a recursive manner.\n\n");
		    printf("p_time x   :  Returns the elapsed time for the process/processes. \n\t\tThe elapsed time is in the form [[dd-]hh:]mm:ss.\n\t\t'x' can be a single process id or a list of process ids.\n\n");
		    printf("history x  :  Executes the xth most recently used command. 'history 0'\n\t\t will execute the last command you typed. History\n\t\t commands are not recorded in history.\n\n");
                    printf("mem_hog    :  Returns the 10 most resource hogging processes which are\n\t\tconsuming big share of the memory.\n\n");
		    printf("hist_freq  :  Returns the commands in history with their frequency so\n\t\tthat you know the most frequently and least\n\t\tfrequently used commands.\n\n");
		    printf("hide_id x  :  Convert an e-mail address x in a way that there is no\n\t\tscope of it being misused by spammers. This commands\n\t\t returns an encrypted emaid-id. This id when used again\n\t\t in the command returns the actual id.\n\n");
	            printf("local_ws   :  Start a basic HTTP web server on port 8000 of the system.\n\t\tCan be used to share a file or folder from current\n\t\t directory in the local network by http://localhost:8000/.\n\n");
		}
		else
		{
		    printf("\nThe following ten commands have been explicitly implemented in our shell\n\n");
		    printf("t_display  :  Returns a list of threads for the processes. When\n\t\ta process hangs, we might need to identify the list\n\t\tof threads running for a particular process\n\n");
		    printf("p_tree     :  Returns the process Id and commands in a hierarchy.\n\t\t From this tree, we can identify which is the parent\n\t\t process and the child processes it forked in a recursive manner.\n\n");
		    printf("p_time x   :  Returns the elapsed time for the process/processes. \n\t\tThe elapsed time is in the form [[dd-]hh:]mm:ss.\n\t\t'x' can be a single process id or a list of process ids.\n\n");
		    printf("history x  :  Executes the xth most recently used command. 'history 0'\n\t\t will execute the last command you typed. History\n\t\t commands are not recorded in history.\n\n");
		    printf("mem_hog    :  Returns the 10 most resource hogging processes which are\n\t\tconsuming big share of the memory.\n\n");
		    printf("hist_freq  :  Returns the commands in history with their frequency so\n\t\tthat you know the most frequently and least\n\t\tfrequently used commands.\n\n");
		}
		    printf("hide_id x  :  Convert an e-mail address x in a way that there is no\n\t\tscope of it being misused by spammers. This commands\n\t\t returns an encrypted emaid-id. This id when used again\n\t\t in the command returns the actual id.\n\n");
		    printf("local_ws   :  Start a basic HTTP web server on port 8000 of the system.\n\t\tCan be used to share a file or folder from current\n\t\t directory in the local network by http://localhost:8000/.\n\n");
	    }
            else if(f==1)
            {
                f=0;
                continue;
            }
            else
            {
                execute(argv);
            }
        }
  }

}

