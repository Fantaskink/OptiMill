#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_LEN 128 //For image printing. Laurits ved det
#define AREA_SIZE 25 //Amount of areas
#define WINDMILL_MODELS 2 //Amount of implimented windmill models
#define PRICE_PER_KWH 0.2
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
    //int lifespan;             //Lifespan of windmill
} Windmill;

typedef struct Area
{
    int id;                   //ID of the area
    char name[50];            //Name of the area
    region region;            //Region in which the area is located
    double wind_speed;        //Raw wind speed of area in meters per second
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
int clean_stdin();
const char *get_input_region_name(int a);
const char *get_manufacturer(int a);
const char *get_input_priority(int a);
int get_region();
int get_wind_turbine();
int get_priority();
int get_budget();
int get_user_continue();
int get_input(const char *string, int a, int b);
const char *get_region_name(Area area);

double calc_total_expenses(Area area, Windmill windmill);
double calc_area_expenses(Area area, Windmill windmill);
double calc_digging_expenses(Area area);
double calc_foundation_expenses(Area area, Windmill windmill);

double calc_power_output(Area area, Windmill windmill);
double calc_wind_shear(Area area, Windmill windmill);
double calc_windmill_income(Area area, Windmill windmill);

int exp_comparator(const void *p, const void *q);
int kW_comparator(const void *p, const void *q);
int invest_comparator(const void *p, const void *q);

void print_area_data(Area area);
void print_windmill_model(Windmill windmill);
void print_area_array(Area area[]);
int find_best_area_index(Area area[], int in_region, int in_budget);
void print_windmill_investment_return(Area area, Windmill windmill);
void print_area_summary(Area area, Windmill windmill);
void print_image(FILE *fptr);

int main(void)
{
    //Temporary variables for loading structs
    int ID, REGION;
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
    ////ID, NAVN, REGION, VINDHASTIGHED, LAND_HØJDE, RUHEDSKLASSE, TIL_HUS, TIL_ELNET
    while (fscanf(data, "%d %s %d %lf %lf %lf %lf %lf",
                        &ID, NAME, &REGION, &WIND_SPEED, &LAND_HEIGHT, &ROUGHNESS,
                        &DIST_TO_HOUSE, &DIST_TO_POWERGRID) > 0)
    {
        area[i].id = ID;
        strcpy(area[i].name, NAME);
        area[i].region = REGION;
        area[i].wind_speed = WIND_SPEED;
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

    int quit = 1;
    int region; 
    int wind_turbine; 
    int priority;
    int budget;
    int new_choice;
    size_t arr_len;
    
    //Get the array length of our Area struct
    arr_len = sizeof(area) / sizeof(Area);

    region = get_region() - 1;
    budget = get_budget();
    wind_turbine = get_wind_turbine() - 1;
    priority = get_priority();

    while (quit)
    {
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
            area[j].expenses = calc_area_expenses(area[j], windmill[wind_turbine]);
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
        //Find the best result for given sorting
        int best_index = find_best_area_index(area, region, budget);

        //Print the best result
        print_area_summary(area[best_index], windmill[wind_turbine]);
        
        printf("------------------------------------------------------\n");
        
        do
        {
            new_choice = get_user_continue(); //returns 0 to 6
        
            switch(new_choice){
            case 1:     
                region = get_region() - 1;
                break;
            case 2:
                budget = get_budget();
                break;
            case 3:
                wind_turbine = get_wind_turbine() - 1;
                break;
            case 4:
                priority = get_priority();
                break;
            case 5:
                print_area_data(area[best_index]);
                break;
            case 6:
                print_windmill_investment_return(area[best_index], windmill[wind_turbine]);
                break;
            case 7:
                break;
            case 0:
                quit = 0;
            default:
                exit(EXIT_FAILURE);    
            }
        } while (new_choice < 6 && quit != 0);
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
	char string[] = "Vælg budget:\n1. 18,000,000-24,000,000 kr.\n2. 24,000,000-30,000,000 kr.\n3. 30,000,000-50,000,000 kr.\n";

	return (20000000 * get_input(string, 1, 3));
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

int get_user_continue()
{
    char string[] = "Vil du starte en ny beregning?:\n1. Vælg ny region\n2. Indtast nyt budget\n3. Vælg anden vindmøllemodel\n4. Vælg sorteringsmulighed\n5. Se detaljer på bedst valgte område\n6. Se investeringsdetaljer\n7. Skab ny oversigt\n0. Afslut program\n";

    return(get_input(string, 0, 7));
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

//Prints all the details inside given area
void print_area_data(Area area)
{
    printf("------------------------------------------------------\n");
    printf("ID: \t\t\t  %d\n", area.id);
    printf("Navn:\t\t\t  %s\n", area.name);
    printf("Region:\t\t\t  %s\n", get_region_name(area));
    printf("Vindhastighed:\t\t  %.2f m/s\n", area.wind_speed);
    printf("Landhoejde:\t\t  %.2f m\n", area.land_height);
    printf("Ruhedsklasse:\t\t  %.2f\n", area.roughness);
    printf("Afstand til naermeste hus: %.2f km\n", area.dist_to_house);
    printf("Afstand til elnettet:\t  %.2f km\n", area.dist_to_powergrid);
    printf("Samlede kW produktion:\t  %.2f kW\n", area.kW_output);
    printf("Terraenomkostninger:\t  %.2f kr\n", area.expenses);
    printf("Samlede omkostninger:\t  %.2f kr\n", area.total_expenses);
    printf("------------------------------------------------------\n");
}
//Prints all the information for the chosen windmill model
void print_windmill_model(Windmill windmill){
	printf("------------------------------------------------------\n");
	printf("ID: \t\t\t  %d\n", windmill.id);
	printf("Navn:\t\t\t  %s\n", windmill.name);
	printf("Vejl. Pris:\t\t\t  %d kr.\n", windmill.price);
	printf("Hoejde:\t\t\t  %d m\n", windmill.height);
	printf("Vingefang:\t\t  %d m\n", windmill.wing_span);
	printf("El produktion:\t\t  %d kWh\n", windmill.kW);
	printf("------------------------------------------------------\n");
}

//Prints the optimal area
void print_area_summary(Area area, Windmill windmill)
{
    double yearly_income = calc_windmill_income(area, windmill);

    printf("Bedste valg:\n");
    printf("Navn: \t Samlede omkostninger: \t Aarligt afkast: \t Energiproduktion:\n");
    printf("%s  %.2f kr \t %.2f kr/aar\t %.2f kw\n", area.name, area.total_expenses, yearly_income, area.kW_output);
}

// Prints the entire area array - FOR DEBUGGING  //
void print_area_array(Area area[]) 
{ 
    printf("ID: \t Navn: \t\t Samlede omkostninger (kr): \t Energiproduktion (kW) \n");
    for(int i=0; i<AREA_SIZE; i++)
    {
        printf("%d \t%s \t %.2f \t\t %.2f\n", area[i].id, area[i].name, area[i].total_expenses, area[i].kW_output);
    }
}

//--------------------Expense calculation functions-------------------
//Total expenses for a whole windmill project.
double calc_total_expenses(Area area, Windmill windmill)
{
    double total_expense;

    total_expense = (area.expenses + windmill.price);

    return(total_expense);
}

//Expense for placing a windmill in given area
double calc_area_expenses(Area area, Windmill windmill)
{
    int transport_expense = 100000;

    double area_expense;

    area_expense =
        (calc_digging_expenses(area) +
        calc_foundation_expenses(area, windmill) +
        transport_expense);

    return(area_expense);
}


//Approximation of cable excavation and construction fees
double calc_digging_expenses(Area area)
{
    return(area.dist_to_powergrid * PRICE_PER_KM);
}

//Approximation of the expense used to cast concrete
double calc_foundation_expenses(Area area, Windmill windmill)
{
    int foundation_price_pr_kw = 290;

    return(windmill.kW_max * foundation_price_pr_kw);

}

//Approximation of the windturbine head and stand expense
double calc_windturbine_expense(Windmill windmill){

    int turbine_price_pr_kw = 5000;

    return(windmill.kW_max * turbine_price_pr_kw);
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
    return(int)(area2->inv_return - area1->inv_return);
}


//---------------------------------------------------------------------

int find_best_area_index(Area area[], int in_region, int in_budget){

    int index = 0;

    while(index < AREA_SIZE){

        if ((int) area[index].region == in_region && in_budget > area[index].total_expenses)
        {
            break;
        }
        
        index += 1;
    }
    return index;
}

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
    double hourly_income = calc_power_output(area, windmill) * PRICE_PER_KWH;
    return(hourly_income);
}

//Prints the yearly yield from windmill in DKK
void print_windmill_investment_return(Area area, Windmill windmill)
{
    int hours, days, weeks, months, years;
    double percent;
    double income = calc_windmill_income(area, windmill);
    double yearly_income = income * HOURS_IN_YEAR;
    
    hours = windmill.price / income;
    days = windmill.price / income / HOURS_IN_DAY;
    months = windmill.price / income / HOURS_IN_MONTH;
    years = windmill.price / income / HOURS_IN_YEAR;
    percent = ((income * HOURS_IN_YEAR) / windmill.price) * 100;

    days = hours / 24;
    hours = hours % 24;
     
    months = days / 31;
    days = days % 31;
    
    years = months / 12;
    months = months % 12;

    years = years % 12;

    printf("Tid indtil vindmøllen har betalt for sig selv:\n");

    if (years > 0) 
        printf("%d år, ", years);
    

     if (months > 0) 
     {
        if (months > 1)
            printf("%d måneder, ", months);
        else
            printf("%d måned", months);
    }

    if (days > 0)
    {
        if (days > 1) 
            printf("%d dage, ", days);
        else
            printf("%d dag", days);
    }
     
     if (hours > 0)
     {
         printf("%lf timer.", hours);
     }
    
     printf("Dette svare til at vindmøllen tjener: %lf Kr. om året\n", yearly_income);
     printf("Investeringen et årligt afkast på: %.2lf %%\n", percent);
}