#include <stdio.h>


int main(void)
{
    int laengde;
    printf("Indtast laengde: ");
    scanf("%d", &laengde);

    printf("8");
    for(int i = 1; i <= laengde; i++)
    {
        printf("=");
    }
    printf("D\n");
}
