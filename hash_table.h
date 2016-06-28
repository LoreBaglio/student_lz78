//
// Created by lorenzo on 19/06/16.
//

#ifndef STUDENT_LZ78_HASH_TABLE_H
#define STUDENT_LZ78_HASH_TABLE_H

#endif //STUDENT_LZ78_HASH_TABLE_H

#define NO_ENTRY_FOUND -1
#define FULL_DICTIONARY -2

#include <stdio.h>
#include <stdint.h>


struct table_key{
    int father;
    char code;
};

struct entry_table{
    struct table_key* key;
    int value;
    struct entry_table* next;
};

struct hash_table{
    int size;
    int effective_size;
    int entry_counter;
    struct entry_table** table;
};

// Return hash value for lookup
int hash(struct table_key*, int);

struct hash_table* create(int size);
int get(struct hash_table*, struct table_key*);
int put(struct hash_table*, struct table_key*, int );
void print_table(struct hash_table*);
void destroy(struct hash_table *);

// Utilities
int compare_key(struct table_key*, struct table_key*);
struct entry_table* lookup(struct hash_table*, struct table_key*);
int count_digits(int);
int table_is_full(struct hash_table *table);