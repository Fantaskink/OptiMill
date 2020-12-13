#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_LEN 128             //Used when printing the optimill logo
#define AREA_COUNT 25            //Amount of implemented areas
#define WINDMILL_MODELS 2       //Amount of implemented windmill models
#define PRICE_PER_KWH 0.2       //The return price for selling 1 KWh of electricity
#define PRICE_PER_KM 200000     //Price for cable digging being 200 kr pr. meter
#define TURBINE_PRICE_PER_KW 5000 
#define HOURS_IN_DAY 24
//#define HOURS_IN_WEEK 168
#define HOURS_IN_MONTH 730      //Average of 8766/12 = 730.5 hours in a month
#define HOURS_IN_YEAR 8766      //Average of 365.25 days in a year


typedef enum region
{
    Hovedstaden,
    Syddanmark,
    Nordjylland,
    Midtjylland,
    Sjaelland
} region;

typedef struct Windmill
{
	int id;					  //ID of windmill model
    char name[50];            //Name of the windmill
    int price;                //Price of the rotorblades and tower, in Danish Crowns
    int height;               //In meters
    int wing_span;            //In meters
    int kW;                   //Calculated power production
    int kW_max;               //Maximum power production also denoted as the installed power

} Windmill;

typedef struct Area
{
    int id;                   //ID of the area
    char name[50];            //Name of the area
    region region;            //Region in which the area is located
    double wind_speed;        //Measuered wind speed of area in meters per second in 10 meters height
    double land_height;       //Landheight from sea level, in meters
    double roughness;         //Roughness class: 0..4
    double dist_to_house;     //The distance to the nearest house in km
    double dist_to_powergrid; //The distance to the nearest tranformerstation capable of handling the windmills capacity, in km
    double expenses;          //Installation expenses also denoted terrain expenses, in Danish Crowns
    double total_expenses;    //Expenses including wind turbine expenses
    double kW_output;         //kW output of the specific area with regards to chosen wind turbine
    double inv_return;        //Monetary return of specific windmill when placed in specific area
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
int find_best_area_index(Area area[], int in_region, int in_budget);
void print_windmill_investment_return(Area area, Windmill windmill);
void print_area_summary(Area area, Windmill windmill);
void print_image(char *filename);

int main(void)
{
    //Create Struct Array
    Area area[AREA_COUNT];
    Windmill windmill[WINDMILL_MODELS];

    //Read from data file
    FILE* data = fopen("data.txt", "r");
    FILE* model = fopen("model.txt", "r");

    //Check if the pointers found the files
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

    //Transfer all the data from area data file into every field of area struct array
    int i = 0;
    while (!feof(data)) 
    {
        fscanf(data, " %d %s %d %lf %lf %lf %lf %lf",
                        &area[i].id, area[i].name, &area[i].region,
                        &area[i].wind_speed, &area[i].land_height, &area[i].roughness,
                        &area[i].dist_to_house, &area[i].dist_to_powergrid);
        i++;
    }
    fclose(data);

    //Transfer all the information from model file into every field of the windmill struct array
    i = 0;
	while (!feof(model))
	{
        fscanf(model, " %d %s %d %d %d",
	        &windmill[i].id, windmill[i].name, 
            &windmill[i].height, &windmill[i].wing_span, 
            &windmill[i].kW_max);

        windmill[i].price = windmill[i].kW_max * TURBINE_PRICE_PER_KW;

		i++;
	}
    fclose(model);

    //Print the optimill logo
    char *filename = "image.txt";
    print_image(filename);
    
    //Variables to keep track of user choice
    int quit = 1;
    int region; 
    int wind_turbine; 
    int priority;
    int budget;
    int new_choice;
    
    //Get the array length of our Area struct
    int arr_len = sizeof(area) / sizeof(Area);

    //Get the user inputs atleast once before looping
    region = get_region() - 1;
    budget = get_budget();
    wind_turbine = get_wind_turbine() - 1;
    priority = get_priority();

    //Run the user interractions
    while (quit)
    {
        printf("Dine valg:\n");
        printf("------------------------------------------------------\n");
        printf("Region:\t\t\t %s\n", get_input_region_name(region));
        printf("Budget:\t\t\t %d kr\n", budget);
        printf("Producent:\t\t %s\n", get_manufacturer(wind_turbine));
        printf("Prioritét\t\t %s\n", get_input_priority(priority));
        printf("------------------------------------------------------\n");

        //Calculate for all the areas their energy production, installation expenses, total expenses and the investment return
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
        case 1:
            qsort(area, arr_len, sizeof(Area), exp_comparator);
            break;

        case 2:
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
        
        //Prompt user to change their input to make new calculations or terminate the program
        do
        {
            new_choice = get_user_continue();
        
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
                print_windmill_model(windmill[wind_turbine]);
                break;
            case 7:
                print_windmill_investment_return(area[best_index], windmill[wind_turbine]);
                break;
            case 8:
                break;
            case 0:
                quit = 0;
                break;
            default:
                exit(EXIT_FAILURE);    
            }
        } while (new_choice < 8 && quit != 0);
    }
    
    return 0;
}

