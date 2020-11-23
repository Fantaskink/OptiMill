#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_LEN 128 //For image printing. Laurits ved det
#define AREA_SIZE 25 //Amount of areas
#define PRICE_PER_KW 0.2
#define HOURS_IN_DAY 24
#define HOURS_IN_WEEK 168
#define HOURS_IN_MONTH 732
#define HOURS_IN_YEAR 8784
#define PRICE_PER_KM 200000

//Enumerators
typedef enum region
{
    Hovedstaden,
    Syddanmark,
    Nordjylland,
    Midtjylland,
    Sjaelland
} region;

//Structs
typedef struct Windmill
{
    char name[50];            //Name of the windmill
    int price;                //In Danish Crowns
    int height;               //In meters
    int wing_span;            //In meters
    int kWh;                  //Power production of windmill
    int lifespan;             //Lifespan of windmill
} Windmill;

typedef struct Area
{
    int id;                   //ID of the area
    char name[50];            //Name of the area
    region region;            //Region of the area
    double wind_speed;        //Raw wind speed of area in meters per second
    int in_sea;               //Is equal to 1 if the area is at sea
    double land_height;       //In meters
    double roughness;         //Roughness class: 0..4
    double dist_to_house;     //In km
    double dist_to_powergrid; //In km
    double expenses;          //In Danish Crowns
    double total_expenses;    //Expenses including wind turbine expenses
    double kwh_output;        //kWh output of the specific area with regards to chosen wind turbine
    double inv_return;        //Monetary return of specific windmill if placed in specific area
} Area;

//Prototypes
void print_image(FILE *fptr);
int clean_stdin();
const char *get_input_region_name(int a);
const char *get_manufacturer(int a);
const char *get_input_priority(int a);
int get_region();
int get_wind_turbine();
int get_priority();
int get_budget();
int get_input(const char *string, int a, int b);
void print_area_data(Area area);
double calc_total_expenses(Area area, Windmill windmill);
double calc_area_expenses(Area area);
double sea_factor(Area area);
double calc_digging_expenses(Area area);
double calc_roughness_expenses(Area area);
const char *get_region_name(Area area);
double calc_power_output(Area area, Windmill windmill);
double calc_wind_shear(Area area, Windmill windmill);
double calc_windmill_income(Area area, Windmill windmill);
void print_windmill_investment_return(Area area, Windmill windmill);
int exp_comparator(const void *p, const void *q);
int kwh_comparator(const void *p, const void *q);
void print_struct_array(Area *array, size_t len, int in_region, int *f_index);

