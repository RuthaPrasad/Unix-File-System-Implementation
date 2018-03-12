#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FIXED_TABLESIZE 1000
#define FILE_SIZE 4096
#define BLKSIZE 128


typedef struct inode
{
	int inode_num;
	int size;
	int blksize;
	int file_type;
	int ctime;
	int mtime;
	int atime;
	int nlinks;
}inode_t;

struct entry_s {
	int not_empty;
	char path[PATH_MAX];
	int hash_value;
	inode_t inode;
};

typedef struct entry_s entry_t;
typedef int ht_t;

int ht_hash( char *key);
ht_t copy_inode_to_ht(ht_t ht, inode_t src, int index);
ht_t ht_insert_node(ht_t ht, char *path, inode_t inode, int *hash_val);
inode_t ht_get_inode(ht_t ht, char *path);
ht_t ht_delete(ht_t ht, char *path);
int ht_display(ht_t ht);

int ht_diskfiles_init(char* htdisk_filename,char* btdisk_filename);

int ht_path_exists(char *path);
int ht_display_inode_ls(char *path);
int ht_display_inode_size(char *path);
int ht_display_inode_file_contents(char* source_path);
int ht_insert(char* dest_path,int file_type);
int ht_inode_file_contents_append(char* buf,char* path,int nlinks);
int ht_delete_inode(char* source_path);
inode_t ht_get_inode_info(char* dest_path);
int ht_copy_files(char* source_path,char* dest_path);
	
int ht_open(char* source_path);
void ht_close(int fd);
void update_c_source_path();

#define MAX_FILE_CONTENT 4096
#define t 5

struct node
{
	int key;
	char file_content[MAX_FILE_CONTENT];
};
typedef struct node node_t;

struct treenode
{
	int index; 
	int n; 
	int leaf;
	node_t nodes[2 * t - 1];
  	int links[2 * t];
};
typedef struct treenode treenode_t;

struct tree
{
  	char file_name[20];
	FILE *file_ptr;
	int root;  
	int size;
};
typedef struct tree tree_t;

tree_t* create_tree();
treenode_t new_node();

void insert(tree_t*, node_t);
void insert_split_child(tree_t*, int, treenode_t*, int);
void insert_nonfull(tree_t*, int, treenode_t*, node_t);
void delete_key(tree_t*, int, int);
node_t search(tree_t*, int, int);
void display_content(tree_t*, int);
void write_file(tree_t*, treenode_t*, int); 
void read_file(tree_t*, treenode_t*, int );
node_t get_previous_key(tree_t*, int);
node_t get_next_key(tree_t*, int);
void display_content(tree_t* tree_ptr,int key);

FILE* htdisk;
tree_t* tree_ptr; 
int INODE_NUM;
int file_type;

void store_metadata();
void load_metadata();




		