//Takes our ASCII image file and prints to terminal
void print_image(char *filename)
{
    FILE *fptr = fopen(filename, "r");

    /*Check if we got the pointer to the file */
    if (fptr == NULL)
    {
        printf("Fejl ved indlæsning af logo\n");
        exit(EXIT_FAILURE);
    }

    char read_string[MAX_LEN];

    /*Read line by line until end of file*/
    while ( !feof(fptr)) {

        fgets(read_string, MAX_LEN, fptr);

        printf("%s", read_string);
    }

    /*Close the file */
    fclose(fptr);
    printf("\n");
}

/* These functions all return integer values corresponding to the choices made by the user */
int get_priority()
{
    char string[] = "Vælg prioritet:\n1. Prioritér laveste omkostninger\n2. Prioritér højeste energiproduktion\n3. Prioritér højeste afkast\n";

    return(get_input(string, 1, 3));
}

int get_budget()
{
	char string[] = "Vælg budgetklasse:\n1. 20.000.000 kr.\n2. 40.000.000 kr.\n3. 60.000.000 kr.\n";

	return (20000000 * get_input(string, 1, 3));
}

int get_region()
{ 
    char string[] = "Vælg region:\n1. Hovedstaden\n2. Syddanmark\n3. Nordjylland\n4. Midtjylland\n5. Sjælland\n";
    
    return(get_input(string, 1, 5)); 
}

int get_wind_turbine()
{
    char string[] = "Vælg vindmølle:\n1. Vestas V90 - 2MW\n2. Siemens SG 2.6MW - 114\n";

    return(get_input(string, 1, 2));
}

int get_user_continue()
{
    char string[] = "Valg muligheder:\n1. Vælg ny region\n2. Indtast nyt budget\n3. Vælg anden vindmøllemodel\n4. Vælg sorteringsmulighed\n5. Se detaljer på bedst valgte område\n6. Se detaljer på vindmøllemodel\n7. Se investeringsdetaljer\n8. Skab ny oversigt\n0. Afslut program\n";

    return(get_input(string, 0, 8));
}

//Handles all user input and ensures the input is in the given range. It returns the user's input as an int.
int get_input(const char *string, int a, int b)
{
    char c;
    int input;
    do
    {
        printf("%s", string);
    }

    /*If some of these criteria get back as true then it means the user inputted something out of range
     and the user gets prompted with the incomming string once again*/
    while(((scanf("%d%c", &input, &c)!=2 || c!='\n') && clean_stdin()) || input < a || input > b);
    
    return(input);
}
//Cleans the stdin to resolve errors occurring with undefined inputs from user
int clean_stdin()
{
    while (getchar()!='\n');
    return 1;
}

//---------------------------------------------------------------------
//These next three functions returns a string representation of the choice to be displayed to the user
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
        result = "Vestas V90 - 2MW";
        break;

    case 1:
        result = "Siemens SG 2.6MW - 114";
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

