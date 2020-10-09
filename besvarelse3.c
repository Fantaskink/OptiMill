#include <stdio.h>
#define INTERRUPT_KEY -1

int main(void)
{
  
    int a, b, ans;

    while(1) 
    {

        printf("Skriv to positive heltal: ");

        //Stopper uventet input af bogstaver
        if (scanf("%d%d", &a, &b) != 2)
        {
            printf("Bad input !\n");
        }

        // (-1) input til at stoppe programmet
        else if (a == INTERRUPT_KEY || b == INTERRUPT_KEY)
        {
            printf("Interrupt tast trykket ...\n");
            break;
        }

        else if (a <= 0 || b <= 0)
        {
            printf("Ingen 0 eller negative inputs!\n");
        }

        //Naiv udregning af GCD
        else
        {
            for (int i = 1; i <= a ; i++)
            {
                if (a % i == 0 && b % i == 0)
                {
                    ans = i;
                }
            }
            
            printf("GCD af %d og %d er %d\n", a, b, ans);
        }

        //Fluser standard input stream.
        fflush(stdin);
    }
    
    return 0;
}