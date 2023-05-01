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
#include <time.h>

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

void write_to_logfile(const char* message) {
	// Get the current timestamp

	time_t rawtime;
	struct tm* timeinfo;
	char timestamp[20];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

	// Open or Create the logfile in append mode
	FILE* logfile = fopen("restaurant_log.csv", "a");
	if (logfile == NULL) {
		printf("Error opening log file.\n");
		return;
	}

	//Write the log entry to the file

	fprintf(logfile, "%s: %s", timestamp, message);

	//Close the log File
	fclose(logfile);
}

void save_to_log(restaurant *r, const char *filename) {
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error opening log file.\n");
		return;
	}

	//Write the number of tables to the file
	fprintf(file, "%d\n", r -> tablenumber);

	for (int i = 0; i < r -> tablenumber; i++) {
		Table *table = &r -> tables[i];
		fprintf(file, "%d,%d,%d,%s,%s\n",
				table -> x, table -> y, table -> reserved,
				table -> reserved ? table -> customer -> name : "",
				table -> reserved ? table -> customer -> contact : "");
	}

	fclose(file);
}

bool load_from_log(restaurant *r, const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error opening log file.\n");
		return false;
	}

	fscanf(file, "%d", &r -> tablenumber);

	for (int i = 0; i < r -> tablenumber; i++) {
		Table *table = &r -> tables[i];
		int reserved;
		fscanf(file, "%d,%d,%d", &table -> x, &table -> y, &reserved);
		table -> reserved = (bool)reserved;

		if (table -> reserved) {
			table -> customer = (Customer *)malloc(sizeof(Customer));
			fscanf(file, ",%[^,],%s", table->customer->name, table->customer->contact);
		} else {
			table->customer = NULL;
			fscanf(file, ",,");
		}
	}

	fclose(file);
	return true;
}


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

    Table tablenew = {x, y, false, NULL};

    r->tables[r -> tablenumber++] = tablenew;

    printf("Table Nr. %d added at position x = %d, y = %d.\n\n", r->tablenumber - 1, x, y);
    char log_message[100];
    snprintf(log_message, 100, "Table Nr. %d added at position x = %d, y = %d.\n", r -> tablenumber - 1, x, y);
    write_to_logfile(log_message);

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
    char log_message[100];
    snprintf(log_message, 100,"Table at position x = %d, y = %d has been removed.\n", x, y);
    write_to_logfile(log_message);
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

void showReservationInfoFromFile(const char *filename) {
    restaurant r;

    if (!load_from_log(&r, filename)) {
        printf("Error loading reservations from the save file.\n");
        return;
    }

    printf("\nTable reservation information:\n");

    for (int i = 0; i < r.tablenumber; i++) {
        Table *table = &r.tables[i];

        printf("Table ID: %d, Position: (%d, %d), Reserved: %s", i, table->x, table->y, table->reserved ? "Yes" : "No");

        if (table->reserved) {
            printf(",\nCustomer Name: %s, Contact: %s\n", table->customer->name, table->customer->contact);
        } else {
            printf("\n");
        }
    }
}

int main()
{
	restaurant r = {0};
	char input;
	char log_file[] = "restaurant_log.csv";


	do {
	 printf("Do you want to load the last log file or create a new one? (l = load, n = new): ");
	 scanf(" %c", &input);

	 if (input == 'l') {
	         // Load the last log file
	         if (load_from_log(&r, "restaurant_log.csv")) {
	         	 break;
	         } else {
	        	printf("Creating a new log file as the previous one does not exist.\n");
	        	input = 'n';
	         }
	     } else if (input == 'n'){
	         // Create a new log file
	         save_to_log(&r, "restaurant_log.csv");
	         printf("Starting with a new log file.\n");
	     } else {
	    	 printf("Invalid input. Please try again.\n");
	     }
	} while (input != 'l' && input != 'n');

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
        	showReservationInfoFromFile(log_file);

        }else {
        	printf("Invalid input.\n");
        }
    }

	return 0;
}

