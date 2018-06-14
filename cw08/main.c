#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <linux/limits.h>

// Ascii pgm bitmaps handling------------------------------------------
void openASCIIPGM(char* path, int*** bitmap, int* H, int* V) // Works but no comments allowed
{
    FILE* file = fopen(path, "r");
    if(file==NULL) {printf("Error during openning file: %s. Exiting.\n", path); exit(0);}
    fscanf(file, "%*s\n%d %d\n%*d\n", H, V);

    *bitmap = calloc(*H, sizeof(int*));
    for(int i=0; i < *H; i++) (*bitmap)[i] = calloc(*V, sizeof(int));

    int temp;
    for(int i = 0; i < *H; i++)
    {
        for(int j = 0; j < *V; j++)
        {
            fscanf(file,"%d",&temp);
            (*bitmap)[i][j] = temp;
        }
    }

    fclose(file);
}

void printBitmap(int** bitmap, int h, int v) // TESTED
{
    for(int i=0;i<h;i++)
    {
        for(int j=0;j<v;j++) printf("%d ", bitmap[i][j]);
        printf("\n");
    }
}

void saveAsASCIIPGM(char* path, int** bitmap, int H, int V) // TESTED
{
    FILE* file = fopen(path, "w+");
    fprintf(file, "P2\n%d %d\n256\n", H, V);
    for(int i = 0; i < H; i++)
    {
        for(int j = 0; j < V; j++)
        {
            // for(int s = 0; s < 3 - countDigits(bitmap[i][j]); s++) fprintf(file, " ");
            fprintf(file, "%d ", bitmap[i][j]);
            if((i*V + j)%17 == 16) fprintf(file, "\n");
        }
    }
    fprintf(file, "\n");
    fclose(file);
}

// Filters handling----------------------------------------------------
void randomFilterType(double** k, int c, int i, int j, double distrVal) // TESTED
{
    if(i==j) k[i%c][i/c] = distrVal;
    else
    {
        double divisionA = (rand()/(float)RAND_MAX)*distrVal;
        double divisionB = distrVal-divisionA;
        randomFilterType(k, c, i, (i+j)/2, divisionA);
        randomFilterType(k, c, (i+j)/2+1, j, divisionB);
    }
}

double** genFilter(int c) // TESTED
{
    double** k = calloc(c, sizeof(double*));
    for(int i = 0; i < c; i++) k[i] = calloc(c, sizeof(double));

    randomFilterType(k, c, 0, c*c-1, 1.0);

    return k;
}

void openFilter(char* path, double*** k, int* c) // TESTED
{
    FILE* file = fopen(path, "r");
    if(file==NULL) {printf("Error during openning file: %s. Exiting.\n", path); exit(0);}

    fscanf(file, "%d\n", c);

    *k = calloc(*c, sizeof(int*));
    for(int i=0; i < *c; i++) (*k)[i] = calloc(*c, sizeof(int));

    double temp;
    for(int i = 0; i < *c; i++)
    {
        for(int j = 0; j < *c; j++)
        {
            fscanf(file, "%lf\n", &temp);
            (*k)[i][j] = temp;
        }
    }
    fclose(file);
}

void printFilter(double** k, int c) // TESTED
{
    for(int i=0;i<c;i++)
    {
        for(int j=0;j<c;j++) printf("%lf\n", k[i][j]);
        printf("\n");
    }
}

void saveAsFilter(char* path, double** k, int c) // TESTED
{
    FILE* file = fopen(path, "w+");
    fprintf(file, "%d\n", c);
    for(int i = 0; i < c; i++)
    {
        for(int j = 0; j < c; j++) fprintf(file, "%f\n", k[i][j]);
    }
    fclose(file);
}

// Filtering----------------------------------------------------------------
int filterPixel(int x, int y, int** bitmap, int H, int V, double** k, int c) // TESTED
{
    double rawPixel = 0;

    int index_i;
    int index_j;

    int x_offset = x-(int)ceil(c/2);
    int y_offset = y-(int)ceil(c/2);

    for(int i = x_offset; i < x_offset + c; i++)
    {
        index_i = i;
        if(index_i < 0) index_i = 0;
        if(index_i >= H) index_i = H-1;
        for(int j = y_offset; j < y_offset + c; j++)
        {
            index_j = j;
            if(index_j < 0) index_j = 0;
            if(index_j >= V) index_j = V-1;

            rawPixel += bitmap[index_i][index_j] * k[i-x_offset][j-y_offset];
        }
    }

    rawPixel = round(rawPixel);
    double result = ((int)(rawPixel))%256;
    if(result<0) result=0;
    return result;
}

typedef struct RenderInfo
{
    // Input/output
    int** bitmap;
    int** newBitmap;

    //Bitmap parameters
    int h;
    int v;

    //Filter parameters
    double** k;
    int c;

    // Region of rendering
    int start;
    int end;
} RenderInfo;

