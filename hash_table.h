//
// Created by lorenzo on 19/06/16.
//

#ifndef STUDENT_LZ78_HASH_TABLE_H
#define STUDENT_LZ78_HASH_TABLE_H

#endif //STUDENT_LZ78_HASH_TABLE_H


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include "encode.h"

// Struct which defines table key
struct table_key{
    node father;
    unsigned char code;
};

// Struct which defines an entry of the table (key and value)
struct entry_table{
    struct table_key* key;
    node value;
    struct entry_table* next;
};


struct hash_table{
    int size;
    struct entry_table** table;
};


// Main functions
struct hash_table* create(int size);
node get(struct hash_table*, struct table_key*, uint8_t* found);
node put(struct hash_table*, struct table_key*, node );
void print_table(struct hash_table*);
void destroy(struct hash_table *);

// Utilities
int hash(struct table_key*, int);
int compare_key(struct table_key*, struct table_key*);
struct entry_table* lookup(struct hash_table*, struct table_key*);
int count_digits(int);
int select_hash_size(int);
