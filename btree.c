#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include <fcntl.h>
#include <string.h>
//#include "btree.h"
#define ALLOC(x) (x *)malloc(sizeof(x))
#define REALLOC(ptr, type, size) (type *)realloc(ptr, sizeof(type) * size)
#define TRUE 1
#define FALSE 0

//to create a tree
tree_t *create_tree()
{
    tree_t *tree_ptr = ALLOC(tree_t);
    strcpy(tree_ptr -> file_name, "fname");
    tree_ptr -> file_ptr = fopen(tree_ptr -> file_name, "w+");
//    fseek(tree_ptr -> file_ptr,0,0);
    tree_ptr -> root = 0;
    tree_ptr -> size = 0;
    treenode_t x = new_node();
    write_file(tree_ptr, &x, 0);
    return tree_ptr;
}

// create a new node with default values
treenode_t new_node()
{
    treenode_t x;
    x.n = 0;
    x.leaf = 1;
    int i;
    for (i = 0; i < 2 * t; i++)
    {
        x.links[i] = -1; //no child
    }
    return x;
}

// search key
node_t search(tree_t *tree_ptr, int index, int k)
{

    treenode_t x;
    read_file(tree_ptr, &x, index);

    int i = 0;
    //search_result_t res;

    while (i < x.n && k > x.nodes[i].key)
    {
        i++;
    }
    if (i < x.n && k == x.nodes[i].key)
    {
        //res.treenode_index = index;
        //res.node_index = i;
        //printf("search:\n");
        //printf("%d %s\n",x.nodes[i].key,x.nodes[i].file_content );
        return x.nodes[i];
    }
    else if (x.leaf)
    {
        node_t res;
        res.key = -1;
        return res;
    }
    else
    {
        return search(tree_ptr, x.links[i], k);
    }
}

void display_content(tree_t *tree_ptr, int key)
{
    
	/*
	// EXAMPLE CLIENT CODE ON HOW TO CALL THIS FUNCTION
	temp.key = key;
	strcpy(temp.file_content,"Hello\n");
	insert(tree_ptr,temp);
 
	display_content(tree_ptr, key);

	*/

    node_t res;
    res = search(tree_ptr,tree_ptr->root,key);
    printf("%s\n",res.file_content);

}
// insert key
void insert(tree_t *tree_ptr, node_t node)
{
    int root_index = tree_ptr -> root;
    treenode_t root;
    treenode_t newnode;
    read_file(tree_ptr, &root, root_index);

    if (root.n == 2 * t - 1)
    {
        newnode = new_node();
        tree_ptr -> size += 1;
        tree_ptr -> root = tree_ptr -> size;
        newnode.leaf = 0;
        newnode.links[0] = root_index;
        newnode.index = tree_ptr -> size;
        
        write_file(tree_ptr, &newnode, tree_ptr -> root);
        
        insert_split_child(tree_ptr, tree_ptr -> root, &newnode, 0);
        insert_nonfull(tree_ptr, tree_ptr -> root, &newnode, node);
    }
    else
    {
        insert_nonfull(tree_ptr, tree_ptr -> root, &root, node);
    }
}

// insert into treenode that is not full
void insert_nonfull(tree_t *tree_ptr, int tree_index, treenode_t *x, node_t node)
{
    treenode_t y = new_node();
    int i = x -> n - 1;
    if (x -> leaf)
    {
        while (i >= 0 && node.key < x -> nodes[i].key)
        {
            x->nodes[i + 1] = x->nodes[i];
            i--;
        }
        x->nodes[i + 1] = node;
        x->n += 1;
        
        write_file(tree_ptr, x, tree_index);
    }
    else
    {
        while (i >= 0 && node.key < x -> nodes[i].key)
        {
            i--;
        }
        i++;

        read_file(tree_ptr, &y, x->links[i]);
        
        if (y.n == 2 * t - 1)
        {
            insert_split_child(tree_ptr, tree_index, x, i);

            if (node.key > x->nodes[i].key)
            {
                i += 1;
            }
        }
        read_file(tree_ptr, &y, x->links[i]);
        
        insert_nonfull(tree_ptr, x->links[i], &y, node);
    }
}

