#include "hashtable.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int ht;

/*
int ht_diskfiles_init(char* htdisk_filename,char* btdisk_filename)
{
	htdisk=fopen(htdisk_filename,"r+");
	fseek(htdisk,0,0);
	tree_ptr = create_tree(btdisk_filename);
	
	INODE_NUM=0;
	file_type=0;
}
*/
	
/* Hash a string for a particular hash tab. */
int ht_hash(char *key ) {

	unsigned long int hashval = 0;
	int i = 0;
	
	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen(key))
	{
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}
	return hashval % FIXED_TABLESIZE;
}

/* Retrieve a path-inode pair from a hash tab. */
inode_t ht_get_inode(ht_t ht, char *path)
{
	int hash_value = ht_hash(path);
	int i = hash_value;
	entry_t ht_node;
	ht_node.inode.inode_num = -1;
	ht_node.inode.file_type=-1;
	ht_node.inode.size=-1;
	ht_node.inode.blksize=-1;
	ht_node.inode.ctime =time(NULL);
	ht_node.inode.mtime =time(NULL);
	ht_node.inode.atime =time(NULL);
	for (;i< FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && (strcmp(ht_node.path,path)==0))
		{
			ht_node.inode.atime =time(NULL);
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return ht_node.inode;
		}
	}
	for (i = 0;i < hash_value;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && (strcmp(ht_node.path, path)==0))
		{
			ht_node.inode.atime =time(NULL);
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return ht_node.inode;
		}
	}
	return ht_node.inode;
}
inode_t ht_get_inode_info(char* dest_path)
{
	return ht_get_inode(ht,dest_path);	
}

int ht_path_exists(char *path)
 {
	int hash_value = ht_hash(path);
	int i;
	entry_t ht_node;
	for (i=0;i<FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && (strcmp(ht_node.path,path)==0))
		{
			ht_node.inode.atime =time(NULL);
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 1;
		}
	}
	return 0;
}
ht_t copy_inode_to_htdisk(ht_t ht,inode_t src,int index,char* path,int not_empty,int hash_value)
{
	entry_t ht_node;
	fseek(htdisk,index*sizeof(entry_t),0);
	fread(&ht_node, sizeof(entry_t),1,htdisk);
	
	ht_node.hash_value=hash_value;
	ht_node.not_empty=not_empty;
	strcpy(ht_node.path,path);
	ht_node.inode.inode_num = src.inode_num;
	ht_node.inode.size = src.size;
	ht_node.inode.blksize = src.blksize;
	ht_node.inode.file_type = src.file_type;
	ht_node.inode.nlinks=src.nlinks;
	ht_node.inode.ctime = src.ctime;
	ht_node.inode.mtime = time(NULL);
	ht_node.inode.atime = time(NULL);
	
	fseek(htdisk,index*sizeof(entry_t),0);
	fwrite(&ht_node, sizeof(entry_t),1,htdisk);
	
	return ht;
			
}

