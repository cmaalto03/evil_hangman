#include "my_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
struct my_string
{
   int size;
   int capacity;
   char* data;
};

typedef struct my_string My_string;

MY_STRING my_string_init(int capacity)
{
    My_string* s = (My_string*)malloc(sizeof(My_string));

    if (s != NULL)
    {
        s->size = 0;
        s->capacity = capacity;
        s->data = (char*)malloc(sizeof(char)* s->capacity);

        if (s->data == NULL)
        {
            free(s);
            return NULL;
        }
    }

    return s;
}

MY_STRING my_string_init_default(void)
{
    return my_string_init(7);
}

MY_STRING my_string_init_c_string(const char* c_string)
{
    int i = 0;

    while(c_string[i] != '\0')
    {
      i++;
    }

    My_string* s = (My_string*) my_string_init(i + 1);

    for(int j = 0; j < i; j++)
    {
      s->data[j] = c_string[j];
      s->size++;
      
    }
   
    return s;
}

void my_string_destroy(MY_STRING* phMy_string)
{
    My_string* pMy_string = (My_string*)*phMy_string;

    free(pMy_string->data);
    free(pMy_string);

    *phMy_string = NULL;
}

int my_string_get_capacity(MY_STRING hMy_string)
{
    My_string* pMy_string = (My_string*)hMy_string;

    return pMy_string->capacity;
}

int my_string_get_size(MY_STRING hMy_string)
{
    My_string* pMy_string = (My_string*)hMy_string;

    return pMy_string->size;
}


Status my_string_extraction(MY_STRING hMy_string, FILE* fp)
{
    My_string* pMy_string = (My_string*)hMy_string;

    char c;
    int size_of_string = 0;

    char* buff = malloc(sizeof(char) * 32);

    if (buff == NULL)
    {
        return FAILURE;
    }
    int noc = fscanf(fp, "%c", &c);


    // removes all white space
    while (noc != EOF && (c == ' ' || c == '\n' || c == '\t' ))
    {
        noc = fscanf(fp, "%c", &c);
    }


    while (noc != EOF)
    {
        if (c == ' ' || c == '\n' || c == '\t')
        {
            ungetc(c, fp);
            break;
        }
        *(buff+size_of_string) = c;

        size_of_string++;

        noc = fscanf(fp, "%c", &c);
    }
    

    // checks to see if no string read, so return FAILURE just in case
    if (size_of_string == 0)
    {
        free(buff);
        return FAILURE;
    }

    // checks to see if not enough capacity, re-sizes accordinly
    if (pMy_string->capacity < size_of_string)
    {
        free(pMy_string->data);
        pMy_string->data = malloc(sizeof(char) * size_of_string * 2);
	pMy_string->capacity = size_of_string * 2;
	
        if(pMy_string->data == NULL)
        {
            free(buff);
            return FAILURE;
        }

    }
    
    pMy_string->size = size_of_string;

    for(int i = 0; i < pMy_string->size; i++)
    {
        pMy_string->data[i] = buff[i];
    }

    free(buff);

    return SUCCESS;
}

Status my_string_insertion(MY_STRING hMy_string, FILE* fp)
{
    My_string* pMy_string = (My_string*)hMy_string;

    for(int i = 0; i < pMy_string->size; i++)
    {
        fprintf(fp, "%c", pMy_string->data[i]);
    }



    return SUCCESS;
}

int my_string_compare(MY_STRING hLeft_string, MY_STRING hRight_string)
{
    My_string* pLeft_string = (My_string*)hLeft_string;
    My_string* pRight_string = (My_string*)hRight_string;

    int len = 0;

    while (len < pLeft_string->size && len < pRight_string->size)
    {
      if (pLeft_string->data[len] == pRight_string->data[len])
        {
          len++;
        }
      else
        {
          return pLeft_string->data[len] - pRight_string->data[len];
        }
    }

    return pLeft_string->size - pRight_string->size;

}

Status my_string_push_back(MY_STRING hMy_string, char value)
{
    My_string* pMy_string = (My_string*)hMy_string;

    if (pMy_string->size >= pMy_string->capacity)
    {
        char* new_data = malloc(sizeof(char) * pMy_string->capacity*2);

        if (new_data == NULL)
	    {  
            return FAILURE;
        }

        for (int i = 0; i < pMy_string->size; i++)
        {
            new_data[i] = pMy_string->data[i];
	    }

        free(pMy_string->data);
        pMy_string->data = new_data;
        pMy_string->capacity = pMy_string->capacity*2;

    }

    pMy_string->data[pMy_string->size++] = value;

    return SUCCESS;
}

Boolean my_string_empty(MY_STRING hMy_string)
{
    My_string* pMy_string = (My_string*)hMy_string;

    return ( pMy_string->size <= 0 ) ? TRUE : FALSE;
}

