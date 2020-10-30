#include <stdio.h>
#include <stdlib.h>
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
    int price;     //In Danish Crowns
    int height;    //In meters
    int wing_span; //In meters
    int kWh;       //Power production of windmill
};

struct Area
{
    char name[50];
    region region;            //Region of the area
    double wind_speed;        //Meters/second
    int in_sea;               //Is equal to 1 if the area is at sea
    double land_height;       //In meters
    double roughness;         //Between 0..4
    double dist_to_house;     //In km
    double dist_to_powergrid; //In km
    double expenses;          //In Danish Crowns
};

//Prototypes
void print_image(FILE *fptr);
int clean_stdin();
void user_input(int *region, int *wind_turbine, int *priority);
void print_area(struct Area area);
double calc_total_expenses(struct Area area, struct Windmill windmill);
double calc_area_expenses(struct Area area);
double calc_terrain_expenses(struct Area area);
double calc_digging_expenses(struct Area area);
double calc_roughness_expenses(struct Area area);
const char *get_region(struct Area area);
double calc_power_output(struct Area area, struct Windmill windmill);
double calc_wind_shear(struct Area area, struct Windmill windmill);
int exp_comparator(const void *p, const void *q);
void print_struct_array(struct Area *array, size_t len);

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

    struct Area Location[2];

    strcpy(Location[0].name, "Københavns lufthavn");
    Location[0].wind_speed = 5.1;
    Location[0].region = Hovedstaden;
    Location[0].in_sea = 0;
    Location[0].land_height = 2;
    Location[0].roughness = 1;
    Location[0].dist_to_house = 120;
    Location[0].dist_to_powergrid = 5;
    Location[0].expenses = calc_area_expenses(Location[0]);

    strcpy(Location[1].name, "Aarhus Lufthavn");
    Location[1].wind_speed = 3.6;
    Location[1].region = Midtjylland;
    Location[1].in_sea = 0;
    Location[1].land_height = 30;
    Location[1].roughness = 0.4;
    Location[1].dist_to_house = 2000;
    Location[1].dist_to_powergrid = 5;
    Location[1].expenses = calc_area_expenses(Location[1]);

    //Image file printer
    char *filename = "image.txt";
    FILE *fptr = NULL;

    if ((fptr = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "error opening %s\n", filename);
        return 1;
    }

    print_image(fptr);
    fclose(fptr);

    //print_area(Copenhagen);
    //printf("Power output: %f", calc_power_output(Copenhagen));
    int region, wind_turbine, priority;
    user_input(&region, &wind_turbine, &priority);

    double total_expense = calc_total_expenses(Location[0], Vestas);
    printf("[Name: %s \t Area expense: %.2f \t Total area expense: %.2f\n",Location[0].name, Location[0].expenses, total_expense);
    
    /* --------------------------------------------------------- */
    size_t arr_len = sizeof(Location) / sizeof(struct Area);

    /* sorting structs using qsort() example */ 
    qsort(Location, arr_len, sizeof(struct Area), exp_comparator);

    /* print sorted structs */
    print_struct_array(Location, arr_len);

    /* print wind shear of cheapest location*/
    printf("Vindhastighed for %s i højden %d m er: %.2f m/s\n", Location[(int) region + 1].name, Vestas.height, calc_wind_shear(Location[(int) region + 1], Vestas));
    
    return 0;
}

//Funktion som printer image.txt fil
void print_image(FILE *fptr)
{
    char read_string[MAX_LEN];

    while (fgets(read_string, sizeof(read_string), fptr) != NULL)
        printf("%s", read_string);

    printf("\n");
}

int clean_stdin()
{
    while (getchar()!='\n');
    return 1;
}