//Prints all the details inside given area
void print_area_data(Area area)
{
    printf("------------------------------------------------------\n");
    printf("ID: \t\t\t  %d\n", area.id);
    printf("Navn:\t\t\t  %s\n", area.name);
    printf("Region:\t\t\t  %s\n", get_input_region_name(area.region));
    printf("Vindhastighed:\t\t  %.2f m/s\n", area.wind_speed);
    printf("Landhoejde:\t\t  %.2f m\n", area.land_height);
    printf("Ruhedsklasse:\t\t  %.2f\n", area.roughness);
    printf("Afstand til naermeste hus:%.2f km\n", area.dist_to_house);
    printf("Afstand til elnettet:\t  %.2f km\n", area.dist_to_powergrid);
    printf("Energi produktion:\t  %.2f kW\n", area.kW_output);
    printf("Terraenomkostninger:\t  %.2f kr\n", area.expenses);
    printf("Samlede omkostninger:\t  %.2f kr\n", area.total_expenses);
    printf("------------------------------------------------------\n");
}
//Prints all the information for the chosen windmill model
void print_windmill_model(Windmill windmill){
	printf("------------------------------------------------------\n");
	printf("ID: \t\t\t  %d\n", windmill.id);
	printf("Producent:\t\t\t  %s\n", windmill.name);
	printf("Vejl. Pris:\t\t  %d kr.\n", windmill.price);
	printf("Højde:\t\t\t  %d m\n", windmill.height);
	printf("Vingefang:\t\t  %d m\n", windmill.wing_span);
	printf("Installeret effekt:\t\t  %d kW\n", windmill.kW_max);
	printf("------------------------------------------------------\n");
}

//Prints the optimal area in a formatted way.
void print_area_summary(Area area, Windmill windmill)
{
    double yearly_income = calc_windmill_income(area, windmill) * HOURS_IN_YEAR / pow(10, 6);
    double yearly_production = area.kW_output * HOURS_IN_YEAR / pow(10, 6);
    double total_expenses_mio = area.total_expenses / pow(10, 6);
    printf("Bedste valg:\n");
    printf("Navn: \t\t Samlede omkostninger: Årlige afkast: Årlige energiproduktion:\n");
    printf("%s \t %.2f mio. kr\t\t%.2f mio/år\t%.2f GWh\n", area.name, total_expenses_mio, yearly_income, yearly_production);
}

//--------------------Expense calculation functions-------------------
//Total expenses for a windmill project.
double calc_total_expenses(Area area, Windmill windmill)
{
    double total_expense;

    total_expense = (area.expenses) + windmill.price;

    return(total_expense);
}

//Calculates the installation cost for placing a windmill in given area.
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

//--------------------Sorting algorithm functions-------------------
//Comparator function for sorting areas expenses from low to high
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

//Compares two areas investment return and sorts high to low
int invest_comparator(const void *p, const void *q)
{
    Area *area1 = (Area *)p;
    Area *area2 = (Area *)q;

    return(int)(area2->inv_return - area1->inv_return);
}


//---------------------------------------------------------------------
//Returns back the index in the area struct array where the region matches and total expenses is in the budget range
int find_best_area_index(Area area[], int in_region, int in_budget){

    int index = 0;

    while(index < AREA_COUNT){

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
    double wind_turbine_efficiency = 0.40;
    double air_dens = 1.225;
    double v = calc_wind_shear(area, windmill);
    double r = windmill.wing_span / 2;

    W = (M_PI/2) * pow(r,2) * pow(v,3) * air_dens * wind_turbine_efficiency;
    
    //Change from watt to kW
    kW = W / 1000; 
    
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

//Prints investment return details
void print_windmill_investment_return(Area area, Windmill windmill)
{
    int hours, days, months, years;
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

    printf("Investeringsdetaljer\n");
    printf("------------------------------------------------------\n");
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
        printf("%d timer.\n", hours);
    
    
     printf("Dette svarer til at vindmøllen tjener: %.2lf Kr. om året\n", yearly_income);
     printf("Investeringen har et årligt afkast på: %.2lf %%\n", percent);
     printf("------------------------------------------------------\n");
}