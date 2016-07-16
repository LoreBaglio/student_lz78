
#include "hash_table.h"

/**
 * This function returns an hash table of size passed as argument.
 */
struct hash_table *create(int size) {

    struct hash_table* hash_table = NULL;
    int i, array_size;

    hash_table = calloc(1, sizeof(struct hash_table));
    if (hash_table == NULL){
        printf("Failed to allocate memory\n");
        return NULL;
    }


    array_size = select_hash_size(size);
    hash_table->table = calloc(array_size , sizeof(struct entry_table*));

    if (hash_table->table == NULL){
        printf("Failed to allocate memory\n");
        return NULL;
    }

    hash_table->size = array_size;

    for (i = 0; i < hash_table->size; i++)
        hash_table->table[i] = NULL;

    return hash_table;
}

/**
 * This function does a lookup in the table and returns value of the key passed as argument.
 * Variable "found" can be set to 0 or 1. If it's set to 1, the return value is the value
 * for the key, if it's set to 0 the return value has no meaning. So before checking return value
 * is mandatory to check "found" variable.
 */
node get(struct hash_table* hashtable, struct table_key* key, uint8_t* found) {

    struct entry_table* entry = lookup(hashtable, key);

    if (entry) {
        *found = 1;
        return entry->value;
    }
    else
        *found = 0;

    // If not found, result has no meaning
    return 0;
}

/**
 * This function puts a value in the hash table.
 */
node put(struct hash_table* hashtable, struct table_key* key, node value) {

    int hash_index = hash(key, hashtable->size);

    struct entry_table* bin = hashtable->table[hash_index];
    struct entry_table* last = NULL;

    struct entry_table* entry = lookup(hashtable, key);

    // If null, there is no element with this key
    if (entry == NULL){

        // If bin is null, this is the first insert in the bucket of the hash table
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
            // Else we create another entry_table and we do an head insert in
            // the selected bucket.
            entry = calloc(1, sizeof(struct entry_table));
            entry->key = calloc(1, sizeof(struct table_key));
            entry->key->father = key->father;
            entry->key->code = key->code;
            entry->value = value;

            // Head insert
            last = bin->next;
            bin->next = entry;
            entry->next = last;
        }

    } else {
        // Otherwise, we simply update value
        entry->value = value;
    }

    return value;
}


/**
 * This function deallocates all the memory used by the hash table.
 */
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

    free(hash_table->table);
    free(hash_table);

}

/**
 * Auxiliary function to compare two table_key struct.
 */
int compare_key(struct table_key * first, struct table_key * second) {

    if (first->code == second->code && first->father == second->father)
        return 1;
    else
        return 0;
}

/**
 * This function does a lookup in the table and returns pointer to the entry found
 * or NULL if no entry is found.
 */
struct entry_table* lookup(struct hash_table * hashtable, struct table_key * key) {

    int hash_index = hash(key, hashtable->size);

    struct entry_table* result = NULL;
    struct entry_table* bin = hashtable->table[hash_index];

    while (bin){

        // Check if keys are equal
        if (compare_key(bin->key, key)){
            result = bin;
            break;
        }

        bin = bin->next;
    }

    return result;
}

/**
 * Auxiliary function to print hash table
 */
void print_table(struct hash_table * hash_table) {

    int i;
    for (i = 0; i < hash_table->size; i++) {

        struct entry_table *entry = hash_table->table[i];

        while (entry) {

            struct table_key *key = entry->key;

            int value = entry->value;
            printf("%d) Key: %ld-%x, Value: %d\n", i, key->father, key->code, value);
            entry = entry->next;

        }
    }

}

/**
 * This function computes hash function in order to minimize hash collision.
 */
int hash(struct table_key *key, int size) {

    unsigned long int hashval = 0;
    int i = 0;
    int digits_of_father = count_digits(key->father);

    // 1 (key->code) + '\0'
    char* real_key = calloc(1, digits_of_father + 2);
    sprintf(real_key, "%ld%c", key->father, key->code);
    real_key[digits_of_father + 1] = '\0';

    /* Convert our string to an integer */
    while( hashval < ULONG_MAX && i < strlen( real_key  ) ) {
        hashval = hashval << 8;
        hashval += real_key[ i ];
        i++;
    }
    free(real_key);

    return hashval % size;
}

int count_digits(int n) {

    return ceil(log10(n + 1));
}



/**
 * Auxiliary function which returns the effective size of the hash table.
 */
int select_hash_size(int size) {
    int ret = size/3;

    if (ret == 0)
        ret = size;

    return ret;
}