Status my_string_pop_back(MY_STRING hMy_string)
{
    My_string* pMy_string = (My_string*)hMy_string;

    if (my_string_empty(pMy_string))
    {
        return FAILURE;
    }

    pMy_string->size--;

    return SUCCESS;
}

char* my_string_at(MY_STRING hMy_string, int index)
{
    My_string* pMy_string = (My_string*)hMy_string;

    if (index < 0 || index >= pMy_string->size)
    {
        return NULL;
    }

    return pMy_string->data+index;
}

char* my_string_c_str(MY_STRING hMy_string)
{
    My_string* pMy_string = (My_string*)hMy_string;

    if (pMy_string->capacity == pMy_string->size)
    {
        char* copy = (char*)malloc(sizeof(char) * pMy_string->capacity * 2);

        for(int i = 0; i < pMy_string->capacity; i++)
        {
            copy[i] = pMy_string->data[i];
        }
        free(pMy_string->data);
        pMy_string->data = copy;
        pMy_string->capacity = pMy_string->capacity * 2;
    }

    pMy_string->data[pMy_string->size] = '\0';
    return pMy_string->data;
}

Status my_string_concat(MY_STRING hResult, MY_STRING hAppend)
{
    My_string* pResult = (My_string*)hResult;
    My_string* pAppend = (My_string*)hAppend;

    char* temp_data = (char*)malloc(sizeof(char)*pResult->size);




    
    int temp_size = pResult->size;
    int temp_capacity = pResult->capacity;


    
    for(int i = 0; i < pResult->size; i++)
    {
        temp_data[i] = pResult->data[i];
    }

    for (int i = 0; i < pAppend->size; i++)
    {
        Status push_back_status = my_string_push_back(pResult, pAppend->data[i]);

        if (push_back_status == FAILURE)
        {
            // reset original string 

            free(pResult->data);
            pResult->data = temp_data;
            pResult->size = temp_size;
            pResult->capacity = temp_capacity;

            return FAILURE;
        }
    }

   free(temp_data);

    return SUCCESS;

}

Status my_string_assignment(MY_STRING hLeft, MY_STRING hRight)
{
    My_string* pLeft = (My_string*)hLeft;
    My_string* pRight = (My_string*)hRight;

    if (pLeft->capacity < pRight->capacity)
    {

        free(pLeft->data);
        pLeft->data = (char *)malloc(pRight->capacity); 

        if (pLeft->data == NULL)
        {
            return FAILURE;
        }

        pLeft->capacity = pRight->capacity;
    }


    for (int i = 0; i < pRight->size; i++)
    {
        pLeft->data[i] = pRight->data[i];
    }

    pLeft->size = pRight->size;  


    return SUCCESS;
}


MY_STRING my_string_init_copy(MY_STRING hMy_string)
{
    My_string* pMy_string = (My_string*)hMy_string;



    MY_STRING hCopy = NULL;
    hCopy = my_string_init_default();

    
    My_string *pCopy = (My_string*)hCopy;

    free(pCopy->data);
    pCopy->data = (char*)malloc(sizeof(char) * pMy_string->capacity);

    if (pCopy->data == NULL)
    {
        return NULL;
    }

    pCopy->size = pMy_string->size;
    pCopy->capacity = pMy_string->capacity;

    for(int i = 0; i < pMy_string->size; i++)
    {
        pCopy->data[i] = pMy_string->data[i];
    }
    
    return hCopy;

    
}
void my_string_swap(MY_STRING hLeft, MY_STRING hRight)
{
    My_string* pLeft = (My_string*)hLeft;
    My_string* pRight = (My_string*)hRight;

    My_string temp = *pLeft;

    *pLeft = *pRight;

    *pRight = temp;

}


Status get_word_key_value(MY_STRING current_word_family, MY_STRING new_key, MY_STRING word, char guess)
{
    int word_size = my_string_get_size(word);


    while (my_string_empty(new_key) == FALSE)
    {
        my_string_pop_back(new_key);
    }

    for (int i = 0; i < word_size; i++)
    {
        if (*my_string_at(word, i) == guess)
        {
          if (my_string_push_back(new_key,tolower( guess)) == FAILURE)
            return FAILURE;
   
        }
        else
        {
          if (my_string_push_back(new_key, *my_string_at(current_word_family, i)) == FAILURE)
            return FAILURE;
        }
    }
}

MY_STRING my_string_init_dash(int number_of_dashes)
{
    MY_STRING pNew = my_string_init_default();

    for(int i = 0; i < number_of_dashes; i++)
    {
        my_string_push_back(pNew, '-');
    }
    
    return pNew;
}