#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_LEN 128 //For image printing. Laurits ved det

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
    char name[50];            //Name of the windmill
    int price;                //In Danish Crowns
    int height;               //In meters
    int wing_span;            //In meters
    int kWh;                  //Power production of windmill
};

struct Area
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
};

//Prototypes
void print_image(FILE *fptr);
int clean_stdin();
int get_region();
int get_wind_turbine();
int get_priority();
int get_budget();
int get_input(const char *string, int a, int b);
void print_area_data(struct Area area);
double calc_total_expenses(struct Area area, struct Windmill windmill);
double calc_area_expenses(struct Area area);
double calc_terrain_expenses(struct Area area);
double calc_digging_expenses(struct Area area);
double calc_roughness_expenses(struct Area area);
const char *get_region_name(struct Area area);
double calc_power_output(struct Area area, struct Windmill windmill);
double calc_wind_shear(struct Area area, struct Windmill windmill);
int exp_comparator(const void *p, const void *q);
int kwh_comparator(const void *p, const void *q);
void print_struct_array(struct Area *array, size_t len, int in_region, int *f_index);

int main(void)
{
    struct Windmill windmill[2];

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

    struct Area area[4];

    strcpy(area[0].name, "Københavns Lufthavn");
    area[0].id = 0;
    area[0].wind_speed = 5.1;
    area[0].region = Hovedstaden;
    area[0].in_sea = 0;
    area[0].land_height = 2;
    area[0].roughness = 1.5;
    area[0].dist_to_house = 12;
    area[0].dist_to_powergrid = 5;
    area[0].expenses = calc_area_expenses(area[0]);

    strcpy(area[1].name, "Aarhus Lufthavn");
    area[1].id = 1;
    area[1].wind_speed = 3.6;
    area[1].region = Midtjylland;
    area[1].in_sea = 0;
    area[1].land_height = 30;
    area[1].roughness = 1;
    area[1].dist_to_house = 2;
    area[1].dist_to_powergrid = 5;
    area[1].expenses = calc_area_expenses(area[1]);

    //Dummy areas:
    strcpy(area[2].name, "Billund Lufthavn");
    area[2].id = 2;
    area[2].wind_speed = 4.6;
    area[2].region = Sydjylland;
    area[2].in_sea = 0;
    area[2].land_height = 50;
    area[2].roughness = 2;
    area[2].dist_to_house = 20;
    area[2].dist_to_powergrid = 10;
    area[2].expenses = calc_area_expenses(area[2]);

    strcpy(area[3].name, "Skagen Hovedgade");
    area[3].id = 3;
    area[3].wind_speed = 6.0;
    area[3].region = Nordjylland;
    area[3].in_sea = 0;
    area[3].land_height = 5;
    area[3].roughness = 0.5;
    area[3].dist_to_house = 30;
    area[3].dist_to_powergrid = 20;
    area[3].expenses = calc_area_expenses(area[3]);

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
    int region = -1; 
    int wind_turbine = -1; 
    int priority = -1;
    int budget = -1; 
    int input = 0;
    int calculate = -1;
    size_t arr_len;
    

    //Get the array length of our Area struct
    /* Find ud af hvad sizeof(struct Area) giver*/
    arr_len = sizeof(area) / sizeof(struct Area);

    while (!quit)
    {
        char string[200] = "1. Region\n2. Vindmølle\n3. Sortering\n4. Budget\n5. Kør program\n6. Luk programmet\n";
        input = get_input(string, 1, 6);

        switch (input)
        {
        case 1:
            region = get_region();
            break;
        case 2:
            wind_turbine = get_wind_turbine();
            break;
        case 3:
            priority = get_priority();
            break;
        case 4:
            budget = get_budget();
            break;
        case 5:
            calculate = 1;
            break;
        case 6:
            quit = 1;
        default:
            break;
        }

        if(calculate == 1 && wind_turbine != -1 && region != -1 && priority != -1 && quit != 1)
        {
            //Calculate kwh_output and total expenses for all the areas
            for (int i = 0; i < arr_len; i++)
            {
                area[i].kwh_output = calc_power_output(area[i], windmill[wind_turbine]);
                area[i].total_expenses = calc_total_expenses(area[i], windmill[wind_turbine]);
            }

            //Run the sorting of areas given the priority from user
            switch (priority)
            {
            case 0: // Sort the Areas by expenses low -> high
                qsort(area, arr_len, sizeof(struct Area), exp_comparator);
                break;
            case 1: //Sort the Areas by kWh output high -> low
                qsort(area, arr_len, sizeof(struct Area), kwh_comparator);
                break;
            default:
                exit(-1);
                break;
            }

            // Print the sorted list
            print_struct_array(area, arr_len, region, &f_index);

            //Print out all the area data of all the areas in given region
            print_area_data(area[f_index]);
        }
        else if(quit != 1)
        {
            printf("Utilstrækkeligt input\n");
        }
        
    }
    //---------------------------------------------------------------------
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

int get_priority()
{
    char string[100] = "Vælg prioritet:\n1. Prioritér laveste omkostninger\n2. Prioritér højeste energiproduktion\n";

    return (get_input(string, 1, 2));
}

int get_budget()
{
    char string[100] = "Indtast budget: \n";

    return (get_input(string, 1, 2147483647));
}

int get_region()
{ 
    char string[100] = "Vælg region:\n1. Hovedstaden\n2. Sydjylland\n3. Nordjylland\n4. Midtjylland\n5. Sjælland\n";
    
    return (get_input(string, 1, 5)); 
}

int get_wind_turbine()
{
    char string[100] = "Vælg vindmølle:\n1. Vestas\n2. Siemens\n";

    return (get_input(string, 1, 2));
}

int get_input(const char *string, int a, int b)
{
    char c;
    int input = 0;
    do
    {
        printf("%s", string);
    }

    while(((scanf("%d%c", &input, &c)!=2 || c!='\n') && clean_stdin()) || input < a || input > b);
    
    return(input);
}

//Print out all the data inside given area
void print_area_data(struct Area area)
{
    printf("------------------------------------------------------\n");
    printf("ID: \t\t\t %d\n", area.id);
    printf("Navn:\t\t\t %s\n", area.name);
    printf("Region:\t\t\t %s\n", get_region_name(area));
    printf("Vindhastighed:\t\t %.2f m/s\n", area.wind_speed);
    printf("På havet:\t\t\t %d\n", area.in_sea);
    printf("Landhøjde:\t\t %.2f m\n", area.land_height);
    printf("Ruhedsklasse:\t %.2f\n", area.roughness);
    printf("Afstand til nærmeste hus:\t %.2f km\n", area.dist_to_house);
    printf("Afstand til el-nettet:\t %.2f km\n", area.dist_to_powergrid);
    printf("Samlede kwh produktion:\t %.2f kW\n", area.kwh_output);
    printf("Terrænomkostninger:\t %.2f kr\n", area.expenses);
    printf("Samlede omkostninger:\t\t %.2f kr\n", area.total_expenses);
    printf("------------------------------------------------------\n");
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
//Approximation of cable excavation and construction fees
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
const char *get_region_name(struct Area area)
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
        return ("Ukendt region");
        break;
    }
}

//--------------------Sorting algorithm functions-------------------

//Comparator function for sorting areas expenses from high to low
int exp_comparator(const void *p, const void *q) 
{ 
    struct Area *p1 = (struct Area *)p;
    struct Area *p2 = (struct Area *)q;
    return (int)(100.f*p1->total_expenses - 100.f*p2->total_expenses);
} 

//Comparator function sorting areas kwh output from high to low
int kwh_comparator(const void *p, const void *q)
{
    struct Area *p1 = (struct Area *)p;
    struct Area *p2 = (struct Area *)q;
    return (int)(100.f*p2->kwh_output - 100.f*p1->kwh_output);
}

// Prints the sorted struct in the given region and returns the first index in that list  //
void print_struct_array(struct Area *array, size_t len, int in_region, int *f_index) 
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
            
            //Insures the first index is only set one time.
            if (count == 1)
            {
                *f_index = i;
            }
        }
    }
}

//---------------------------------------------------------------------

double calc_power_output(struct Area area, struct Windmill windmill)
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

    //Formula for finding exact windspeed at a given height
    wind_shear = area.wind_speed * (log(windmill.height/roughness_length)/(log(10/roughness_length))); 

    return (wind_shear);
}
