#include <stdio.h>
#include <math.h>
#define MAX_LEN 128

void print_image(FILE *fptr);

calc_data(double vh);

int main(void)
{
    double data = 2.0;
    printf("Hello Github\n");

    char *filename = "image.txt";
    FILE *fptr = NULL;
 
    if((fptr = fopen(filename,"r")) == NULL)
    {
        fprintf(stderr,"error opening %s\n",filename);
        return 1;
    }
 
    print_image(fptr);
 
    fclose(fptr);
 
    printf("Vind hastigheden er: %f", get_data(data));
    return 0;
    
}

calc_data(double vh) {

    return vh;
}