// split treenode into two
void insert_split_child(tree_t *tree_ptr, int tree_index, treenode_t *x, int child_index)
{
    int i;
    treenode_t z = new_node();
    treenode_t y = new_node();

    tree_ptr->size += 1;
    
    read_file(tree_ptr, &y, x->links[child_index]);

    z.leaf = y.leaf;
    z.n = t - 1;

    for (i = 0; i < t - 1; i++)
    {
        z.nodes[i] = y.nodes[i + t];
        z.links[i] = y.links[i + t];
    }

    z.links[i] = y.links[i + t];

    y.n = t - 1;

    for (i = x->n; i >= child_index + 1; i--)
    {
        x->links[i + 1] = x->links[i];
    }
    x->links[child_index + 1] = tree_ptr->size;

    for (i = x->n - 1; i >= child_index; i--)
    {
        x->nodes[i + 1] = x->nodes[i];
    }
    x->nodes[child_index] = y.nodes[t - 1];

    x->n += 1;

    write_file(tree_ptr, &y, x->links[child_index]);
    write_file(tree_ptr, x, tree_index);
    write_file(tree_ptr, &z, tree_ptr -> size);
}

// delete key
void delete_key(tree_t *tree_ptr,int index, int k)
{
    int i = 0;
    int j;

    treenode_t* x = ALLOC(treenode_t);
    read_file(tree_ptr, x, index);
    while(i < x -> n && k > x -> nodes[i].key)
    {
        i++;
    }

    if(i < x -> n && k == x -> nodes[i].key)
    {
        // 1

        if(x -> leaf)
        {
            for(j = i; j < x -> n - 1; j++)
            {
                x -> nodes[j] = x -> nodes[j + 1];
            }
            x -> n -= 1;

            write_file(tree_ptr, x, index);
        }

        // 2
        else
        {
            treenode_t* y = ALLOC(treenode_t);
            treenode_t* z = ALLOC(treenode_t);

            int y_index = x -> links[i];
            int z_index = x -> links[i + 1];
            
            read_file(tree_ptr, y, y_index);
            read_file(tree_ptr, z, z_index);
            
            // 2a            
            if(y -> n >= t)
            {
                node_t k1 = get_previous_key(tree_ptr, x -> links[i]);
                x -> nodes[i] = k1;
            
                write_file(tree_ptr, x, index);
            
                delete_key(tree_ptr, x -> links[i], k1.key);
                
            }
            // 2b
            else if(z -> n >= t)
            {
                node_t k1 = get_next_key(tree_ptr, x -> links[i + 1]);
                x -> nodes[i] = k1;
            
                write_file(tree_ptr, x, index);
            
                delete_key(tree_ptr, x -> links[i + 1], k1.key);

            }
            // 2c
            else
            {
                y -> nodes[t - 1] = x -> nodes[i];
                for(j = i; j < x -> n - 1; j++)
                {
                    x -> nodes[j] = x -> nodes[j + 1];
                    x -> links[j + 1] = x -> links[j + 2];
                }
                x -> n -= 1;
                if(x -> n == 0 && index == tree_ptr -> root)
                {
                    tree_ptr -> root = x -> links[i];
                }
                for(j = 0; j < t - 1; j++)
                {
                    y -> nodes[j + t] = z -> nodes[j];
                    y -> links[j + t] = z -> links[j];
                }
                y -> links[j + t] = z -> links[j];
                y -> n = 2 * t - 1;
                z -> n = 0;
            
                write_file(tree_ptr, x, index);
                write_file(tree_ptr, y, y_index);
                write_file(tree_ptr, z, z_index);
            
                delete_key(tree_ptr, x -> links[i], k);
            }
        }
    }
    else if(x -> leaf)
    {
     //   printf("Key not found\n");
    }
    // 3
    else
    {

        int is_w = 0;
        
        treenode_t* y = ALLOC(treenode_t);
        treenode_t* w = NULL;
        treenode_t* z = NULL;
        
        int y_index = x -> links[i];
        int w_index;
        int z_index;
        
        read_file(tree_ptr, y, y_index);
        
        
        if(y -> n == t - 1)
        {
            if(i != 0)
            {
                w_index = x -> links[i - 1];
                w = ALLOC(treenode_t);
        
                read_file(tree_ptr, w, w_index);
            }
            if(i != x -> n)
            {
                z_index = x -> links[i + 1];
                z = ALLOC(treenode_t);
        
                read_file(tree_ptr, z, z_index);
            }
            if(w != NULL && w -> n >= t)
            {
                for(j = y -> n; j >= 1; j--)
                {
                    y -> nodes[j] = y -> nodes[j - 1];
                    y -> links[j + 1] = y -> links[j];
                }
                y -> links[j + 1] = y -> links[j];
                y -> links[j] = w -> links[w -> n];
                y -> n += 1;
                y -> nodes[j] = x -> nodes[i - 1];
                x -> nodes[i - 1] = w -> nodes[w -> n - 1];
                w -> n -= 1;
        
                write_file(tree_ptr, x, index);
                write_file(tree_ptr, y, y_index);
                write_file(tree_ptr, w, w_index);
            }
            else if(z != NULL && z -> n >= t)
            {
                y -> nodes[y -> n] = x -> nodes[i];
                x -> nodes[i] = z -> nodes[0];
                y -> links[y -> n + 1] = z -> links[0];
                for(j = 0; j < z -> n - 1; j++)
                {
                    z -> nodes[j] = z -> nodes[j + 1];
                    z -> links[j] = z -> links[j + 1];
                }
                z -> links[j] = z -> links[j + 1];
                y -> n += 1;
                z -> n -= 1;

                write_file(tree_ptr, x, index);
                write_file(tree_ptr, y, y_index);
                write_file(tree_ptr, z, z_index);
            }
            else
            {
                if(w != NULL)
                {
                    w -> nodes[t - 1] = x -> nodes[i - 1];
                    for(j = i - 1;j < x -> n - 1; j++)
                    {
                        x -> nodes[j] = x -> nodes[j + 1];
                        x -> links[j + 1] = x -> links[j + 2];
                    }
                    x -> n -= 1;
                    if(x -> n == 0 && index == tree_ptr -> root)
                    {
                        tree_ptr -> root = x -> links[i - 1];
                    }
                    for(j = 0; j < t - 1; j++)
                    {
                        w -> nodes[j + t] = y -> nodes[j];
                        w -> links[j + t] = y -> links[j];
                    }
                    w -> links[j + t] = y -> links[j];
                    w -> n = 2 * t - 1;
                    y -> n = 0;
                    is_w = 1;
                
                    write_file(tree_ptr, x, index);
                    write_file(tree_ptr, y, y_index);
                    write_file(tree_ptr, w, w_index);
                }
                else if(z != NULL)
                {
                    y -> nodes[t - 1] = x -> nodes[i];
                    for(j = i;j < x -> n - 1; j++)
                    {
                        x -> nodes[j] = x -> nodes[j + 1];
                        x -> links[j + 1] = x -> links[j + 2];
                    }
                    x -> n -= 1;
                    if(x -> n == 0 && index == tree_ptr -> root)
                    {
                        tree_ptr -> root = x -> links[i];
                    }
                    for(j = 0; j < t - 1; j++)
                    {
                        y -> nodes[j + t] = z -> nodes[j];
                        y -> links[j + t] = z -> links[j];
                    }
                    y -> links[j + t] = z -> links[j];
                    y -> n = 2 * t - 1;
                    z -> n = 0;

                    write_file(tree_ptr, x, index);
                    write_file(tree_ptr, y, y_index);
                    write_file(tree_ptr, z, z_index);
                }
            }
        }
        if(is_w)
        {
            delete_key(tree_ptr, x -> links[i - 1], k);
        }
        else
        {
            delete_key(tree_ptr, x -> links[i], k);
        }
    }
}

