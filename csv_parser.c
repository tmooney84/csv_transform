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

#ifndef STRUCT_STRING_ARRAY_ARRAY
#define STRUCT_STRING_ARRAY_ARRAY
typedef struct s_string_array_array
{
    int size;
    string_array **array;
} string_array_array;
#endif

string_array_array *my_csv_parser(char *csv_string, char *param_2)
{
    if (!csv_string || !param_2)
    {
        return NULL;
    }

    else if (csv_string[0] == '\0' || param_2[0] == '\0')
    {
        return NULL;
    }

    char delim = param_2[0];
    int csv_len = strlen(csv_string);
    int num_columns = 0;
    int num_rows = 0;
    int new_flag = 0;

    // find number of columns and rows
    for (int i = 0; i < csv_len; i++)
    {
        if (new_flag == 0 && csv_string[i] == delim)
        {
            num_columns++;
        }
        else if (csv_string[i] == '\n')
        {
            new_flag = 1;
            num_rows++;
        }
    }

    num_columns += 1; // to account correct count of columns in relation to delimiters

    // create matrix to allocate for strings
    string_array_array *csv_matrix = malloc(sizeof(string_array_array));
    if (!csv_matrix)
        return NULL;
    csv_matrix->size = num_rows;
    csv_matrix->array = malloc(num_rows * sizeof(string_array *));
    if (!csv_matrix->array)
    {
        free(csv_matrix);
        return NULL;
    }

    for (int i = 0; i < num_rows; i++)
    {
        csv_matrix->array[i] = malloc(sizeof(string_array));
        if(!csv_matrix->array[i])
        {
            free(csv_matrix->array);
            free(csv_matrix);
        }
        csv_matrix->array[i]->size = num_columns;
        csv_matrix->array[i]->array = malloc(sizeof(char *) * num_columns);
        if (!csv_matrix->array[i]->array)
        {
            for (int v = 0; v < num_rows; v++)
            {
                free(csv_matrix->array[v]);
            }
            free(csv_matrix->array);
            free(csv_matrix);
            return NULL;
        }

        for (int j = 0; j < num_columns; j++)
        {
            csv_matrix->array[i]->array[j] = malloc(255 * sizeof(char));
            if (!csv_matrix->array[i]->array[j])
            {
                for (int f = 0; f < num_rows; f++)
                {
                    for (int g = 0; g < num_columns; g++)
                    {
                        free(csv_matrix->array[f]->array[g]);
                    }
                    free(csv_matrix->array[f]);
                }
                free(csv_matrix->array);
                free(csv_matrix);
            }
        }
    }

    //parse to matrix

    int row = 0;
    int col = 0;
    int l_idx = 0; //letter index
    for(int k = 0; k < csv_len && row < num_rows && col < num_columns; k++)
    {
        if(csv_string[k] != delim && csv_string[k] != '\n')
        {
            csv_matrix->array[row]->array[col][l_idx] = csv_string[k];
            l_idx++;
        }
        else if(csv_string[k] == delim)
        {
            l_idx = 0;
            col++;
        }
        else if(csv_string[k] == '\n')
        {
            l_idx = 0;
            col = 0;
            row ++;
        }
    }

    return csv_matrix;
}