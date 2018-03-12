#include<stdio.h>
#include<stdlib.h>
#include "hashtable.h"

int main()
{
	FILE* fd= fopen("metadata.dat","w+");
	
	int zero=0;
	
	fseek(fd,0,SEEK_SET);
	fwrite(&zero,sizeof(int),1,fd);
	
    	fseek(fd,sizeof(int),SEEK_SET);
    	fwrite(&zero,sizeof(int),1,fd);
	
    	fseek(fd,2*sizeof(int),SEEK_SET);
    	fwrite(&zero,sizeof(int),1,fd);
	
	fclose(fd);

  	htdisk = fopen("ht_disk.dat", "w+");
	
	int i=0;
	entry_t init_node;
	for (i=0;i< FIXED_TABLESIZE;i++)
	{
		fseek(htdisk,i*sizeof(entry_t),SEEK_SET);
		fread(&init_node, sizeof(entry_t),1,htdisk);
		init_node.not_empty = 0;
	}
	fclose(htdisk);
	tree_ptr = create_tree();
	
	fd= fopen("C_source_path.txt","w+");
	fclose(fd);
	
	fd= fopen("file.txt","w+");
	fclose(fd);
	
	return 0;
}
