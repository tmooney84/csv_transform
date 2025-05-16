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

string_array *csv_parser(char *param_1, int *num_rows) {
    if (param_1 == NULL)
        return NULL;
    
    int len = strlen(param_1);
    int lines = 0;
    
    for (int i = 0; i < len; i++) {
        if (param_1[i] == '\n') {
            lines++;
        }
    }
    if (len > 0 && param_1[len - 1] != '\n')
        lines++;

    *num_rows = lines;

    string_array *result = malloc(sizeof(string_array));
    if (!result)
        return NULL;
    result->size = lines;
    result->array = malloc(lines * sizeof(char *));
    if (!result->array) {
        free(result);
        return NULL;
    }

    char *copy = strdup(param_1);
    if (!copy) {
        free(result->array);
        free(result);
        return NULL;
    }

    int index = 0;
    char *line = strtok(copy, "\n");
    while (line != NULL && index < lines) {
        result->array[index] = malloc((strlen(line) + 1) * sizeof(char));
        if (!result->array[index]) {
            for (int j = 0; j < index; j++)
                free(result->array[j]);
            free(result->array);
            free(result);
            free(copy);
            return NULL;
        }
        strcpy(result->array[index], line);
        index++;
        line = strtok(NULL, "\n");
    }
    free(copy);
    return result;
}

string_array *my_data_transform(char *param_1) {
    if (param_1 == NULL)
        return NULL;

    int num_rows = 0;
    string_array *csv = csv_parser(param_1, &num_rows);
    if (!csv)
        return NULL;

    for (int r = 1; r < num_rows; r++) {
        char *row_copy = strdup(csv->array[r]);
        if (!row_copy)
            continue;
        
        char *tokens[10] = {0};
        int token_index = 0;
        char *token = strtok(row_copy, ",");
        while (token != NULL && token_index < 10) {
            tokens[token_index++] = token;
            token = strtok(NULL, ",");
        }
        if (token_index != 10) {
            free(row_copy);
            continue;
        }

        char email_domain[256] = "";
        char *at_ptr = strchr(tokens[4], '@');
        if (at_ptr) {
            strncpy(email_domain, at_ptr + 1, sizeof(email_domain) - 1);
            email_domain[sizeof(email_domain) - 1] = '\0';
        } else {
            strncpy(email_domain, tokens[4], sizeof(email_domain) - 1);
            email_domain[sizeof(email_domain) - 1] = '\0';
        }

        int age = atoi(tokens[5]);
        char age_bucket[10] = "";
        if (age >= 1 && age <= 20)
            strcpy(age_bucket, "1->20");
        else if (age >= 21 && age <= 40)
            strcpy(age_bucket, "21->40");
        else if (age >= 41 && age <= 65)
            strcpy(age_bucket, "41->65");
        else if (age >= 66 && age <= 99)
            strcpy(age_bucket, "66->99");
        else
            strcpy(age_bucket, tokens[5]);  

        char time_of_day[20] = "";
        if (strlen(tokens[9]) >= 13) {  
            char hour_str[3] = { tokens[9][11], tokens[9][12], '\0' };
            int hour = atoi(hour_str);
            if (hour >= 6 && hour < 12)
                strcpy(time_of_day, "morning");
            else if (hour >= 12 && hour < 18)
                strcpy(time_of_day, "afternoon");
            else if (hour >= 18 && hour < 24)
                strcpy(time_of_day, "evening");
            else
                strcpy(time_of_day, tokens[9]);  
        } else {
            strcpy(time_of_day, tokens[9]);
        }

        char new_row[1024] = "";
        snprintf(new_row, sizeof(new_row),
                 "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                 tokens[0],
                 tokens[1],
                 tokens[2],
                 tokens[3],
                 email_domain,    // transformed Email field
                 age_bucket,      // transformed Age field
                 tokens[6],
                 tokens[7],
                 tokens[8],
                 time_of_day);    // transformed Order At field

        free(csv->array[r]);
        csv->array[r] = strdup(new_row);
        free(row_copy);
    }
    return csv;
}