void* renderer(void* _arg) // TESTED
{
    RenderInfo* arg = (RenderInfo*)_arg;

    for(int i=arg->start; i<arg->end; i++)
    {
        for(int j=0; j<arg->v; j++)
        {
            arg->newBitmap[i][j] = filterPixel(i,j,arg->bitmap,arg->h, arg->v, arg->k, arg->c);
        }
    }

    pthread_exit(NULL);
}

int** filterBitmap(int** bitmap, int H, int V, double** k, int c, int N) // TESTED
{
    
    // Initializing
    int** newBitmap = calloc(H, sizeof(int*));
    for(int i=0;i<H;i++) newBitmap[i] = calloc(V, sizeof(int));
    

    pthread_t* threads = calloc(N,sizeof(pthread_t));

    RenderInfo* renderInfos = calloc(N,sizeof(RenderInfo));
    for(int i=0;i<N;i++)
    {
        renderInfos[i].bitmap=bitmap;
        renderInfos[i].newBitmap=newBitmap;
        renderInfos[i].c=c;
        renderInfos[i].h=H;
        renderInfos[i].k=k;
        renderInfos[i].v=V;
    }

    // Threads spawnning
    int width = ceil(H/(float)N);
    // printf("width: %d\n", width);
    int temperror;
    for(int i = 0; i < N-1; i++)
    {
        // printf("start=%d, end=%d\n", i*width, (i+1)*width);
        renderInfos[i].start=i*width;
        renderInfos[i].end=(i+1)*width;
        temperror = pthread_create(&(threads[i]), NULL, renderer, &(renderInfos[i]));
        if(temperror != 0) {printf("Error during creating thread no. %d. Exiting.\n", i); exit(0);}
    }
    // printf("Last: start=%d, end=%d\n", (N-1)*width, H);
    renderInfos[N-1].start=(N-1)*width;
    renderInfos[N-1].end=H;
    pthread_create(&(threads[N-1]), NULL, renderer, &(renderInfos[N-1]));

    // Joinning and cleanning
    for(int i=0; i<N; i++)
    {
        temperror = pthread_join(threads[i], NULL);
        if(temperror != 0) {printf("Error during joinning thread no. %d. Exiting.\n", i); exit(0);}
    }
    free(threads);
    free(renderInfos);

    return newBitmap;
}

int main(int argc, char** argv)
{
    // Arguments parsing
    if(argc != 5) {printf("Invalid number of arguments, provided: %d, required: 4\n", argc-1);exit(0);}
    
    int N;
    sscanf(argv[1], "%d", &N);
    if(N < 1)  {printf("Invalid value of first argument, provided: %s, required: natural number > 0\n", argv[1]);exit(0);}
    if(N > 128) {printf("Warring: probably provided number of thread (arg 1): %s is too big\n", argv[1]);}
    
    char inputBitmapPath[PATH_MAX];
    sscanf(argv[2], "%s", inputBitmapPath);
    char inputFilterPath[PATH_MAX];
    sscanf(argv[3], "%s", inputFilterPath);
    char outputBitmapPath[PATH_MAX];
    sscanf(argv[4], "%s", outputBitmapPath);

    // Diagnostic info
    // printf("max path: %d\n", PATH_MAX);
    // printf("Number of arguments: %d\n", argc);
    // printf("Number of threads: %d\n", N);
    // printf("Input bitmap path: %s\n", inputBitmapPath);
    // printf("Input filter path: %s\n", inputFilterPath);
    // printf("Output bitmap path: %s\n", outputBitmapPath);


    // Starting routiness
    srand(time(NULL));

    // Time measuring staff
    struct timeb start;
    struct timeb end;


    // Filtering
    int** bitmap;
    int h;
    int v;
    openASCIIPGM(inputBitmapPath, &bitmap, &h, &v);

    // // Random filter
    // int c1 = 3;
    // double** k1 = genFilter(c1);
    // saveAsFilter("./filters/random10.txt", k1, c1);


    double** k;
    int c;
    openFilter(inputFilterPath, &k, &c);

    ftime(&start);
    int** newBitmap = filterBitmap(bitmap, h, v, k, c, N);
    ftime(&end);

    saveAsASCIIPGM(outputBitmapPath, newBitmap, h, v);

    // Cleanning
    for(int i=0; i<h; i++) free(bitmap[i]);
    free(bitmap);
    for(int i=0; i<c; i++) free(k[i]);
    free(k);
    for(int i=0; i<h; i++) free(newBitmap[i]);
    free(newBitmap);

    time_t deltasec = end.time-start.time;
    unsigned short deltamsec = end.millitm-start.millitm;
    printf("Number of threads: %d, Time: %.3fs\n", N, deltasec+deltamsec/1000.0);


    return 0;
}