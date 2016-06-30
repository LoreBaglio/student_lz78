//
// Created by lorenzo on 19/06/16.
//

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include "hash_table.h"

/**
----------------MAIN DI PROVA

 #include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include "hash_table.h"

int main(){

    struct hash_table* table = create(10);
    struct table_key* key = malloc(sizeof(struct table_key));
    key->code = 'a';
    key->father = 201;
    put(table, key, 10);
    key->code = 'b';
    key->father = 10;
    put(table, key, 46);
    print_table(table);
    printf("valore: %d\n", get(table,key));

    // Sennò la stampa su CLion va più lenta del return 0 (e non si vede l'output)
    sleep(2);

    return 0;


}
 */


int get(struct hash_table* hashtable, struct table_key* key) {

    struct entry_table* entry = lookup(hashtable, key);

    if (entry)
        return entry->value;

    return NO_ENTRY_FOUND;
}

int put(struct hash_table* hashtable, struct table_key* key, int value) {

    int hash_index = hash(key, hashtable->size);
    struct entry_table* bin = hashtable->table[hash_index];
    struct entry_table* last = NULL;

    struct entry_table* entry = lookup(hashtable, key);

    // No element with this key
    if (entry == NULL){

        entry = calloc(1, sizeof(struct entry_table));

        // Primo inserimento
        if (bin == NULL){
            bin = calloc(1, sizeof(struct entry_table));

            bin->key = calloc(1, sizeof(struct table_key));
            bin->key->father = key->father;
            bin->key->code = key->code;

            bin->value = value;
            bin->next = NULL;
            hashtable->table[hash_index] = bin;
        }
        else {

            entry->key = calloc(1, sizeof(struct table_key));
            entry->key->father = key->father;
            entry->key->code = key->code;
            entry->value = value;

            // Inserisco in testa
            last = bin->next;
            bin->next = entry;
            entry->next = last;
        }

    } else {
        // Update value
        entry->value = value;
    }

    return value;
}

int compare_key(struct table_key * first, struct table_key * second) {

    if (first->code == second->code && first->father == second->father)
        return 1;
    else
        return 0;
}

struct entry_table* lookup(struct hash_table * hashtable, struct table_key * key) {

    struct entry_table* result = NULL;
    struct entry_table* bin = hashtable->table[hash(key, hashtable->size)];

    while (bin){

        // Found it
        if (compare_key(bin->key, key)){
            result = bin;
            break;
        }

        bin = bin->next;
    }

    return result;
}

void print_table(struct hash_table * hash_table) {

    int i;
    for (i = 0; i < hash_table->size; i++) {

        struct entry_table *entry = hash_table->table[i];

        while (entry) {

            struct table_key *key = entry->key;

            int value = entry->value;
            printf("%d) Key: %d-%x, Value: %d\n", i, key->father, key->code, value);
            entry = entry->next;

        }
    }

}

int hash(struct table_key *key, int size) {

    unsigned long int hashval = 0;
    int i = 0;
    int digits_of_father = count_digits(key->father);

    // 1 (key->code) + '\0'
    char* real_key = calloc(1, digits_of_father + 2);
    real_key[digits_of_father + 1] = '\0';
    sprintf(real_key, "%d%c", key->father, key->code);

    /* Convert our string to an integer */
    while( hashval < ULONG_MAX && i < strlen( real_key  ) ) {
        hashval = hashval << 8;
        hashval += real_key[ i ];
        i++;
    }

    return hashval % size;
}

int count_digits(int n) {

    int count=0;

    while(n!=0)
    {
        n /= 10;             // n = n/10
        ++count;
    }

    return count;
}

int select_hash_size(int size);

struct hash_table *create(int size) {

    struct hash_table* hash_table = NULL;
    int i, array_size;

    hash_table = calloc(1, sizeof(struct hash_table));
    if (hash_table == NULL){
        printf("Failed to allocate memory\n");
        return NULL;
    }

    // Average number of collision
    array_size = select_hash_size(size);
    hash_table->table = calloc(array_size , sizeof(struct entry_table*));

    if (hash_table->table == NULL){
        printf("Failed to allocate memory\n");
        return NULL;
    }

    hash_table->size = array_size;
    hash_table->effective_size = size;

    for (i = 0; i < hash_table->size; i++)
        hash_table->table[i] = NULL;

    return hash_table;
}

int select_hash_size(int size) {
    int ret = size/3;

    if (ret == 0)
        ret = size;

    return ret;
}

void destroy(struct hash_table *hash_table) {

    int i = 0;
    struct entry_table *next_entry;

    for (i = 0; i < hash_table->size; i++) {

        struct entry_table *entry = hash_table->table[i];

        if (entry == NULL)
            continue;

        while (entry) {

            next_entry = entry->next;
            free(entry->key);
            free(entry);
            entry = next_entry;

        }

    }

    free(hash_table);

}







