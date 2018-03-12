#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hashtable.h"

void cp(char* source_path,char* dest_path);
void ls(char* source_path);
void cd(char* dest_path);
void makedir(char* dest_path);
void cat(char* source_path);
void echo(char* buf,int append,char* dest_path);
void vi(char* source_path);
void rmv(char* source_path);
void du(char* soure_path);

char cmdline[1024];
char cmdlinecopy[1024];
char cmd[1024];	
char cwd[PATH_MAX];
int tree_depth;

int main(int argc, char *argv[])
{
	
	load_metadata();

	char* pch;
	strcpy(cwd,"/root");
	ht_insert(cwd,0);
			
	char echo_buf[1024];
	char source_path[1024];
	char dest_path[1024];
	int start;
	int end;
	char buf[4026];
	
	int word_count=0;
	while(1)
	{
		printf ("%s$",cwd);
		scanf (" %[^\n]",cmdline);
		strcpy(cmdlinecopy,cmdline);//printf("%s\n", cmdline);
		pch = strtok (cmdline," ");
		word_count=0;
		while (pch != NULL)
		{
			if(word_count==0)
				strcpy(cmd,pch);
			pch = strtok (NULL, " ");
			word_count++;
  		}
  		
  		if(strcmp(cmd,"unmount")==0)
  		{
	  		store_metadata();
	  		exit(0);	
	  	}
  		else if(strcmp(cmd,"clear")==0)
  		{
	  		system("clear");	
	  	}
  		else if(strcmp(cmd,"cp")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>3)//invalid cp cmdline
				printf("Error in command\n");
			else
			{	
				word_count=0;
				while (pch != NULL)//cp source dest
				{
					if(word_count==1)
						strcpy(source_path,pch);
					if(word_count==2)
						strcpy(dest_path,pch);
					pch = strtok (NULL, " ");
					word_count++;
		  		}
		  		cp(source_path,dest_path);
		  	}		
  		}
  		else if(strcmp(cmd,"ls")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>3)//invalid ls cmdline
				printf("Error in command\n");
			else
			{	
				if(word_count==3)//ls -l file
				{
					word_count=0;
					while (pch != NULL)
					{
						if(word_count==2)
							strcpy(source_path,pch);
						pch = strtok (NULL, " ");
						word_count++;
			  		}
			  	}
			  	else if(word_count==2)//ls file
				{
					word_count=0;
					while (pch != NULL)
					{
						if(word_count==1)
							strcpy(source_path,pch);
						pch = strtok (NULL, " ");
						word_count++;
			  		}
			  	}
			  	else if(word_count==1)
			  	{
			  		strcpy(source_path,cwd);
			  	}
		  		ls(source_path);

		  	}		
  		}
  		else if(strcmp(cmd,"cd")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>2)//invalid cd cmdline
				printf("Error in command\n");
			else
			{	
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==1)
						strcpy(dest_path,pch);
					pch = strtok (NULL, " ");
					word_count++;
		  		}
		  		cd(dest_path);

		  	}		
  		}
  		else if(strcmp(cmd,"mkdir")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>2)//invalid mkdir cmdline
				printf("Error in command\n");
			else
			{	
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==1)
						strcpy(dest_path,pch);
					pch = strtok (NULL, " ");
					word_count++;
		  		}
		  		makedir(dest_path);

		  	}		
  		}
  		else if(strcmp(cmd,"cat")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>2)//invalid cat cmdline
				printf("Error in command\n");
			else
			{	
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==1)
						strcpy(source_path,pch);
					pch = strtok (NULL, " ");
					word_count++;
		  		}
		  		cat(source_path);

		  	}		
  		}
  		else if(strcmp(cmd,"echo")==0)
  		{
	  		strcpy(echo_buf,cmdlinecopy);
	  		int append;
			if(word_count>4)//invalid echo cmdline
				printf("Error in command\n");
			else if(word_count==2)
			{
				pch = strtok (cmdlinecopy,"'");
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==1)
						printf("%s\n",pch);
					pch = strtok (NULL, "'");
					word_count++;
			  	}
			}
			else
			{	
				pch = strtok (cmdlinecopy," ");
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==0)
						strcpy(cmd,pch);
					if(word_count==1)
						strcpy(buf,pch);
					if(word_count==2)
					{	
						if(strcmp(pch,">>")==0)
							append=1;
						else
							append=0;
					}		
					if(word_count==3)
						strcpy(dest_path,pch);
					pch = strtok (NULL, " ");
					word_count++;
		  		}
				pch = strtok (echo_buf,"'"); //echo 'hello world' > filename
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==1)
						strcpy(buf,pch);
					pch = strtok (NULL, "'");
					word_count++;
			  	}
				echo(buf,append,dest_path);
				
		  	}		
  		}
  		else if(strcmp(cmd,"vi")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>2)//invalid gedit cmdline
				printf("Error in command\n");
			else
			{	
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==0)
						strcpy(cmd,pch);
					if(word_count==1)
						strcpy(source_path,pch);
					pch = strtok (NULL, " ");
					word_count++;
		  		}
		  		vi(source_path);
		  	}		
  		}
  		else if(strcmp(cmd,"rm")==0 || strcmp(cmd,"rmdir")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>2)//invalid rm/rmdir cmdline
				printf("Error in command\n");
			else
			{	
				word_count=0;
				while (pch != NULL)
				{
					if(word_count==0)
						strcpy(cmd,pch);
					if(word_count==1)
						strcpy(source_path,pch);
					pch = strtok (NULL, " ");
					word_count++;
		  		}
		  		rmv(source_path);
			}		
  		}
  		else if(strcmp(cmd,"du")==0)
  		{
	  		pch = strtok (cmdlinecopy," ");
			if(word_count>3)//invalid du cmdline
				printf("Error in command\n");
			else
			{	
				if(word_count==3)//du -s file
				{
					word_count=0;
					while (pch != NULL)
					{
						if(word_count==2)
							strcpy(source_path,pch);
						pch = strtok (NULL, " ");
						word_count++;
			  		}
			  	}
			  	else if(word_count==2)//du file
				{
					word_count=0;
					while (pch != NULL)
					{
						if(word_count==1)
							strcpy(source_path,pch);
						pch = strtok (NULL, " ");
						word_count++;
			  		}
			  	}
		  		du(source_path);

		  	}		
  		}
	}
}
int get_absolute_path(char* path)
{
    	char copy[PATH_MAX];
	strcpy(copy,path);//copy path for safekeeping
//	printf("path:%s\n",path);
	
	char* pch = strtok(copy,"/");
	int go_up_count=0;
	int word_count=0;
	char buf[100];
	if(strcmp(path,cwd)==0)//cwd
	{
//		printf("99\n");
		return 0;
	}
	while (pch != NULL)//sees how many levels to go up
	{
		strcpy(buf,pch);
		pch = strtok (NULL, "/");
		if(strcmp(buf,"..")==0)
			go_up_count++;
		word_count++;
	}
	if(word_count>0)//if path has / in it
	{
		if(go_up_count>0)
		{
//			printf("50\n");
			
				word_count=0;
				strcpy(copy,cwd);
				char new_cwd[PATH_MAX];//initialise new absolute path
				strcpy(new_cwd,"");
				pch = strtok (copy,"/");
				while (pch != NULL && word_count<go_up_count)//creates new absolute path from cwd by popping cwd dirs
				{
					strcat(new_cwd,"/");
					strcat(new_cwd,pch);
					pch = strtok (NULL, "/");
					word_count++;
				}
				word_count=0;
				strcpy(copy,path);
			    	pch = strtok (copy,"/");
				while (pch != NULL)
				{
					if(go_up_count<=word_count)//append new_cwd with dirs of the relative path
					{	
						strcat(new_cwd,"/");
						strcat(new_cwd,pch);
					}
					pch = strtok (NULL,"/");
					word_count++;
				}
				strcpy(path,new_cwd);			
			
		}
		else if(go_up_count==0 && strcmp(path,cwd)!=0) //child
		{
//			printf("88\n");
			char child[PATH_MAX];//initialise new absolute path
			strcpy(child,cwd);
			strcat(child,"/");
			strcat(child,path);
			strcpy(path,child);
			
		}
	}
	else if((word_count==1 && (strcmp(path,".")==0) || strcmp(path,cwd)==0))//cwd
	{
//		printf("99\n");
		strcpy(path,cwd);
	}
	else if(word_count==1 && strcmp(path,"..")==0)//parent
	{
//		printf("104\n");
		int count=0;
		strcpy(copy,cwd);
		char new_cwd[PATH_MAX];//initialise new absolute path
		strcpy(new_cwd,"");
		pch = strtok (copy,"/");
		while (pch != NULL && count<word_count-1)
		{
			strcat(new_cwd,"/");
			strcat(new_cwd,pch);
			pch = strtok (NULL, "/");
			count++;
		}
		strcpy(path,new_cwd);		
		
	}
	else if(word_count==0)//empty //cwd
	{
//		printf("122\n");
		strcpy(path,cwd);		
	}
//	printf("abs_path:%s\n",path);

}
void cp(char* source_path,char* dest_path)
{
	get_absolute_path(source_path);
	get_absolute_path(dest_path);
	
	if(ht_path_exists(source_path)==1 && ht_path_exists(dest_path)==1)
	{
		
		ht_copy_files(source_path,dest_path);
	}
	else
	{
		printf("File(s) does not exist\n");
	}
}
void ls(char* source_path)
{
	get_absolute_path(source_path);

	if(ht_path_exists(source_path)==1)
	{ 
		ht_display_inode_ls(source_path);
	}
	else
	{
		printf("File does not exist\n");
	}
}
void cd(char* dest_path)
{
	get_absolute_path(dest_path);
	if(ht_path_exists(dest_path)==1)//child exists
	{
		inode_t ht_inode=ht_get_inode_info(dest_path);
		if(ht_inode.file_type==0)
			strcpy(cwd,dest_path);
		else
			printf("Cannot redirect into file\n");
	}
	else//child doesnt exist
	{
		printf("Destination folder/file does not exist\n");
	}
}
void makedir(char* dest_path)
{
	if(strcmp(dest_path,"..")==0 || strcmp(dest_path,".")==0)
	{
		printf("Invalid name\n"); 
	}
	else
	{
		get_absolute_path(dest_path);
		if(ht_path_exists(dest_path)==1)
		{
			printf("File already exists\n");
		}
		else
		{
			ht_insert(dest_path,0);
			strcat(dest_path,"$");
			ht_inode_file_contents_append(dest_path,cwd,1);	
		}
	}
}
void cat(char* source_path)
{
	if(strcmp(source_path,"..")==0 || strcmp(source_path,".")==0)
	{
		printf("Invalid name\n"); 
	}
	else
	{
		get_absolute_path(source_path);
		if(ht_path_exists(source_path)==1)
		{
			ht_display_inode_file_contents(source_path);
		}
		else
		{
			printf("File does not exist\n");
		}
	}
}
void echo(char* buf,int append,char* dest_path)
{
	if(strcmp(dest_path,"..")==0 || strcmp(dest_path,".")==0)
	{
		printf("Invalid name\n"); 
	}
	else
	{
		//printf("buf holds %s\n",buf);
		get_absolute_path(dest_path);
		if(ht_path_exists(dest_path)==1)//file exists, append
		{
			printf("File exists\n");
			if(append==1)
			{
				printf("Appending to existing file\n");
				ht_inode_file_contents_append(buf,dest_path,0);
			}
			else
			{
				printf("Existing File not overwritten\n");		
			}
			
		}
		else if(append!=1)//file doesnt exist,create,append
		{
			printf("file created\n");
			ht_insert(dest_path,1);
			ht_inode_file_contents_append(buf,dest_path,0);
			strcat(dest_path,"$");
			ht_inode_file_contents_append(dest_path,cwd,1);	
		}
		else
		{
			printf("File does not exist\n");
		}
	}
}
void rmv(char* source_path)
{
	if(strcmp(source_path,"..")==0 || strcmp(source_path,".")==0)
	{
		printf("Invalid name\n"); 
	}
	else
	{
		get_absolute_path(source_path);
		if(ht_path_exists(source_path)==1)
		{
			inode_t ht_inode=ht_get_inode_info(source_path);
			
			if(ht_inode.file_type==0 && ht_inode.nlinks==2)
			{	
				ht_delete_inode(source_path);
			}
			else if(ht_inode.file_type==1)
			{
				ht_delete_inode(source_path);
				strcat(source_path,"$");
				ht_inode_file_contents_append(source_path,cwd,-1);	
			}
			else
			{	
				printf("Error non empty directory\n");
			}
	
		}
		else
		{
			printf("File does not exist\n");
		}
	}
}
void du(char* source_path)
{
	get_absolute_path(source_path);
	if(ht_path_exists(source_path)==1)
	{
		ht_display_inode_size(source_path);
	}	
	else
	{
		printf("File does not exist\n");
	}
}
void vi(char* source_path)
{
	if(strcmp(source_path,"..")==0 || strcmp(source_path,".")==0)
	{
		printf("Invalid name\n"); 
	}
	else
	{
		get_absolute_path(source_path);
		if(ht_path_exists(source_path)==1)//file exists
		{
			printf("File already exists\n");
		}
		else//file doesnt exist,create,append
		{
			ht_insert(source_path,1);
			
			char buf[4096];
			printf("Press '$' to exit from editor\n");
			scanf("%[^$]s",buf);
  			ht_inode_file_contents_append(buf,source_path,0);
		}
	}
	
}