void user_input(int *region, int *wind_turbine, int *priority)
{
    int input = 0;
    char c;

    do
    {
        printf("Vælg region:\n1. Hovedstaden\n2. Sydjylland\n3. Nordjylland\n4. Midtjylland\n5. Sjælland\n");
    }

    while(((scanf("%d%c", &input, &c)!=2 || c!='\n') && clean_stdin()) || input < 1 || input > 5);
    
    *region = input;

    do
    {
        printf("Vælg vindmølle:\n1. Vestas\n2. Siemens\n");
    }

    while(((scanf("%d%c", &input, &c)!=2 || c!='\n') && clean_stdin()) || input < 1 || input > 2);

    *wind_turbine = input;

    do
    {
        printf("Vælg prioritet:\n1. Prioritér laveste omkostninger\n2. Prioritér højeste energiproduktion\n");
    }

    while(((scanf("%d%c", &input, &c)!=2 || c!='\n') && clean_stdin()) || input < 1 || input > 2);

    *priority = input;
}

void print_area(struct Area area)
{
    printf("name: %s\n", area.name);
    printf("Region: %s\n", get_region(area));
    printf("Wind Speed: %.2f\n", area.wind_speed);
    printf("in_sea: %d\n", area.in_sea);
    printf("Total expenses: %.2f\n", area.expenses);
}
//--------------------Expense calculation functions-------------------
double calc_total_expenses(struct Area area, struct Windmill windmill)
{
    double total_expense = 0;

    total_expense =
        area.expenses +
        windmill.price;

    return (total_expense);
}

double calc_area_expenses(struct Area area)
{
    double area_expense = 0;

    area_expense =
        calc_terrain_expenses(area) +
        calc_digging_expenses(area) +
        calc_roughness_expenses(area);

    return (area_expense);
}

//Not actual calculations
double calc_terrain_expenses(struct Area area)
{
    return (area.in_sea * 1000000);
}

double calc_digging_expenses(struct Area area)
{
    int PRICE_PER_KM = 200000;
    return (area.dist_to_powergrid * PRICE_PER_KM);
}

//Not actual calculations
double calc_roughness_expenses(struct Area area)
{
    return (area.roughness * 10000);
}
//---------------------------------------------------------------------
const char *get_region(struct Area area)
{
    switch (area.region)
    {
    case 0:
        return ("Hovedstaden");
        break;
    case 1:
        return ("Sydjylland");
        break;
    case 2:
        return ("Nordjylland");
        break;
    case 3:
        return ("Midtjylland");
        break;
    case 4:
        return ("Sjaelland");
        break;
    default:
        return ("Unknown region");
        break;
    }
}
//--------------------Sorting algorithm functions-------------------

//Comparator function for sorting areas expenses from high to low
int exp_comparator(const void *p, const void *q) 
{ 
    struct Area *p1 = (struct Area *)p;
    struct Area *p2 = (struct Area *)q;
    return (int)(100.f*p2->expenses - 100.f*p1->expenses);
} 

/* Example struct array printing function */ 
void print_struct_array(struct Area *array, size_t len) 
{ 
    size_t i;
 
    for(i=0; i<len; i++) 
        printf("[ Name: %s \t Expense: %.2f DKK]\n", array[i].name, array[i].expenses);
 
    puts("--");
}
//---------------------------------------------------------------------

double calc_power_output(struct Area area, struct Windmill windmill)
{
    double wind_turbine_efficiency = 0.35;
    double air_dens = 1.2;
    double v = calc_wind_shear(area, windmill);
    double r = windmill.wing_span / 2;

    return(M_PI/2*pow(r,2)*pow(v,3)*air_dens*wind_turbine_efficiency);
}

double calc_wind_shear(struct Area area, struct Windmill windmill)
{
    double wind_shear, roughness_length;

    //Converts roughness of the given area into roughness length
    if(area.roughness == 0)
        roughness_length = 0.0002;

    else if (area.roughness == 0.5)
        roughness_length = 0.0024;

    else if (area.roughness == 1)
        roughness_length = 0.03;

    else if (area.roughness == 1.5)
        roughness_length = 0.055;

    else if (area.roughness == 2)
        roughness_length = 0.1;

    else if (area.roughness == 2.5)
        roughness_length = 0.2;

    else if (area.roughness == 3)
        roughness_length = 0.4;

    else if (area.roughness == 3.5)
        roughness_length = 0.8;

    else if (area.roughness == 4)
        roughness_length = 1.6;

    //formula for finding exact windspeed at a given height
    wind_shear = area.wind_speed * (log(windmill.height/roughness_length)/(log(10/roughness_length))); 

    return (wind_shear);
}