// to get the predecessor of key
node_t get_previous_key(tree_t *tree_ptr, int index)
{
    treenode_t x;
    read_file(tree_ptr, &x, index);
    if (x.leaf)
    {
        return (x.nodes[x.n - 1]);
    }
    else
    {
        return get_previous_key(tree_ptr, (x.links[x.n]));
    }
}

// to get the successor of key
node_t get_next_key(tree_t *tree_ptr, int index)
{
    treenode_t x;
    read_file(tree_ptr, &x, index);
    if (x.leaf)
    {
        return x.nodes[0];
    }
    else
    {
        return get_next_key(tree_ptr, x.links[0]);
    }
}

void write_file(tree_t *ptr_tree, treenode_t *p, int index)
{
    fseek(ptr_tree->file_ptr, index * sizeof(treenode_t), 0);
    fwrite(p, sizeof(treenode_t), 1, ptr_tree->file_ptr);
}

void read_file(tree_t *ptr_tree, treenode_t *p, int index)
{
    fseek(ptr_tree->file_ptr, index * sizeof(treenode_t), 0);
    fread(p, sizeof(treenode_t), 1, ptr_tree->file_ptr);
}

// Used to display nodes
void display_nodes(tree_t *tree_ptr, int index)
{
    treenode_t treenode;
    read_file(tree_ptr, &treenode, index);
    int i = 0;
    if (treenode.n > 0)
    {
        printf("\tLeaf: %d\n", treenode.leaf);
        printf("\tKeys: ");
        for (i = 0; i < treenode.n; i++)
        {
            printf("%d ", treenode.nodes[i].key);
        }
        printf("\n\tLinks: ");
        for (i = 0; i < treenode.n + 1; i++)
        {
            printf("\t%d", treenode.links[i]);
        }
    }
    else
    {
        printf("Deleted");
    }
    printf("\n");
}
