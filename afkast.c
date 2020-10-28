#include <stdio.h>
#include <math.h>

#define Price_Per_Kilowatt 0.2 
#define Hours_in_day 24
#define hours_in_week 168
#define hours_in_month 732
#define hours_in_year 8784

void kilowatt_to_dkk_calc(double input_kilowatt, double *dkk);
void udregn_investeringsafkast(double input_vindmølle_pris, double *ddk, double *timer, double *dage, double *uger, double *maaneder, double *aar);

int main(void) {
    double input_kilowatt, input_vindmølle_pris, dkk, timer, dage, uger, maaneder, aar;

     printf("Indtast antal kWh: ");
     scanf("%lf", &input_kilowatt);
     printf("\nHvor meget kostede vindmøllen: ");
     scanf("%lf", &input_vindmølle_pris);
     kilowatt_to_dkk_calc(input_kilowatt, &dkk);
     udregn_investeringsafkast(input_vindmølle_pris, &dkk, &timer, &dage, &uger, &maaneder, &aar);
     printf("Vindmøllen tjener: %lf Kr. i timen\n\n", dkk);
     printf("Vindmøllen tjner sig selv hjem på:\n");
     printf("%lf timer for at vindmøllen har tjent sig selv hjem.\n", timer);
     printf("%lf dage for at vindmøllen har tjent sig selv hjem.\n", dage);
     printf("%lf uger for at vindmøllen har tjent sig selv hjem.\n", uger);
     printf("%lf måneder for at vindmøllen har tjent sig selv hjem.\n", maaneder);
     printf("%lf år for at vindmøllen har tjent sig selv hjem.\n", aar);

     return 0;
}

void kilowatt_to_dkk_calc(double input_kilowatt, double *dkk) {
     *dkk = input_kilowatt * Price_Per_Kilowatt;
}

void udregn_investeringsafkast(double input_vindmølle_pris, double *dkk, double *timer, double *dage, double *uger, double *maaneder, double *aar) {
     *timer = input_vindmølle_pris / *dkk;
     *dage = input_vindmølle_pris / *dkk / Hours_in_day;
     *uger = input_vindmølle_pris / *dkk / hours_in_week;
     *maaneder = input_vindmølle_pris / *dkk / hours_in_month;
     *aar = input_vindmølle_pris / *dkk / hours_in_year;
}