int main(void)
{
    //Temporary variables for loading structs
    int ID, IN_SEA, REGION;                   
    char NAME[50];                      
    double  WIND_SPEED, LAND_HEIGHT,
            ROUGHNESS, DIST_TO_HOUSE,
            DIST_TO_POWERGRID;


    //Create Struct Array
    Area area[AREA_SIZE];

    //Read from data file

    FILE* data = fopen("data.txt", "r");

    if (data == NULL)
    {
        printf("Kunne ikke indlæse datafilen!\n");
        exit(EXIT_FAILURE);
    }

    //Transfer all the data from data file into every field of struct array
    int i = 0;
    ////ID, NAVN, REGION, VINDHASTIGHED, I_VAND, LAND_HØJDE, RUGHEDSKLASSE, TIL_HUS, TIL_ELNET
    while (fscanf(data, "%d %s %d %lf %d %lf %lf %lf %lf",
                        &ID, NAME, &REGION, &WIND_SPEED, &IN_SEA, &LAND_HEIGHT, &ROUGHNESS,
                        &DIST_TO_HOUSE, &DIST_TO_POWERGRID) > 0)
    {
        area[i].id = ID;
        strcpy(area[i].name, NAME);
        area[i].region = REGION;
        area[i].wind_speed = WIND_SPEED;
        area[i].in_sea = IN_SEA;
        area[i].land_height = LAND_HEIGHT;
        area[i].roughness = ROUGHNESS;
        area[i].dist_to_house = DIST_TO_HOUSE;
        area[i].dist_to_powergrid = DIST_TO_POWERGRID;

        i++;
    }

    Windmill windmill[2];

    strcpy(windmill[0].name, "Vestas");
    windmill[0].price = 25202400;
    windmill[0].height = 200;
    windmill[0].wing_span = 162;
    windmill[0].kWh = 6000;

    strcpy(windmill[1].name, "Siemens");
    windmill[1].price = 18894090;
    windmill[1].height = 155;
    windmill[1].wing_span = 130;
    windmill[1].kWh = 4000;

    /* ------------------------- Optimill logo printer -------------------------------- */

    char *filename = "image.txt";
    FILE *fptr = NULL;

    if ((fptr = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Fejl ved indlæsning %s\n", filename);
        return 1;
    }

    print_image(fptr);
    fclose(fptr);

    /* ------------------------- Main code -------------------------------- */

    int f_index = 0;
    int quit = 0;
    int region; 
    int wind_turbine; 
    int priority;
    int budget; 
    size_t arr_len;
    
    //Get the array length of our Area struct
    /* Find ud af hvad sizeof(Area) giver*/
    arr_len = sizeof(area) / sizeof(Area);

    while (!quit)
    {
        region = get_region() - 1;
        budget = get_budget();
        wind_turbine = get_wind_turbine() - 1;
        priority = get_priority();

        printf("Dine valg:\n");
        printf("------------------------------------------------------\n");
        printf("Region:\t\t\t %s\n", get_input_region_name(region));
        printf("Budget:\t\t\t %d kr\n", budget);
        printf("Producent:\t\t %s\n", get_manufacturer(wind_turbine));
        printf("Prioritét\t\t %s\n", get_input_priority(priority));
        printf("------------------------------------------------------\n");

        //Calculate kwh_output and total expenses for all the areas
        for (int i = 0; i < arr_len; i++)
        {
            area[i].kwh_output = calc_power_output(area[i], windmill[wind_turbine]);
            area[i].total_expenses = calc_total_expenses(area[i], windmill[wind_turbine]);
            area[i].expenses = calc_area_expenses(area[i]);
            area[i].inv_return = calc_windmill_income(area[i], windmill[wind_turbine]);
        }

        //Run the sorting of areas given the priority from user
        switch (priority)
        {
        case 0: // Sort the Areas by expenses low -> high
            qsort(area, arr_len, sizeof(Area), exp_comparator);
            break;

        case 1: //Sort the Areas by kWh output high -> low
            qsort(area, arr_len, sizeof(Area), kwh_comparator);
            break;

        case 2:
            //qsort(area, arr_len, sizeof(Area), afkast(ikke lavet endnu)comparator);
            break;

        default:
            exit(EXIT_FAILURE);
            break;
        }

        //Print the sorted list
        print_struct_array(area, arr_len, region, &f_index);

        //Print out all the area data of all the areas in given region
        print_area_data(area[f_index]);

        quit = 1;
    }
    //---------------------------------------------------------------------
    return 0;
}

//Takes image.txt file and prints to terminal (Technically not our own code)
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

const char *get_input_region_name(int a)
{
    switch (a)
    {
    case 0:
        return("Hovedstaden");
        break;

    case 1:
        return("Syddanmark");
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
        return("Fejl");
        break;
    }
}

const char *get_manufacturer(int a)
{
    switch (a)
    {
    case 0:
        return("Vestas");
        break;
    
    case 1:
        return("Siemens");
        break;
        
    default:
        return("Fejl");
    }
}

const char *get_input_priority(int a)
{
    switch (a)
    {
    case 1:
        return("Omkostninger");
        break;
    
    case 2:
        return("Energiproduktion");
        break;

    case 3: 
        return("Årlig afkast");
        break;

    default:
        return("Fejl");
        break;
    }
}

int get_priority()
{
    char string[] = "Vælg prioritet:\n1. Prioritér laveste omkostninger\n2. Prioritér højeste energiproduktion\n";

    return(get_input(string, 1, 2));
}

int get_budget()
{
    char string[] = "Indtast budget i danske kroner: \n";

    return(get_input(string, 1, 2147483647));
}

int get_region()
{ 
    char string[] = "Vælg region:\n1. Hovedstaden\n2. Syddanmark\n3. Nordjylland\n4. Midtjylland\n5. Sjælland\n";
    
    return(get_input(string, 1, 5)); 
}

int get_wind_turbine()
{
    char string[] = "Vælg vindmølle:\n1. Vestas\n2. Siemens\n";

    return(get_input(string, 1, 2));
}

int get_input(const char *string, int a, int b)
{
    char c;
    int input;
    do
    {
        printf("%s", string);
    }

    while(((scanf("%d%c", &input, &c)!=2 || c!='\n') && clean_stdin()) || input < a || input > b);
    
    return(input);
}

//Prints all the data inside given area
void print_area_data(Area area)
{
    printf("------------------------------------------------------\n");
    printf("ID: \t\t\t  %d\n", area.id);
    printf("Navn:\t\t\t  %s\n", area.name);
    printf("Region:\t\t\t  %s\n", get_region_name(area));
    printf("Vindhastighed:\t\t  %.2f m/s\n", area.wind_speed);
    printf("På havet:\t\t  %d\n", area.in_sea);
    printf("Landhøjde:\t\t  %.2f m\n", area.land_height);
    printf("Ruhedsklasse:\t\t  %.2f\n", area.roughness);
    printf("Afstand til nærmeste hus: %.2f km\n", area.dist_to_house);
    printf("Afstand til elnettet:\t  %.2f km\n", area.dist_to_powergrid);
    printf("Samlede kwh produktion:\t  %.2f kW\n", area.kwh_output);
    printf("Terrænomkostninger:\t  %.2f kr\n", area.expenses);
    printf("Samlede omkostninger:\t  %.2f kr\n", area.total_expenses);
    printf("------------------------------------------------------\n");
}

//--------------------Expense calculation functions-------------------
double calc_total_expenses(Area area, Windmill windmill)
{
    double total_expense = 0;

    total_expense = (area.expenses + windmill.price);

    return(total_expense);
}

double calc_area_expenses(Area area)
{
    double area_expense = 0;

    area_expense =
        calc_digging_expenses(area) +
        calc_roughness_expenses(area) *
        sea_factor(area);

    return(area_expense);
}

//Approximation of wind turbine cost when constructed on the sea
double sea_factor(Area area)
{
    if(area.in_sea > 0)
    {
        return(2.6);
    }
    else
    {
        return(1);
    }
}

//Approximation of cable excavation and construction fees
double calc_digging_expenses(Area area)
{
    return(area.dist_to_powergrid * PRICE_PER_KM);
}

//Not actual calculations
double calc_roughness_expenses(Area area)
{
    return(area.roughness * 10000);
}

//---------------------------------------------------------------------
const char *get_region_name(Area area)
{
    switch (area.region)
    {
    case 0:
        return("Hovedstaden");
        break;

    case 1:
        return("Syddanmark");
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
        return("Ukendt region");
        break;
    }
}

//--------------------Sorting algorithm functions-------------------

//Comparator function for sorting areas expenses from high to low
int exp_comparator(const void *p, const void *q) 
{ 
    Area *p1 = (Area *)p;
    Area *p2 = (Area *)q;
    return (int)(100.f*p1->total_expenses - 100.f*p2->total_expenses);
} 

//Comparator function sorting areas kwh output from high to low
int kwh_comparator(const void *p, const void *q)
{
    Area *p1 = (Area *)p;
    Area *p2 = (Area *)q;
    return(int)(100.f*p2->kwh_output - 100.f*p1->kwh_output);
}

// Prints the sorted struct in the given region and returns the first index in that list  //
void print_struct_array(Area *array, size_t len, int in_region, int *f_index) 
{ 
    int count = 0;
    size_t i;
 
    printf("ID: \t Navn: \t\t Samlede omkostninger (kr): \t Energiproduktion (kW) \n");
    for(i=0; i<len; i++)
    {
        //Printer de områder ud der har samme region som den indtastede region
        if ((int) array[i].region == in_region)
        {
            count += 1;
            printf("%d \t%s \t %.2f \t\t %.2f\n", array[i].id, array[i].name, array[i].total_expenses, array[i].kwh_output);
            
            //Ensures the first index is only set one time.
            if (count == 1)
            {
                *f_index = i;
            }
        }
    }
}

//---------------------------------------------------------------------

double calc_power_output(Area area, Windmill windmill)
{
    double W;
    double wind_turbine_efficiency = 0.35;
    double air_dens = 1.2;
    double v = calc_wind_shear(area, windmill);
    double r = windmill.wing_span / 2;
    W = (M_PI/2) * pow(r,2) * pow(v,3) * air_dens * wind_turbine_efficiency;
    
    //Returns result in kW
    return W/1000;
}

double calc_wind_shear(Area area, Windmill windmill)
{
    double wind_shear, roughness_length;

    //Converts roughness of the given area into roughness length
    if(area.roughness == 0)
        roughness_length = 0.0002;

    else if(area.roughness == 0.5)
        roughness_length = 0.0024;

    else if(area.roughness == 1)
        roughness_length = 0.03;

    else if(area.roughness == 1.5)
        roughness_length = 0.055;

    else if(area.roughness == 2)
        roughness_length = 0.1;

    else if(area.roughness == 2.5)
        roughness_length = 0.2;

    else if(area.roughness == 3)
        roughness_length = 0.4;

    else if(area.roughness == 3.5)
        roughness_length = 0.8;

    else if(area.roughness == 4)
        roughness_length = 1.6;

    //Formula for finding exact windspeed at a given height
    wind_shear = area.wind_speed * (log(windmill.height/roughness_length)/(log(10/roughness_length))); 

    return(wind_shear);
}

double calc_windmill_income(Area area, Windmill windmill)
{
    double hourly_income = calc_power_output(area, windmill) * PRICE_PER_KW;
    return(hourly_income);
}

void print_windmill_investment_return(Area area, Windmill windmill)
{
     double hours, days, weeks, months, years, percent;
     double income = calc_windmill_income(area, windmill);
     
     hours = windmill.price / income;
     days = windmill.price / income / HOURS_IN_DAY;
     weeks = windmill.price / income / HOURS_IN_WEEK;
     months = windmill.price / income / HOURS_IN_MONTH;
     years = windmill.price / income / HOURS_IN_YEAR;
     percent = ((income * HOURS_IN_YEAR) / windmill.price) * 100;

     printf("Vindmøllen tjener: %lf Kr. i timen\n", income);
     printf("Tid indtil vindmøllen har betalt for sig selv:\n");
     printf("%lf år, %lf måneder, %lf uger, %lf dage, %lf timer\n", years, months, weeks, days, hours);
     printf("Altså har investeringen et årligt afkast på: %.2lf %%\n", percent);
}