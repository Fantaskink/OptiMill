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


    struct Area Omraade1;

    strcpy( Omraade1.name, "København");

    Omraade1.wind_speed = 4;
    Omraade1.region = 0;
    Omraade1.in_sea = 0;
    Omraade1.land_height = 2;
    Omraade1.tree_factor = 0;
    Omraade1.roughness = 1;
    Omraade1.dist_to_house = 120;
    Omraade1.dist_to_powergrid = 5;
    Omraade1.windmill = Vestas, Siemens;
    Omraade1.expenses = calc_total_expenses(Omraade1);


    struct Area area2;
   
    strcpy (area2.name, "Sydjylland");
    
    area2.wind_speed = 8;
    area2.region = 1;
    area2.in_sea = 0;
    area2.land_height = 2;
    area2.tree_factor = 0.2;
    area2.roughness = 0.4;
    area2.dist_to_house = 2000;
    area2.dist_to_powergrid = 5;
    area2.windmill = Vestas, Siemens;
    area2.expenses = calc_total_expenses(area2);


    struct Area area3;

    strcpy (area3.name, "Nordjylland");

    area3.wind_speed = 4;
    area3.region = 2;
    area3.in_sea = 0;
    area3.land_height = 2;
    area3.tree_factor = 0.1;
    area3.roughness = 0.4;
    area3.dist_to_house = 2000;
    area3.dist_to_powergrid = 5;
    area3.windmill = Vestas, Siemens;
    area3.expenses = calc_total_expenses(area3);


    struct Area area4;

    strcpy (area4.name, "Midtjylland");

    area4.wind_speed = 4;
    area4.region = 3;
    area4.in_sea = 0;
    area4.land_height = 2;
    area4.tree_factor = 0.2;
    area4.roughness = 0.4;
    area4.dist_to_house = 2000;
    area4.dist_to_powergrid = 5;
    area4.windmill = Vestas, Siemens;
    area4.expenses = calc_total_expenses(area4);


    struct Area area5;

    strcpy (area5.name, "Sjælland");

    area5.wind_speed = 4;
    area5.region = 4;
    area5.in_sea = 0;
    area5.land_height = 2;
    area5.tree_factor = 0.3;
    area5.roughness = 0.4;
    area5.dist_to_house = 1000;
    area5.dist_to_powergrid = 5;
    area5.windmill = Vestas, Siemens;
    area5.expenses = calc_total_expenses(area5);



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
        return("Sjælland");
        break;
    default:
        return("Unknown region");
        break;
    }
}