#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_LEN 128 //For image printing. Laurits ved det
#define AREA_SIZE 25 //Amount of areas
#define WINDMILL_MODELS 2 //Amount of implimented windmill models
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
	int id;					  //ID of windmill model
    char name[50];            //Name of the windmill
    int price;                //In Danish Crowns
    int height;               //In meters
    int wing_span;            //In meters
    int kW;                   //Actual power production
    int kW_max;               //Maximum power production
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
    double kW_output;        //kW output of the specific area with regards to chosen wind turbine
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
void print_windmill_model(Windmill windmill);
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
int kW_comparator(const void *p, const void *q);
int invest_comparator(const void *p, const void *q);
void print_struct_array(Area *array, size_t len, int in_region, int *f_index);
void print_area_summary(Area area, Windmill windmill);

int main(void)
{
    //Temporary variables for loading structs
    int ID, IN_SEA, REGION;
    int PRICE, HEIGHT, WING_SPAN, KWH;
    char NAME[50];                      
    double  WIND_SPEED, LAND_HEIGHT,
            ROUGHNESS, DIST_TO_HOUSE,
            DIST_TO_POWERGRID;



    //Create Struct Array
    Area area[AREA_SIZE];

    Windmill windmill[WINDMILL_MODELS];

    //Read from data file

    FILE* data = fopen("data.txt", "r");

    FILE* model = fopen("model.txt", "r");

    if (data == NULL)
    {
        printf("Kunne ikke indlæse datafilen!\n");
        exit(EXIT_FAILURE);
    }

    if (model == NULL)
    {
    	printf("Kunne ikke indlæse vindmøllemodel filen");
    	exit(EXIT_FAILURE);
    }

    //Transfer all the data from data file into every field of area struct array
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

    //Transfer all the information from model file into every field of the windmill struct array
    i = 0;
	while (fscanf(model, "%d %s %d %d %d %d",
				  &ID, NAME, &PRICE, &HEIGHT, &WING_SPAN, &KWH) > 0)
	{
		windmill[i].id = ID;
		strcpy(windmill[i].name, NAME);
		windmill[i].price = PRICE;
		windmill[i].height = HEIGHT;
		windmill[i].wing_span = WING_SPAN;
		windmill[i].kW_max = KWH;

		i++;
	}

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

        //Calculate kW_output and total expenses for all the areas
        for (int j = 0; j < arr_len; j++)
        {
            area[j].kW_output = calc_power_output(area[j], windmill[wind_turbine]);
            area[j].expenses = calc_area_expenses(area[j]);
            area[j].total_expenses = calc_total_expenses(area[j], windmill[wind_turbine]);
            area[j].inv_return = calc_windmill_income(area[j], windmill[wind_turbine]);
        }

        //Run the sorting of areas given the priority from user
        switch (priority)
        {
        case 1: // Sort the Areas by expenses low -> high
            qsort(area, arr_len, sizeof(Area), exp_comparator);
            break;

        case 2: //Sort the Areas by kW output high -> low
            qsort(area, arr_len, sizeof(Area), kW_comparator);
            break;

        case 3:
            qsort(area, arr_len, sizeof(Area), invest_comparator);
            break;

        default:
            exit(EXIT_FAILURE);
        }

        //Print the sorted list
        print_struct_array(area, arr_len, region, &f_index);

        print_area_summary(area[f_index], windmill[wind_turbine]);

        //Print out all the area data of all the areas in given region
        print_area_data(area[f_index]);

        //print_windmill_model(windmill[wind_turbine]);

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
    char *result;
    switch (a)
    {
    case 0:
        result = "Hovedstaden";
        break;

    case 1:
        result = "Syddanmark";
        break;
    case 2:
        result = "Nordjylland";
        break;
    case 3:
        result = "Midtjylland";
        break;
    case 4:
        result = "Sjælland";
        break;
    default:
        result = "Fejl";
        break;
    }

    return result;
}

const char *get_manufacturer(int a)
{
    char *result;
    switch (a)
    {
    case 0:
        result = "Vestas";
        break;
    case 1:
        result = "Siemens";
        break;
    default:
        result = "Fejl";
        break;
    }

    return result;
}

const char *get_input_priority(int a)
{
    char *result;

    switch (a)
    {
    case 1:
        result = "Omkostninger";
        break;
    
    case 2:
        result = "Energiproduktion";
        break;

    case 3: 
        result = "Årlig afkast";
        break;

    default:
        result = "Fejl";
        break;
    }

    return result;
}

int get_priority()
{
    char string[] = "Vælg prioritet:\n1. Prioritér laveste omkostninger\n2. Prioritér højeste energiproduktion\n3. Prioritér højeste afkast\n";

    return(get_input(string, 1, 3));
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
    printf("Samlede kW produktion:\t  %.2f kW\n", area.kW_output);
    printf("Terrænomkostninger:\t  %.2f kr\n", area.expenses);
    printf("Samlede omkostninger:\t  %.2f kr\n", area.total_expenses);
    printf("------------------------------------------------------\n");
}
//Prints all the information for the chosen windmill model
void print_windmill_model(Windmill windmill){
	printf("------------------------------------------------------\n");
	printf("ID: \t\t\t  %d\n", windmill.id);
	printf("Navn:\t\t\t  %s\n", windmill.name);
	printf("Pris:\t\t\t  %d kr.\n", windmill.price);
	printf("Højde:\t\t\t  %d m\n", windmill.height);
	printf("Vingefang:\t\t  %d m\n", windmill.wing_span);
	printf("El produktion:\t\t  %d kWh\n", windmill.kW);
	printf("------------------------------------------------------\n");
}

