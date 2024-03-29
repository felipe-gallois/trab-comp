#ifndef HASH_H
#define HASH_H

#define HASH_SIZE 997

typedef struct HashEntry {
    int type;
    char *string;
    struct HashEntry *next;
} HashEntry;

void hash_init();
int get_address(char *string);
HashEntry *find_entry(char *string);
HashEntry *insert_entry(char *string, int type);
void print_hash();

#endif
