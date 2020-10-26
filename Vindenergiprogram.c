#include <stdio.h>
#include <math.h>
#include <string.h>
#define MAX_LEN 128

//enumerators
typedef enum region 
{
    Hovedstaden, 
    Sydjylland, 
    Nordjylland, 
    Midtjylland, 
    Sjaelland
} region;

//Structs
struct Area
{
    char name[50];
    region region;
    double wind_speed; //Meters/second
    int inSea;
    double land_height; //in meters
    double tree_factor; //Between 0..1
    double roughness; //Between 0..1
    double dist_to_house; //In km
    double dist_to_powergrid; //In km
    double expenses; //In Danish Crowns
};

struct Windmill
{
    int price;
    int height;
    int wing_span;
    int kWh;
};

//Prototypes
void print_image(FILE *fptr);
double calc_data(double vh);
void print_area(struct Area area);
double calc_area_expenses(struct Area area);
const char* get_region(struct Area area);

int main(void)
{
    struct Area Omraade1;

    strcpy( Omraade1.name, "København");

    Omraade1.wind_speed = 4;
    Omraade1.region = 0;
    Omraade1.inSea = 0;
    Omraade1.land_height = 2;
    Omraade1.tree_factor = 0;
    Omraade1.roughness = 1;
    Omraade1.dist_to_house = 120;
    Omraade1.dist_to_powergrid = 5;
    Omraade1.expenses = calc_area_expenses(Omraade1);

    char *filename = "image.txt";
    FILE *fptr = NULL;
 
    if((fptr = fopen(filename,"r")) == NULL)
    {
        fprintf(stderr,"error opening %s\n",filename);
        return 1;
    }
 
    print_image(fptr);
    fclose(fptr);

    print_area(Omraade1);
    
 
    return 0;  
}

//Funktion som printer image.txt fil
void print_image(FILE *fptr)
{
    char read_string[MAX_LEN];
 
    while(fgets(read_string,sizeof(read_string),fptr) != NULL)
        printf("%s",read_string);

    printf("\n");
}

void print_area(struct Area area)
{

    printf("name: %s\n", area.name);
    printf("Region: %s\n", get_region(area)); 
    printf("Wind Speed: %.2f\n", area.wind_speed);
    printf("InSea: %d\n", area.inSea);
    printf("Expenses: %.2f\n", area.expenses);
}

double calc_area_expenses(struct Area area)
{
    double expenses = 0;

    expenses = (area.dist_to_powergrid * 20000) + 
    (area.tree_factor * 10000) +
    (area.roughness * 10000) +
    (area.inSea * 1000000);

    return(expenses);
}

const char* get_region(struct Area area)
{
        switch (area.region)
    {
    case 0:
        return("Hovedstaden");
        break;
    case 1:
        return("Sydjylland");
        break;
    case 2:
        return("Nordjylland");
        break;
    case 3:
        return("Midtjylland");
        break;
    case 4:
        return("Sjaelland");
        break;
    default:
        return("Unknown region");
        break;
    }
}