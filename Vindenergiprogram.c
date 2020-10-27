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
struct Windmill
{
    int price; //In Danish Crowns
    int height; //In meters
    int wing_span; //In meters
    int kWh; //Power production of windmill
};

struct Area
{
    char name[50];
    region region;
    double wind_speed; //Meters/second
    int in_sea;
    double land_height; //in meters
    double tree_factor; //Between 0..1
    double roughness; //Between 0..1
    double dist_to_house; //In km
    double dist_to_powergrid; //In km
    double expenses; //In Danish Crowns
    struct Windmill windmill; //Windmill used for calculating expenses
};

//Prototypes
void print_image(FILE *fptr);
void print_area(struct Area area);
double calc_total_expenses(struct Area area);
double calc_terrain_expenses(struct Area area);
double calc_logging_expenses(struct Area area);
double calc_digging_expenses(struct Area area);
double calc_roughness_expenses(struct Area area);
const char* get_region(struct Area area);

int main(void)
{
    struct Windmill Vestas;
    
    Vestas.price = 25202400;
    Vestas.height = 200;
    Vestas.wing_span = 162;
    Vestas.kWh = 6000;
    
    struct Windmill Siemens;

    Siemens.price = 18894090;
    Siemens.height = 155;
    Siemens.wing_span = 130;
    Siemens.kWh = 4000;


    struct Area Copenhagen;

    strcpy( Copenhagen.name, "København lufthavn");

    Copenhagen.wind_speed = 5.1;
    Copenhagen.region = Hovedstaden;
    Copenhagen.in_sea = 0;
    Copenhagen.land_height = 2;
    Copenhagen.tree_factor = 0;
    Copenhagen.roughness = 1;
    Copenhagen.dist_to_house = 120;
    Copenhagen.dist_to_powergrid = 5;
    Copenhagen.windmill = Vestas;
    Copenhagen.expenses = calc_total_expenses(Copenhagen);


    struct Area Aarhus;
   
    strcpy (Aarhus.name, "Aarhus Lufthavn");
    
    Aarhus.wind_speed = 3.6;
    Aarhus.region = Midtjylland;
    Aarhus.in_sea = 0;
    Aarhus.land_height = 30;
    Aarhus.tree_factor = 0.2;
    Aarhus.roughness = 0.4;
    Aarhus.dist_to_house = 2000;
    Aarhus.dist_to_powergrid = 5;
    Aarhus.windmill = Siemens;
    Aarhus.expenses = calc_total_expenses(Aarhus);

    //Image file printer
    char *filename = "image.txt";
    FILE *fptr = NULL;
 
    if((fptr = fopen(filename,"r")) == NULL)
    {
        fprintf(stderr,"error opening %s\n",filename);
        return 1;
    }
 
    print_image(fptr);
    fclose(fptr);

    print_area(Aarhus);
    
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
    printf("in_sea: %d\n", area.in_sea);
    printf("Windmill price: %d\n", area.windmill.price);
    printf("Total expenses: %.2f\n", area.expenses);
}
//Expense calculation functions
double calc_total_expenses(struct Area area)
{
    double expenses = 0;

    expenses = 
    calc_terrain_expenses(area) + 
    calc_digging_expenses(area) +
    calc_roughness_expenses(area) +
    calc_logging_expenses(area) +
    area.windmill.price;

    return(expenses);
}

double calc_terrain_expenses(struct Area area)
{
    return(area.in_sea * 1000000);
}

double calc_digging_expenses(struct Area area)
{
    int PRICE_PER_KM = 200000;
    return(area.dist_to_powergrid * PRICE_PER_KM);
}

double calc_roughness_expenses(struct Area area)
{
    return(area.roughness * 10000);
}

double calc_logging_expenses(struct Area area)
{
    return(area.tree_factor * 10000);
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

void init_structs(void) {
    
}