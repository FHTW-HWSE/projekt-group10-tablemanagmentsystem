#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	char name[50];
	char contact[50];
} Customer;





//davor kannst schreiben ASAD
//MATEUSZ:
void tableAdd(restaurant* r, int x, int y)
{

    if (r->tablenumber == 100)
    {
        printf("We have no room for extra tables!!\n");
        return;
    }

    // check if there is already a table at that position
    for (int i = 0; i < r->tablenumber; i++)
    {
         if (r->tables[i].x == x || r->tables[i].y == y)
         {
             printf("\nNo free space at x = %d, y = %d.\n\n", x, y);
             return;
         }
     }

    Table tablenew = {x, y, false, -1};

    r->tables[r -> tablenumber++] = tablenew;

    printf("Table Nr. %d added at position x = %d, y = %d.\n\n", r->tablenumber - 1, x, y);
}

void tableRemove(restaurant* r, int x, int y)
{

    int index = -1;
    for (int i = 0; i < r -> tablenumber; i++)
    {
        if (r -> tables[i].x == x && r -> tables[i].y == y)
        {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Table at position (%d, %d) does not exist.\n\n", x, y);
        return;
    }

    for (int i = index; i < r -> tablenumber - 1; i++)
    {
        r -> tables[i] = r -> tables[i+1];
    }
    r -> tablenumber--;

    printf("Table at position x = %d, y = %d has been removed.\n\n", x, y);
    return;
}
