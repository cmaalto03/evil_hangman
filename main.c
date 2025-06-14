#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "my_string.h"
#include "avl_tree.h"
#include "generic_vector.h"

#define MAX_WORD_VECTORS 30

/* FOR DEBUGGING */
const Boolean SHOW_ALL_WORD_FAMILYS = TRUE;
const Boolean SHOW_ALL_WORD_POSSIBILITIES = TRUE;

ITEM vector_my_string_init_copy(ITEM hMy_string);
void vector_my_string_destroy(ITEM* phMy_string);

// c'tor, d'tor, and fill functions to get words in dictionary to GENERIC_VECTOR*
void init_vector_arr(GENERIC_VECTOR*);
void fill_vector_arr_from_dictionary(GENERIC_VECTOR*);
void destroy_vector_arr(GENERIC_VECTOR[]);

// functions to prompt input from user for game
void prompt_guess(char*, MY_STRING);
void prompt_word_size(int*);
void prompt_guess_count(int*);
void prompt_play_again(Boolean*);
void prompt_running_total(Boolean*);

void clear_keyboard_buffer(void);


void game(Boolean*, GENERIC_VECTOR*);

// functions to respond to game
void pre_guess_dialogue(int, MY_STRING, MY_STRING);
void post_guess_debug_dialogue(Boolean, AVL_TREE, GENERIC_VECTOR);
void post_guess_correct_guess_dialogue(MY_STRING, MY_STRING, char);

// returns TRUE on post game condition met, and FALSE otherwise
Boolean post_game_dialogue(GENERIC_VECTOR, MY_STRING, int);

int main(int argc, char* argv[])
{
    GENERIC_VECTOR* vector_arr = (GENERIC_VECTOR*) malloc(sizeof(GENERIC_VECTOR) * MAX_WORD_VECTORS);

    init_vector_arr(vector_arr);
    fill_vector_arr_from_dictionary(vector_arr);

    Boolean play_bool = TRUE;

    do
    {
       game(&play_bool, vector_arr);
    }

    while (play_bool == TRUE);

    destroy_vector_arr(vector_arr);
    
    return 0;
}

void game(Boolean* play_bool, GENERIC_VECTOR* vector_arr)
{
    int word_size;
    char guess;
    int guess_count;
    Boolean show_running_total = TRUE;

    prompt_word_size(&word_size);
    prompt_guess_count(&guess_count);
    prompt_running_total(&show_running_total);

    MY_STRING guesses = my_string_init_default();

    MY_STRING new_key = my_string_init_default();
    GENERIC_VECTOR current_word_family_vector = vector_arr[word_size];
    Boolean current_word_family_is_vector_arr = TRUE;

    // get a blank current_word_family_key to start
    MY_STRING current_word_family_key = my_string_init_dash(word_size);

    while(guess_count != 0)
    {
        pre_guess_dialogue(guess_count, guesses, current_word_family_key);
        prompt_guess(&guess, guesses);

        AVL_TREE avl = avl_tree_init_default();

        for(int i = 0; i < generic_vector_get_size(current_word_family_vector); i++)
        {
            get_word_key_value(current_word_family_key, new_key, *generic_vector_at(current_word_family_vector, i), guess);
            avl_tree_insert(avl, new_key, *generic_vector_at(current_word_family_vector, i));
        }

        // this is to compare with next word key to see if guess contains
        MY_STRING old_current_word_family_key = my_string_init_copy(current_word_family_key);

        // reset current_word_family_vector if guess count more than 1
            // Check current_word_family and ensure it is not vector_arr (which gets destroyed in main)
        
        if (current_word_family_is_vector_arr == FALSE)
            generic_vector_destroy(&current_word_family_vector);
        
        // get the next word_family_vector, as well as the current_word_family_key  (pass by reference)

        current_word_family_vector = avl_tree_find_largest(avl, current_word_family_key);
        
        // if statement reduces multiple checks
        if (current_word_family_is_vector_arr == TRUE)
            current_word_family_is_vector_arr = FALSE;

        post_guess_debug_dialogue(show_running_total, avl, current_word_family_vector);

        post_guess_correct_guess_dialogue(current_word_family_key, old_current_word_family_key, guess);

        guess_count--;
        avl_tree_destroy(&avl);
        my_string_destroy(&old_current_word_family_key);

        Boolean game_finished = post_game_dialogue(current_word_family_vector, current_word_family_key, guess_count);

        // win or lost
        if (game_finished == TRUE)
        {
            generic_vector_destroy(&current_word_family_vector);
            break;
        }
        // else, continue interating
    
    }
    my_string_destroy(&guesses);
    my_string_destroy(&new_key);
    my_string_destroy(&current_word_family_key);

    prompt_play_again(play_bool);
}

