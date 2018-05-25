#include "utilities.h"

// Generate a random letter
char randomLetter()
{
    // Drawing a letter
    char result = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random() % 26];

    // Drawing capital or not
    int capital = random() % 2;
    if(capital) result = tolower(result);
    return result;
}