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
#include "encode.h"


struct table_key{
    node father;
    unsigned char code;
};

struct entry_table{
    struct table_key* key;
    node value;
    struct entry_table* next;
};

struct hash_table{
    int size;
    int effective_size;
    struct entry_table** table;
};

// Return hash value for lookup
int hash(struct table_key*, int);

struct hash_table* create(int size);
node get(struct hash_table*, struct table_key*, uint8_t* found);
node put(struct hash_table*, struct table_key*, node );
void print_table(struct hash_table*);
void destroy(struct hash_table *);

// Utilities
int compare_key(struct table_key*, struct table_key*);
struct entry_table* lookup(struct hash_table*, struct table_key*);
int count_digits(int);