void init_vector_arr(GENERIC_VECTOR* vector_arr)
{
    for (int i = 0; i < MAX_WORD_VECTORS; i++)
    {
        vector_arr[i] = (GENERIC_VECTOR) generic_vector_init_default(vector_my_string_init_copy, vector_my_string_destroy);
    }
}

void fill_vector_arr_from_dictionary(GENERIC_VECTOR* vector_arr)
{
    MY_STRING hMy_string = NULL;
    FILE* fp; 
    fp = fopen("dictionary.txt", "r");
    hMy_string = my_string_init_default();

    for (int i = 1; i < MAX_WORD_VECTORS; i++)
    {
        while (my_string_extraction(hMy_string, fp))
        {
            if (my_string_get_size(hMy_string) == i)
            {
                generic_vector_push_back(vector_arr[i], hMy_string);
            }
            fgetc(fp);
        }

        fseek(fp, 0, SEEK_SET);
    }

    my_string_destroy(&hMy_string);
    fclose(fp);
}

void destroy_vector_arr(GENERIC_VECTOR vector_arr[])
{
    for(int i = 0; i < MAX_WORD_VECTORS; i++)
    {
        generic_vector_destroy(&vector_arr[i]);
    }
    free(vector_arr);
}

ITEM vector_my_string_init_copy(ITEM hMy_string)
{
    MY_STRING hCopy = my_string_init_copy(hMy_string);

    return hCopy;
}

void vector_my_string_destroy(ITEM* phMy_string)
{
    MY_STRING hMy_string = (MY_STRING)*phMy_string;

    my_string_destroy(&hMy_string);
}

void clear_keyboard_buffer(void)
{ 
    char c = 'c';

    while (c != '\n')
    {
        scanf("%c", &c);
    }
}

void prompt_guess(char* guess, MY_STRING guesses)
{
    int noc;
    printf("Enter guess: ");
    noc = scanf("%c", guess);
    clear_keyboard_buffer();

    while (noc != 1 || !isalpha(*guess))
    {
        printf("Invalid input. Please enter a character\n");
        printf("Enter guess: ");
        noc = scanf("%c", guess);
        clear_keyboard_buffer();

    }

    *guess = tolower(*guess);
    my_string_push_back(guesses, ' ');
    my_string_push_back(guesses, *guess);
}

void prompt_word_size(int* word_length)
{
    int noc;
    printf("What length word do you want to play with?: ");
    noc = scanf("%d", word_length);
    clear_keyboard_buffer();

    while (noc != 1 || *word_length <= 1 || *word_length > 29 || *word_length == 23 || *word_length == 25 || *word_length == 26 || *word_length == 27 )
    {
        if (*word_length == 23 || *word_length == 25 || *word_length == 26 || *word_length == 27)
        {
            printf("The dictionary has no words of length %d. Please enter another integer\n", *word_length);
        }
        else
        {
            printf("Invalid input. Please enter an integer larger than 1.\n");
        }
        printf("What length word do you want to play with?: ");
        noc = scanf("%d", word_length);
        clear_keyboard_buffer();
    }
}