/* Insert a path-inode pair into a hash tab. */
ht_t ht_insert_node(ht_t ht, char *path, inode_t inode, int *hash_val)
{
	int hash_value = ht_hash(path);
	int i;
	*hash_val = hash_value;	
	i = hash_value;
	
	entry_t ht_node;
	fseek(htdisk,i*sizeof(entry_t),0);
	fread(&ht_node, sizeof(entry_t),1,htdisk);
			
	if(strcmp(ht_node.path, path)==0)
	{
//		printf("inode exists\n");
		return (copy_inode_to_htdisk(ht,ht_node.inode,i,path,1,hash_value));
	}
	else
	{
		i=i+1;
		for (;i< FIXED_TABLESIZE;i++)
		{
			fseek(htdisk,i*sizeof(entry_t),0);
			fread(&ht_node, sizeof(entry_t),1,htdisk);
			if (!(ht_node.not_empty) )
			{
//				printf("creating new inode\n");
				ht_node.inode.inode_num = INODE_NUM;
				INODE_NUM=INODE_NUM+1;
				ht_node.inode.file_type=file_type;
				ht_node.inode.size=0;
				ht_node.inode.blksize=4*BLKSIZE;
				ht_node.inode.ctime = time(NULL);
				ht_node.inode.mtime = time(NULL);
				ht_node.inode.atime = time(NULL);
				if(file_type==0)//dir
					ht_node.inode.nlinks=2;
				else//file
					ht_node.inode.nlinks=1;
				return (copy_inode_to_htdisk(ht,ht_node.inode,i,path,1,hash_value));
			}
		}
		for (i = 0;i < hash_value;i++)
		{
			fseek(htdisk,i*sizeof(entry_t),0);
			fread(&ht_node, sizeof(entry_t),1,htdisk);
			if (!(ht_node.not_empty)) 
			{
//				printf("creating new inode\n");
				ht_node.inode.inode_num = INODE_NUM;
				INODE_NUM=INODE_NUM+1;
				ht_node.inode.file_type=file_type;
				ht_node.inode.size=0;
				ht_node.inode.blksize=4*BLKSIZE;
				ht_node.inode.ctime = time(NULL);
				ht_node.inode.mtime = time(NULL);
				ht_node.inode.atime = time(NULL);
				if(file_type==0)//dir
					ht_node.inode.nlinks=2;
				else//file
					ht_node.inode.nlinks=1;
				return (copy_inode_to_htdisk(ht,ht_node.inode,i,path,1,hash_value));
			}
		}
	}
}
int ht_insert(char* dest_path,int type)
{
	file_type=type;
	int hash_val;
	inode_t ht_inode=ht_get_inode(ht,dest_path);
	ht = ht_insert_node(ht, dest_path, ht_inode, &hash_val);
}
ht_t ht_delete(ht_t ht, char *path)
{
	int hash_value = ht_hash(path);
	int i = hash_value;
	entry_t ht_node;
	for (;i< FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		 {
				ht_node.not_empty = 0;
				fseek(htdisk,i*sizeof(entry_t),0);
				fwrite(&ht_node, sizeof(entry_t),1,htdisk);
				delete_key(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
	    			return 0;		
		}
	}
	
	for (i = 0;i < hash_value;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		 {
			ht_node.not_empty = 0;
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			delete_key(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
	    		return 0;		
		}
	}
	return ht;
}
int ht_delete_inode(char* path)
{
	ht=ht_delete(ht,path);
}
int ht_display_inode_size(char *path)
{
	int hash_value = ht_hash(path);
	int i = hash_value;
	entry_t ht_node;
	for (;i< FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			printf("size[%d]\t",ht_node.inode.size);
			printf("%s\n",ht_node.path );
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}
	i=0;	
	for (;i < hash_value;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			printf("size[%d]\t",ht_node.inode.size);
			printf("%s\n",ht_node.path );
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}
	
}
int ht_copy_files(char* source_path,char* dest_path)
{
	inode_t source = ht_get_inode_info(source_path);
	inode_t dest = ht_get_inode_info(dest_path);
	
	node_t source_node=search(tree_ptr,tree_ptr->root,source.inode_num);
	char buf[4096];
	strcpy(buf,source_node.file_content);
	
	node_t dest_node=search(tree_ptr,tree_ptr->root,dest.inode_num);
	if(dest_node.key!=-1)
	{
		delete_key(tree_ptr,tree_ptr->root,dest.inode_num);
	}
	dest_node.key=dest.inode_num;
	strcpy(dest_node.file_content,buf);
	insert(tree_ptr,dest_node);
	
	int hash_value = ht_hash(dest_path);
	int i;
	entry_t ht_node;
	for (i=0;i<FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && (strcmp(ht_node.path,dest_path)==0))
		{
			ht_node.inode.atime =time(NULL);
			ht_node.inode.ctime =time(NULL);
			ht_node.inode.mtime =time(NULL);
			ht_node.inode.size =strlen(buf);
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 1;
		}
	}
	return 0;
	
}
void removeSubstring(char *s,const char *toremove)
{
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}    
int ht_inode_file_contents_append(char* buf,char* path,int nlinks)
{
	if(nlinks!=-1)
	{
		int hash_value = ht_hash(path);
		int i = hash_value;
		entry_t ht_node;
		for (;i< FIXED_TABLESIZE;i++)
		{
			fseek(htdisk,i*sizeof(entry_t),0);
			fread(&ht_node, sizeof(entry_t),1,htdisk);
			if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
			{
				ht_node.inode.mtime = time(NULL);
				ht_node.inode.atime = time(NULL);
				ht_node.inode.nlinks = ht_node.inode.nlinks+nlinks;
			
				if(ht_node.inode.file_type==1)
				{
					if(strlen(buf)>ht_node.inode.blksize)
						ht_node.inode.blksize+=4*BLKSIZE;
					ht_node.inode.size=ht_node.inode.size+strlen(buf);
				}
				else if(ht_node.inode.file_type==0)
				{
					ht_node.inode.size=ht_node.inode.size+1;
				}
				
				node_t node=search(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
				if(node.key!=-1)
				{
					delete_key(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
				}
				node.key=ht_node.inode.inode_num;
				strcat(node.file_content,buf);
				//insert into btree
				insert(tree_ptr,node);
				fseek(htdisk,i*sizeof(entry_t),0);
				fwrite(&ht_node, sizeof(entry_t),1,htdisk);
				return 0;
			}
		}
		i=0;	
		for (;i < hash_value;i++)
		{
			fseek(htdisk,i*sizeof(entry_t),0);
			fread(&ht_node, sizeof(entry_t),1,htdisk);
			if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
			{
				ht_node.inode.mtime = time(NULL);
				ht_node.inode.atime = time(NULL);
				ht_node.inode.nlinks = ht_node.inode.nlinks+nlinks;
			
				if(ht_node.inode.file_type==1)
				{
					if(strlen(buf)>ht_node.inode.blksize)
						ht_node.inode.blksize+=4*BLKSIZE;
					ht_node.inode.size=ht_node.inode.size+strlen(buf);
				}
				else if(ht_node.inode.file_type==0)
				{
					ht_node.inode.size=ht_node.inode.size+1;
				}
				
				node_t node=search(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
				if(node.key!=-1)
				{
					delete_key(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
				}
				node.key=ht_node.inode.inode_num;
				strcat(node.file_content,buf);
				//insert into btree
				insert(tree_ptr,node);
				fseek(htdisk,i*sizeof(entry_t),0);
				fwrite(&ht_node, sizeof(entry_t),1,htdisk);
				return 0;
			
			}
		}
	}
	else
	{
		int i = ht_hash(path);
		entry_t ht_node;
		
		for (i=0;i<FIXED_TABLESIZE;i++)
		{
			fseek(htdisk,i*sizeof(entry_t),0);
			fread(&ht_node, sizeof(entry_t),1,htdisk);
			if ((ht_node.not_empty) && (strcmp(ht_node.path,path)==0))
			{
				ht_node.inode.mtime = time(NULL);
				ht_node.inode.atime = time(NULL);
				ht_node.inode.nlinks = ht_node.inode.nlinks+nlinks;
				ht_node.inode.size=ht_node.inode.size-1;
				
			
				char s[4096];
				char toremove[PATH_MAX];
				strcpy(toremove,buf);
				
				node_t node=search(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
				if(node.key!=-1)
				{
					delete_key(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);
				}
				node.key=ht_node.inode.inode_num;
				
				strcpy(s,node.file_content);
				removeSubstring(s,toremove);
				strcpy(node.file_content,s);
				
				if(strlen(s)>ht_node.inode.blksize)
					ht_node.inode.blksize+=4*BLKSIZE;
				ht_node.inode.size=strlen(s);
			
				//insert into btree
				insert(tree_ptr,node);
				fseek(htdisk,i*sizeof(entry_t),0);
				fwrite(&ht_node, sizeof(entry_t),1,htdisk);
				return 0;
			}
		}
		
	}
}
int ht_display_inode_file_contents(char *path)
{
	int hash_value = ht_hash(path);
	int i = hash_value;
	entry_t ht_node;
	for (;i< FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			display_content(tree_ptr,ht_node.inode.inode_num);
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}
	i=0;	
	for (;i < hash_value;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			display_content(tree_ptr,ht_node.inode.inode_num);
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}
}	
int ht_display_file_ls(char *path)
{
	int hash_value = ht_hash(path);
	int i = hash_value;
	entry_t ht_node;
	for (;i< FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			printf("[%d]inum\t",ht_node.inode.inode_num );
			printf("[%d]type\t",ht_node.inode.file_type );
			printf("[%d]nlnk\t",ht_node.inode.nlinks);
			printf("[%d]size\t",ht_node.inode.size );
			printf("[%d]blksize\t",ht_node.inode.blksize );
			printf("[%d]ctime\t",ht_node.inode.ctime );
			printf("[%d]mtime\t",ht_node.inode.mtime );
			printf("[%d]atime\t",ht_node.inode.atime );
			printf("[%s]name\n",ht_node.path );
			printf("\n\n");
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}
	i=0;	
	for (;i < hash_value;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			printf("[%d]inum\t",ht_node.inode.inode_num );
			printf("[%d]type\t",ht_node.inode.file_type );
			printf("[%d]nlnk\t",ht_node.inode.nlinks);
			printf("[%d]size\t",ht_node.inode.size );
			printf("[%d]blksize\t",ht_node.inode.blksize );
			printf("[%d]ctime\t",ht_node.inode.ctime );
			printf("[%d]mtime\t",ht_node.inode.mtime );
			printf("[%d]atime\t",ht_node.inode.atime );
			printf("[%s]name\n",ht_node.path );
			printf("\n\n");
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}

}
int ht_display_dir_ls(int i)
{
	entry_t ht_node;
	fseek(htdisk,i*sizeof(entry_t),0);
	fread(&ht_node, sizeof(entry_t),1,htdisk);
	node_t dir_node = search(tree_ptr,tree_ptr->root,ht_node.inode.inode_num);			
	char* file_paths= strtok(dir_node.file_content,"$");
	char buf[1024];
	while(file_paths!=NULL)
	{
		strcpy(buf,file_paths);
//		printf("path:%s\n",buf);
		ht_display_file_ls(buf);
		file_paths=strtok(NULL,"$");
					
	}
}
int ht_display_inode_ls(char *path)
{
	int hash_value = ht_hash(path);
	int i = hash_value;
	entry_t ht_node;
	for (;i< FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			if(ht_node.inode.file_type==0)
			{
			//	ht_display_file_ls(path);	
				ht_display_dir_ls(i);
			//	ht_display_inode_file_contents(path);
			}	
			else if(ht_node.inode.file_type==1)
				ht_display_file_ls(path);	
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}
	i=0;	
	for (;i < hash_value;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if ((ht_node.not_empty) && !strcmp(ht_node.path, path))
		{
			ht_node.inode.atime = time(NULL);
			if(ht_node.inode.file_type==0)
			{
			//	ht_display_file_ls(path);	
				ht_display_dir_ls(i);
			//	ht_display_inode_file_contents(path);
			}	
			else if(ht_node.inode.file_type==1)
				ht_display_file_ls(path);	
			fseek(htdisk,i*sizeof(entry_t),0);
			fwrite(&ht_node, sizeof(entry_t),1,htdisk);
			return 0;
		}
	}
}	
int ht_display(ht_t ht) 
{
	int i;
	char ht_path[PATH_MAX];
	entry_t ht_node;
//	printf("not_empty\tpath\toffset\tiNum\ttype\t\tsize\t\tctime\tmtime\tatime\tpath\n");
	for (i = 0;i < FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),0);
		fread(&ht_node, sizeof(entry_t),1,htdisk);
		if (ht_node.not_empty == 1)
		{
			
			printf("[%d]inum\t",ht_node.inode.inode_num );
			printf("[%d]type\t",ht_node.inode.file_type );
			printf("[%d]nlnk\t",ht_node.inode.nlinks);
			printf("[%d]size\t",ht_node.inode.size );
			printf("[%d]blksize\t",ht_node.inode.blksize );
			printf("[%d]ctime\t",ht_node.inode.ctime );
			printf("[%d]mtime\t",ht_node.inode.mtime );
			printf("[%d]atime\t",ht_node.inode.atime );
			printf("[%s]name\n",ht_node.path );
			printf("\n\n");
		}
	}
}
char C_source_path[PATH_MAX];
int ht_open(char* source_path)
{
	int fd = open("file.txt",O_APPEND|O_RDWR,0644);
	lseek(fd,0,0);
	strcpy(C_source_path,source_path);
	return fd;
}
void ht_close(int fd)
{
	
	int f= open("C_source_path.txt",O_TRUNC|O_RDWR,0644);
	lseek(f,0,0);
	write(f,C_source_path,strlen(C_source_path));
	close(f);
	close(fd);        
}
void load_metadata()
{
	FILE* fd= fopen("metadata.dat","r+");
	
	tree_ptr = (tree_t *)malloc(sizeof(tree_t));
    	strcpy(tree_ptr -> file_name, "fname");
    	
    	tree_ptr -> file_ptr = fopen(tree_ptr -> file_name, "r+");
	
	fseek(fd,0,SEEK_SET);
	fread(&(tree_ptr->root),sizeof(int),1,fd);
	
    	fseek(fd,sizeof(int),SEEK_SET);
    	fread(&(tree_ptr->size),sizeof(int),1,fd);
	
    	fseek(fd,2*sizeof(int),SEEK_SET);
    	fread(&(INODE_NUM),sizeof(int),1,fd);
	
	fclose(fd);
        
        htdisk=fopen("ht_disk.dat","r+");

	file_type=0;
	update_c_source_path();

}
void store_metadata()
{
	FILE* fd= fopen("metadata.dat","r+");
	
	fseek(fd,0,SEEK_SET);
	fwrite(&(tree_ptr->root),sizeof(int),1,fd);
	
    	fseek(fd,sizeof(int),SEEK_SET);
    	fwrite(&(tree_ptr -> size),sizeof(int),1,fd);
	
    	fseek(fd,2*sizeof(int),SEEK_SET);
    	fwrite(&(INODE_NUM),sizeof(int),1,fd);
	
	fclose(htdisk);
	fclose(tree_ptr->file_ptr);
	
	fclose(fd);

}
void update_c_source_path()
{
	int fd= open("C_source_path.txt",O_APPEND|O_RDWR,0644);
	lseek(fd,0,0);
	int bytes_read = read(fd,&C_source_path, sizeof(C_source_path));
        close(fd);
        ht_insert(C_source_path,1);
        
        char buf[4*BLKSIZE];
        fd= open("file.txt",O_APPEND,O_RDWR);
	lseek(fd,0,0);
	bytes_read = read(fd,&buf, sizeof(buf));
        close(fd);
        ht_inode_file_contents_append(buf,C_source_path,0);        
}