void print_area_summary(Area area, Windmill windmill)
{
    printf("Bedste valg:\n");
    printf("Navn: \t Omkostninger: \t Afkast: \t Energiproduktion:\n");
    printf("%s  %.2f kr \t %.2f kr\t %.2f kw\n", area.name, area.expenses, calc_windmill_income(area, windmill), area.kW_output);
}

//--------------------Expense calculation functions-------------------
double calc_total_expenses(Area area, Windmill windmill)
{
    double total_expense;

    total_expense = (area.expenses + windmill.price);

    return(total_expense);
}

double calc_area_expenses(Area area)
{
    double area_expense;

    area_expense =
        calc_digging_expenses(area) +
        calc_roughness_expenses(area) *
        sea_factor(area);

    return(area_expense);
}

//Approximation of wind turbine cost when constructed on the sea
double sea_factor(Area area)
{
    double factor;

    if(area.in_sea > 0)
    {
        factor = 2.6;
    }
    else
    {
        factor = 1;
    }

    return factor;
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
    char *result;

    switch (area.region)
    {
    case 0:
        result = "Hovedstaden";
        break;

    case 1:
        result = "Syddanmark";
        break;

    case 2:
        result = "Nordjylland";
        break;

    case 3:
        result = "Midtjylland";
        break;

    case 4:
        result = "Sjaelland";
        break;

    default:
        result = "Ukendt region";
        break;
    }

    return result;
}

//--------------------Sorting algorithm functions-------------------

//Comparator function for sorting areas expenses from high to low
int exp_comparator(const void *p, const void *q) 
{ 
    Area *area1 = (Area *)p;
    Area *area2 = (Area *)q;
    return(int)(area1->total_expenses - area2->total_expenses);
} 

//Comparator function sorting areas kW output from high to low
int kW_comparator(const void *p, const void *q)
{
    Area *area1 = (Area *)p;
    Area *area2 = (Area *)q;
    return(int)(area2->kW_output - area1->kW_output);
}

int invest_comparator(const void *p, const void *q)
{
    Area *area1 = (Area *)p;
    Area *area2 = (Area *)q;
    return(int)(area1->inv_return - area2->inv_return);
}

// Prints the sorted struct in the given region and returns the first index in that list  //
void print_struct_array(Area *array, size_t len, int in_region, int *f_index) 
{ 
    int count = 0;
    size_t i;
 
    printf("ID: \t Navn: \t\t Samlede omkostninger (kr): \t Energiproduktion (kW) \n");
    for(i=0; i<len; i++)
    {
        if ((int) array[i].region == in_region)
        {
            count += 1;
            printf("%d \t%s \t %.2f \t\t %.2f\n", array[i].id, array[i].name, array[i].total_expenses, array[i].kW_output);
            
            
            if (count == 1) //First index is only set one time.
            {
                *f_index = i;
            }
        }
    }
}

//---------------------------------------------------------------------

//Returns kW output from windmill calculated with given areas windspeed
double calc_power_output(Area area, Windmill windmill)
{
    double W;
    double kW;
    double wind_turbine_efficiency = 0.39;
    double air_dens = 1.225;
    double v = calc_wind_shear(area, windmill);
    double r = windmill.wing_span / 2;
    W = (M_PI/2) * pow(r,2) * pow(v,3) * air_dens * wind_turbine_efficiency;
    
    kW = W / 1000; //change from watt to kW

    if (kW > windmill.kW_max)
    {
        kW = windmill.kW_max;
    }
    
    return kW;
}

//Returns the wind speed in windmills height 
double calc_wind_shear(Area area, Windmill windmill)
{
    double wind_shear, roughness_length = 0;

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
    /* v = v_ref * ln(z/z_0) / ln(z_ref/z_0)   */
    wind_shear = area.wind_speed * (log(windmill.height/roughness_length)/(log(10/roughness_length))); 

    return(wind_shear);
}

//Return the hourly yield from windmill in DKK
double calc_windmill_income(Area area, Windmill windmill)
{
    double hourly_income = calc_power_output(area, windmill) * PRICE_PER_KW;
    return(hourly_income);
}

//Prints the yearly yield from windmill in DKK
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

     printf("Tid indtil vindmøllen har betalt for sig selv:\n");
     printf("Det tager ");

     if (years > 0)
     {
         printf("%lf år, ", years);
     }

     if (hours > 0)
     {
         printf("%lf måneder, ", months);
     }

     if (weeks > 0)
     {
         printf("%lf uger, ", weeks);
     }

     if (days > 0)
     {
         printf("%lf dage, ", days);
     } 
     
     if (hours > 0)
     {
         printf("%lf timer.", hours);
     }
    
     printf("Dette svare til at vindmøllen tjener: %lf Kr. i timen\n", income);
     printf("Investeringen et årligt afkast på: %.2lf %%\n", percent);
}