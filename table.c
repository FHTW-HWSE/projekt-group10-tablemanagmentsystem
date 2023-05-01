/*
 * theworld.c
 *
 *  Created on: Feb 27, 2023
 *      Author: osboxes
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	char name[50];
	char contact[50];
} Customer;

/**
 * represents information that is contained in one table
*/
typedef struct {

    int x;          ///< x coordinates of the table
    int y;          ///< y coordinates of the table
    bool reserved;  ///< reserverd or not
    Customer *customer;  ///<
} Table;


/**
 * represents information that is contained in the restaurant
*/

typedef struct {

    Table tables[100];
    int tablenumber;

} restaurant;

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

void reserveTable(restaurant *r, int tableID, const char *name, const char *contact)

{
    if (tableID >= r->tablenumber)
    {
        printf("Table ID %d does not exist.\n\n", tableID);
        return;
    }

    Table *table = &r->tables[tableID];

    if (table->reserved)
    {
        printf("Table ID %d is already reserved.\n\n", tableID);
        return;
    }

    //here we allocate space to a new customer
    Customer *new_customer = (Customer *)malloc(sizeof(Customer));

    //here we copy name input string to the namefield of the new_customer structure. with -1 we ensure
    //that the string does not go above the allocated memory

    strncpy(new_customer->name, name, sizeof(new_customer->name) - 1);

    //null terminator
    new_customer->name[sizeof(new_customer->name) - 1] = '\0';

    //here we copy contact input string to the contactfield of the new_customer structure.
    strncpy(new_customer->contact, contact, sizeof(new_customer->contact) - 1);
    new_customer->contact[sizeof(new_customer->contact) - 1] = '\0';

    //table is now reserved
    table->reserved = true;

    //associate the new_customer structure with the table structure (via pointer)

    table->customer = new_customer;
    printf("Table ID %d has been reserved for %s. Contact information: %s.\n\n", tableID, name, contact);
}

void free_table(restaurant* r, int tableID) {
	if (tableID < 0 || tableID >= r->tablenumber) {
	        printf("Invalid table ID.\n\n");
	        return;
	    }

	    Table* table = &r->tables[tableID];
	    if (!table->reserved)
	    {
	        printf("Table ID %d is not reserved.\n\n", tableID);
	        return;
	    }

	    printf("Table Nr. %d, reserved for %s (contact: %s) is now free.\n\n", tableID, table->customer->name, table->customer->contact);

	    free(table->customer);
	    table->customer = NULL;
	    table->reserved = false;
}

void showReservationInfo(restaurant* r)
{
    printf("\nTable reservation information:\n");

    for (int i = 0; i < r->tablenumber; i++)
    {
        Table *table = &r->tables[i];
        printf("Table ID: %d, Position: (%d, %d), Reserved: %s", i, table->x, table->y, table->reserved ? "Yes" : "No");

        if (table->reserved && table->customer != NULL)
        {
        	printf(",\nCustomer Name: %s, Contact: %s\n", table->customer->name, table->customer->contact);
        }

        printf("\n");
     }
}

int main()
{
	restaurant r = {0};
	char input;

	while (true)
	{
	    printf("Enter a function:\n(a = add Table, r = remove Table, b to book a table,"
	    		"f to cancel a reservation, \n s to show all reservations or q to exit):");
	    scanf(" %c", &input);

	    if (input == 'q'){
	    	printf("\nSee you soon!\n\n");
	    	break;

	    } else if (input == 'a'){
	    	int x, y;
	    	printf("\nEnter the position of the new table (x y): ");
	    	scanf("%d %d", &x, &y);
	    	tableAdd(&r, x, y);

	    } else if (input == 'r') {
	        int x, y;
	        printf("\nEnter position of table to remove (x y): ");
	        scanf("%d %d", &x, &y);
	        tableRemove(&r, x, y);

	    }  else if (input == 'r') {
            int x, y;
            printf("\nEnter position of table to remove (x y): ");
            scanf("%d %d", &x, &y);
            tableRemove(&r, x, y);

        } else if (input == 'b') {
        	 int id;
        	 char name[50];
        	 char contact[20];
        	 printf("\nEnter the table ID to book: ");
        	 scanf("%d", &id);
        	 printf("\nEnter the customer name: ");
        	 scanf(" %[^\n]%*c", name);
        	 printf("\nEnter the customer contact: ");
        	 scanf(" %[^\n]%*c", contact);
        	 reserveTable(&r, id, name, contact);

        } else if (input == 'f') {
        	int id;
        	printf("\nEnter the table ID to cancel the reservation: ");
        	scanf("%d", &id);
        	free_table(&r, id);

        } else if (input == 's') {
        	showReservationInfo(&r);

        }else {
        	printf("Invalid input.\n");
        }
    }

	return 0;
}