void prompt_guess_count(int* guess_count)
{
    int noc;
    printf("How many guesses would you like to play with?: ");
    noc = scanf("%d", guess_count);
    clear_keyboard_buffer();

    while (noc != 1 || *guess_count <= 0 )
    {
        printf("Invalid input. Please enter an integer larger than 0.\n");
        printf("How many guesses would you like to play with?: ");
        noc = scanf("%d", guess_count);
        clear_keyboard_buffer();
    }
}

void prompt_play_again(Boolean* play_bool)
{
    int noc;
    char input;

    printf("Would you like to play again (y / n): ");

    noc = scanf("%c", &input);

    clear_keyboard_buffer();

    while (noc != 1 || (input != 'y' && input != 'Y' && input != 'N' && input != 'n'))
    {
    printf("Invalid input. You must enter either y or n\n");
    printf("Would you like to play again (y / n): ");
    noc = scanf("%c", &input);
    clear_keyboard_buffer();
    }

    if (input == 'y' || input == 'Y')
    {
        *play_bool = TRUE;
    }
    else
    {
        *play_bool = FALSE;
    }
}

void prompt_running_total(Boolean* show_running_total)
{
    int noc;
    char input;

    printf("Would you like to show the running total (y / n): ");

    noc = scanf("%c", &input);

    clear_keyboard_buffer();

    while (noc != 1 || (input != 'y' && input != 'Y' && input != 'N' && input != 'n'))
    {
        printf("Invalid input. You must enter either y or n\n");
        printf("Would you like to show the running total (y / n): ");
        noc = scanf("%c", &input);
        clear_keyboard_buffer();
    }

    if (input == 'y' || input == 'Y')
    {
        *show_running_total = TRUE;
    }
    else
    {
        *show_running_total = FALSE;
    }
}

void pre_guess_dialogue(int guess_count, MY_STRING guesses, MY_STRING current_word_family_key)
{
    printf("\nYou have %d guesses left\n", guess_count);
    printf("Used letters: %s\n", my_string_c_str(guesses));
    printf("Word: %s\n\n", my_string_c_str(current_word_family_key));
}

void post_guess_debug_dialogue(Boolean show_running_total, AVL_TREE avl, GENERIC_VECTOR current_word_family_vector)
{
    if (SHOW_ALL_WORD_POSSIBILITIES == TRUE)
    {
        avl_tree_traverse_with_key(avl);
    }

    if (SHOW_ALL_WORD_FAMILYS == TRUE)
    {
        avl_tree_traverse(avl);

    }

    if (show_running_total == TRUE)
    {
        printf("-----The computer has %d possibilities remaining-----\n", generic_vector_get_size(current_word_family_vector));
    }
}

void post_guess_correct_guess_dialogue(MY_STRING current_word_family_key, MY_STRING old_current_word_family_key, char guess)
{
    if (my_string_compare(current_word_family_key, old_current_word_family_key) == 0)
    {
        printf("I'm sorry, there were no %c's in the word\n", guess);
    }
    else
    {
        printf("Lucky guess! The word contains %c\n", guess);
    }

}

Boolean post_game_dialogue(GENERIC_VECTOR current_word_family_vector, MY_STRING current_word_family_key, int guess_count)
{

    if (my_string_compare(current_word_family_key, *generic_vector_at(current_word_family_vector, 0)) == 0)
    {
        printf("Congrats! You have won the game! The correct word was %s\n", my_string_c_str(*generic_vector_at(current_word_family_vector, 0)));
        return TRUE;
    }
    else if (guess_count == 0)
    {
        int rand_index = rand() % (generic_vector_get_size(current_word_family_vector));

        printf("\nWhoops, you've lost! The word I was thinking of was:  %s\n", my_string_c_str(*generic_vector_at(current_word_family_vector, rand_index)));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
