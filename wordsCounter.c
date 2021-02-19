#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "wc.h"

const int hashSize = 5000;

// Node to get stored in the hash table 
struct node {
    // key is the word you'll be counting
	char *key;
    // number of times that word occured
    int frequency;
    struct node *next;
};

// hastable is wc
struct wc {
    struct node **table;
    int currentsize;
    int tablesize;

};

// returns a postive hash value
/*int
hashFunc(char *str)
{
    int hash = 7;
    int length = strlen(str);

    for(int i = 0; i < length; i++)
        hash = (hash * 31) + *(str + i);
    return hash & 0x7FFFFFFF;
}*/

unsigned long
hashFunc(char *str)
{
    unsigned long hash = 5381;
    int length = strlen(str);
    for(int i = 0; i < length; i++, ++str){
        hash =( (hash << 5) + hash )+ (*str);
    }
    
    return hash;
}

/* Allocates memory for a new node. Initializes the new node's members */
struct node * nalloc(char *key) {
    struct node *p = (struct node *) malloc(sizeof(struct node));

    if(p != NULL) {
        p->key = strdup(key);
        p->frequency =  0;
        p->next = NULL;
    }
    return p;
}

// adding a new entry in hash table
struct node * addEntry(struct wc *h, char *key, unsigned long in_hash) {
    struct node *newnode;

    //unsigned long index = hashFunc(key) % h->tablesize;
    unsigned long index = in_hash % h->tablesize;
    struct node *current = h->table[index];

    // Search for duplicate value 
    while(current != NULL) {
        if(strcmp(key, current->key) == 0)
            return current;
        current = current->next;
    }
    // Create new node if no duplicate is found 
    newnode = nalloc(key);
    newnode->next = h->table[index];
    h->table[index] = newnode;
    h->currentsize = h->currentsize + 1;
    return newnode;
}

struct wc *wc_init(char *word_array, long size)
{
    unsigned int ctr = 0;

    struct wc *wc;
    wc = (struct wc *)malloc(sizeof(struct wc));
	assert(wc);

    wc->table = (struct node **) malloc(sizeof(struct node *) * size);
    if(wc != NULL) {
        wc->currentsize = 0;
        wc->tablesize = size;
    }

    for(int i = 0; i < size; i++){
        wc->table[i] = NULL;
    }

    struct node *node = NULL;

    while(ctr < size) {
        char word[1000];
        memset(word, 0x0, 1000);
        int i = 0;
        char wac = word_array[ctr];
        unsigned long hash = 5381;
        
        while(wac != ' ' && wac != '\0' && wac != '\n' && wac != '\r' && wac != '\t') {
            word[i] = wac;
            ctr++;
            i++;
            
            hash = ((hash << 5) + hash) + (wac);
            wac = word_array[ctr];
        }   
        word[i] = '\0';
        ctr++;
        
        if(i < 1 || word[0] == '\0') continue;
        //printf("%s %d %ld\n", word, ctr, size);

        node = addEntry(wc, word, hash);
        node->frequency = node->frequency + 1;
    }

	return wc;
}

void
wc_output(struct wc *wc)
{
	struct node *current = NULL;

    for(int i = 0; i < wc->tablesize; i++) {
        current = wc->table[i];
        while(current != NULL) {
            printf("%s:%d\n", current->key, current->frequency);
            current = current->next;
        }
    }
}


/* Deallocates memory of the string stored in the node and the
   node itself */
void freenode(struct node *node) {
    free(node->key);
    free(node);
}

void
wc_destroy(struct wc *wc)
{
	struct node *current = NULL;
    int i;

    for(i = 0; i < wc->tablesize; i++) {
        current = wc->table[i];
        if(current == NULL)
            continue;
        /* Deallocate memory of every node in the table */
        while(current->next != NULL) {
            wc->table[i] = wc->table[i]->next ;
            freenode(current);
            current = wc->table[i];
        }
        freenode(current);
    }
    free(wc->table);
	free(wc);
}
