#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SUCCESS -17
#define FAILURE -15
#define MAGIC_STRING "#*"

typedef struct File
{
    char f_name[50];
    struct File* link;
} File;

typedef struct Sub_node
{
    char f_name[50];
    int  word_count;
    struct Sub_node* link;
} Sub;

typedef struct Main_node
{
    char  word[50];
    int   f_count;
    Sub*  sub_link;
    struct Main_node* link;
} Main;

typedef struct Hash_table
{
    int index;
    Main* link;
} Hash_table;

int cla_validation(int argc, char* argv[]);
int save_file(int argc, char* argv[], File** f);
int create_database(File** head, Hash_table* ht);
int display_database(Hash_table* ht);
int  search_database(Hash_table* ht, char* name);
int  save_database(char* f_name, Hash_table* ht);
int  update_database(char* f_name, Hash_table* ht, File** f_head);

int insert_last_file(File** head, char* str);
int insert_last_main(Main** head, char* word, char* f_name);
int insert_last_sub(Sub** head, char* f_name);
int print_list(File* head);

#endif
