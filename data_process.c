#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef STRUCT_STRING_ARRAY
#define STRUCT_STRING_ARRAY
typedef struct s_string_array
{
    int size;
    char **array;
} string_array;
#endif

#ifndef STRUCT_INNER_ENTRY
#define STRUCT_INNER_ENTRY
typedef struct inner_entry
{
    char *key;
    int count;
    struct inner_entry *next;
} inner_entry;
#endif

#ifndef STRUCT_OUTER_ENTRY
#define STRUCT_OUTER_ENTRY
#define NUM_CATEGORIES 6

typedef struct outer_entry
{
    char *key;
    inner_entry *head;
    inner_entry *tail;
    int num_inner_cat;
} outer_entry;
#endif


/* Prototypes */
int calc_hash(const char *key);
outer_entry **build_outer_entry(void);
inner_entry *create_inner_entry(const char *key);
int add_inner_entry(outer_entry **categories, inner_entry *entry, int outer_num);
char *my_data_process(string_array *param_1);

outer_entry **build_outer_entry(void)
{
    outer_entry **categories = malloc(NUM_CATEGORIES * sizeof(outer_entry *));
    if (!categories)
        return NULL;

    for (int i = 0; i < NUM_CATEGORIES; i++)
    {
        categories[i] = malloc(sizeof(outer_entry));
        if (!categories[i])
        {
            // TODO: free 0..i-1
            return NULL;
        }
        categories[i]->num_inner_cat = 0;
        categories[i]->key = malloc(256);
        if (!categories[i]->key)
            return NULL;
        categories[i]->key[0] = '\0';

        categories[i]->head = NULL;
        categories[i]->tail = NULL;
    }
    return categories;
}

inner_entry *create_inner_entry(const char *key)
{
    inner_entry *new_entry = malloc(sizeof(inner_entry));
    if (!new_entry)
        return NULL;
    new_entry->key = malloc(strlen(key) + 1);
    if (!new_entry->key)
    {
        free(new_entry);
        return NULL;
    }
    strcpy(new_entry->key, key);
    new_entry->count = 1;
    new_entry->next = NULL;
    return new_entry;
}

int add_inner_entry(outer_entry **categories, inner_entry *entry, int cat_index)
{
    outer_entry *cat = categories[cat_index];
    inner_entry *cur = cat->head;
    
    while(cur)
    { 
        if(strcmp(cur->key, entry->key) == 0){
            cur->count++;
            free(entry->key);
            free(entry);
            return 0;
        }
        cur = cur->next;
    }
    
    //not found, append at tail
    if(!cat->head){
        cat->head = cat->tail = entry;
    }
    else{
        cat->tail->next = entry;
        cat->tail=entry;
    }
    cat->num_inner_cat++;
    return 0;
}

/* Main processor: header defines category names; each subsequent row adds counts */
char *my_data_process(string_array *param_1)
{
    if (!param_1 || param_1->size < 2)
        return NULL;

    outer_entry **categories = build_outer_entry();
    if (!categories)
        return NULL;

    for (int i = 0; i < param_1->size; i++)
    {
        char *row = strdup(param_1->array[i]);
        if (!row)
            continue;

        char *tokens[10] = {0};
        int token_index = 0;
        char *tok = strtok(row, ",");
        while (tok && token_index < 10)
        {
            tokens[token_index++] = tok;
            tok = strtok(NULL, ",");
        }

        
        if (token_index != 10)
        {
            free(row);
            continue;
        }

        if (i == 0)
        {
            strcpy(categories[0]->key, tokens[0]); // Gender
            strcpy(categories[1]->key, tokens[4]); // Email domain
            strcpy(categories[2]->key, tokens[5]); // Age
            strcpy(categories[3]->key, tokens[6]); // City
            strcpy(categories[4]->key, tokens[7]); // Device
            strcpy(categories[5]->key, tokens[9]); // Order At
        }
        else
        {
            int map_pos[6] = {0, 4, 5, 6, 7, 9}; 
            for (int m = 0; m < 6; m++)
            {
                inner_entry *e = create_inner_entry(tokens[map_pos[m]]);
                add_inner_entry(categories, e, m);
            }
        }

        free(row);
    }

    // Build JSON string
    char *json = malloc(2048);
    if (!json) return NULL;
    int p = 0;
    p += sprintf(json + p, "{");
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        p += sprintf(json + p, "\"%s\":{", categories[i]->key);
        int first = 1;
        for (inner_entry *cur = categories[i]->head; cur; cur = cur->next) {
            if (!first) p += sprintf(json + p, ",");
            p += sprintf(json + p, "\"%s\":%d", cur->key, cur->count);
            first = 0;
        }
        p += sprintf(json + p, i == NUM_CATEGORIES-1 ? "}" : "},");
    }
    p += sprintf(json + p, "}");
    return json;
}