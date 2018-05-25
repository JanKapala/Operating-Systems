#include "recordsSys.h"

// Generate a file with an array of records - system verions
int generateSys(char* fileName, size_t recordsAmount, size_t recordSize)
{
    // Parameters checking
    if(recordsAmount < 1)
    {
        printf("%s", "RecordsAmount parameter invalid in 'generate' function.");
        return -1;
    }
    if(recordSize < 1)
    {
        printf("%s", "RecordSize parameter invalid in 'generate' function.");
        return -1;
    }
    if(fileName == NULL)
    {
        printf("%s", "fileName parameter is NULL in 'generate' function.");
        return -1;
    }

    // File opening
    int file;
    if((file = open(fileName, O_RDWR | O_TRUNC | O_CREAT, 0666)) < 0)
    {
        printf("%s", "File opening error.");
        return -1;
    }

    // Temp buffer
    char temp;

    // Writing an randomly generated array of records to file
    for(size_t i=0;i<recordsAmount;i++)
    {
        for(size_t j=0;j<recordSize;j++)
        {
            temp = randomLetter();
            write(file, &temp, 1);
        }
        temp = '\n';
        write(file, &temp, 1);
    }
    return file;
}

// Get a record at specified index 'i' to a buffer - system version
void getRecordSys(char* buffer, size_t i, int file, size_t recordSize)
{
    // Move file pointer to the [i][j] location
    // '+1' because of '\n' char at the end of the each record
    lseek(file,(off_t)((recordSize+1)*i),SEEK_SET);

    // Get the record
    read(file, buffer, sizeof(char)*recordSize);
}

// Print array to standard output - system version
void printSys(int file, size_t recordsAmount, size_t recordSize)
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
    if(file < 0)
    {
        printf("%s", "file parameter is NULL in 'generate' function.");
        return;
    }

    // Temp buffer
    char buffer[recordSize];

    // Set a file pointer at the beginning of file
    lseek(file,0,SEEK_SET);

    // Reading and printing
    for(size_t i=0;i<recordsAmount;i++)
    {
        getRecordSys(buffer,i,file,recordSize);
        for(size_t i = 0; i < recordSize; i++) printf("%c", buffer[i]);
        printf("%c", '\n');
    }
    printf("%c", '\n');
}

// Set a record at specified index 'i' to a value specified in buffer - system version
void setRecordSys(char* buffer, size_t i, int file, size_t recordSize)
{
    // Move file pointer to the [i][j] location
    // '+1' because of '\n' char at the end of the each record
    lseek(file,(off_t)((recordSize+1)*i),SEEK_SET);

    // Get the record
    write(file, buffer, sizeof(char)*recordSize);
}

// Sort array of records in array - system version
void sortSys(int file, size_t recordsAmount, size_t recordSize)
{
    // Parameter checking
    if(recordsAmount <= 1) return;
    if(file < 0) return;

    // Temp parameters
    char a[recordSize];
    char b[recordSize];

    // Sorting
    for(size_t i = 1; i < recordsAmount; i++)
    {
        size_t j=i;

        // Getting records from file
        getRecordSys(a, j, file, recordSize);
        getRecordSys(b, j-1, file, recordSize);
        while(j > 0 && a[0] < b[0])
        {
            // Swaping records in file
            setRecordSys(a,j-1,file,recordSize);
            setRecordSys(b,j,file,recordSize);

            // Getting next records from file
            j--;
            getRecordSys(a, j, file, recordSize);
            getRecordSys(b, j-1, file, recordSize);
        }
    }
}

//Copy records from one file to another using one buffer of given size only
void copySys(int fileSource, int fileDest, size_t recordsAmount, size_t recordSize)
{
    // Parameters checking
    if(fileSource < 0)
    {
        printf("%s", "fileSource parameter is NULL in 'copySys' function.");
        return;
    }
    if(fileDest < 0)
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
    lseek(fileSource,0,SEEK_SET);
    lseek(fileDest,0,SEEK_SET);

    // Reading and printing
    for(size_t i=0;i<recordsAmount;i++)
    {
        getRecordSys(buffer,i,fileSource,recordSize);
        write(fileDest, buffer, recordSize+1);
    }
}