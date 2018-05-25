#include "recordsLib.h"

// Generate a file with an array of records - library verions
FILE* generateLib(char* fileName, size_t recordsAmount, size_t recordSize)
{
    // Parameters checking
    if(recordsAmount < 1)
    {
        printf("%s", "RecordsAmount parameter invalid in 'generate' function.");
        return NULL;
    }
    if(recordSize < 1)
    {
        printf("%s", "RecordSize parameter invalid in 'generate' function.");
        return NULL;
    }
    if(fileName == NULL)
    {
        printf("%s", "fileName parameter is NULL in 'generate' function.");
        return NULL;
    }

    // File opening
    FILE* file;
    if((file = fopen(fileName,"w+t")) == NULL)
    {
        printf("%s", "File opening error.");
        return NULL;
    }

    // Temp buffer
    char temp;

    // Writing an randomly generated array of records to file
    for(size_t i=0;i<recordsAmount;i++)
    {
        for(size_t j=0;j<recordSize;j++)
        {
            temp = randomLetter();
            fwrite(&temp, sizeof(char),1,file);
        }
        temp = '\n';
        fwrite(&temp, sizeof(char),1,file);
    }
    return file;
}

// Get a record at specified index 'i' to a buffer - library version
void getRecordLib(char* buffer, size_t i, FILE* file, size_t recordSize)
{
    // Move file pointer to the [i][j] location
    // '+1' because of '\n' char at the end of the each record
    fseek(file,(int)((recordSize+1)*i),SEEK_SET);

    // Get the record
    fread(buffer, sizeof(char), recordSize, file);
}


// Print array to standard output - library version
void printLib(FILE* file, size_t recordsAmount, size_t recordSize)
{
    // Parameters checking
    if(recordsAmount < 1)
    {
        printf("%s", "RecordsAmount parameter invalid in 'generate' function.");
        return;
    }
    if(recordSize < 1)
    {
        printf("%s", "RecordSize parameter invalid in 'generate' function.");
        return;
    }
    if(file == NULL)
    {
        printf("%s", "file parameter is NULL in 'generate' function.");
        return;
    }

    // Temp buffer
    char buffer[recordSize];

    // Set a file pointer at the beginning of file
    fseek(file,0,SEEK_SET);

    // Reading and printing
    for(size_t i=0;i<recordsAmount;i++)
    {
        getRecordLib(buffer,i,file,recordSize);
        for(size_t i = 0; i < recordSize; i++) printf("%c", buffer[i]);
        printf("%c", '\n');
    }
    printf("%c", '\n');
}

// Set a record at specified index 'i' to a value specified in buffer - library version
void setRecordLib(char* buffer, size_t i, FILE* file, size_t recordSize)
{
    // Move file pointer to the [i][j] location
    // '+1' because of '\n' char at the end of the each record
    fseek(file,(int)((recordSize+1)*i),SEEK_SET);

    // Set the record
    fwrite(buffer, sizeof(char), recordSize, file);
}

// Sort array of records in array - library version
void sortLib(FILE* file, size_t recordsAmount, size_t recordSize)
{
    // Parameter checking
    if(recordsAmount <= 1) return;
    if(file == NULL) return;

    // Temp parameters
    char a[recordSize];
    char b[recordSize];

    // Sorting
    for(size_t i = 1; i < recordsAmount; i++)
    {
        size_t j=i;

        // Getting records from file
        getRecordLib(a, j, file, recordSize);
        getRecordLib(b, j-1, file, recordSize);
        while(j > 0 && a[0] < b[0])
        {
            // Swaping records in file
            setRecordLib(a,j-1,file,recordSize);
            setRecordLib(b,j,file,recordSize);

            // Getting next records from file
            j--;
            getRecordLib(a, j, file, recordSize);
            getRecordLib(b, j-1, file, recordSize);
        }
    }
}

//Copy records from one file to another using one buffer of given size only
void copyLib(FILE* fileSource, FILE* fileDest, size_t recordsAmount, size_t recordSize)
{
    // Parameters checking
    if(fileSource == NULL)
    {
        printf("%s", "fileSource parameter is NULL in 'copySys' function.");
        return;
    }
    if(fileDest == NULL)
    {
        printf("%s", "fileDest parameter is NULL in 'copySys' function.");
        return;
    }
    if(recordsAmount < 1)
    {
        printf("%s", "RecordsAmount parameter invalid in 'copySys' function.");
        return;
    }
    if(recordSize < 1)
    {
        printf("%s", "RecordSize parameter invalid in 'copySys' function.");
        return;
    }

    // Temp buffer
    /*
    There should be '\n' char at the end of the record in file,
    hence 'recordSize+1' and assignment of '\n' to the last byte of buffer
    */
    char buffer[recordSize+1];
    buffer[recordSize]  = '\n'; 

    // Set a file pointers at the beginning of files
    fseek(fileSource,0,SEEK_SET);
    fseek(fileDest,0,SEEK_SET);

    // Reading and printing
    for(size_t i=0;i<recordsAmount;i++)
    {
        getRecordLib(buffer,i,fileSource,recordSize);
        fwrite(buffer, sizeof(char), (size_t)(recordSize+1), fileDest);
    }
}