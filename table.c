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
#include <math.h>

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

void save_to_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening save file.\n");
        return;
    }

    // Write the number of tables to the file
    fprintf(file, "%d\n", r->tablenumber);

    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];
        fprintf(file, "%d,%d,%d,%s,%s\n",
                table->x, table->y, table->reserved,
                table->reserved ? table->customer->name : "",
                table->reserved ? table->customer->contact : "");
    }

    fclose(file);
}

bool load_from_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening save file.\n");
        return false;
    }

    fscanf(file, "%d", &r->tablenumber);

    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];
        int reserved;
        fscanf(file, "%d,%d,%d", &table->x, &table->y, &reserved);
        table->reserved = (bool)reserved;

        if (table->reserved) {
            table->customer = (Customer *)malloc(sizeof(Customer));
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
         if (r->tables[i].x == x && r->tables[i].y == y)
         {
             printf("\nNo free space at x = %d, y = %d.\n", x, y);
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
        printf("Table at position (%d, %d) does not exist.\n", x, y);
        return;
    }

    for (int i = index; i < r -> tablenumber - 1; i++)
    {
        r -> tables[i] = r -> tables[i+1];
    }
    r -> tablenumber--;

    printf("Table at position x = %d, y = %d has been removed.\n", x, y);
    char log_message[100];
    snprintf(log_message, 100,"Table at position x = %d, y = %d has been removed.\n", x, y);
    write_to_logfile(log_message);
    return;
}

void reserveTable(restaurant *r, int tableID, const char *name, const char *contact)

{
    if (tableID >= r->tablenumber)
    {
        printf("Table ID %d does not exist.\n", tableID);
        return;
    }

    Table *table = &r->tables[tableID];

    if (table->reserved)
    {
        printf("Table ID %d is already reserved.\n", tableID);
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
    printf("Table ID %d has been reserved for %s. Contact information: %s.\n", tableID, name, contact);
    char log_message[100];
    snprintf(log_message, 100,"Table ID %d has been reserved for %s. Contact information: %s.\n", tableID, name, contact);
    write_to_logfile(log_message);
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
	    char log_message[200];
	    snprintf(log_message, 200,"Table Nr. %d, reserved for %s (contact: %s) is now free.\n", tableID, table->customer->name, table->customer->contact);
	    write_to_logfile(log_message);

	    free(table->customer);
	    table->customer = NULL;
	    table->reserved = false;
}

void showReservationInfoFromFile(const char *filename, int filter) {
    restaurant r;

    if (!load_from_file(&r, filename)) {
        printf("Error loading reservations from the save file.\n");
        return;
    }

    printf("\nTable reservation information:\n");

    int table_count = 0;
    for (int i = 0; i < r.tablenumber; i++) {
           Table *table = &r.tables[i];

           if (filter == 1 && !table->reserved) {
               continue;
           } else if (filter == 2 && table->reserved) {
               continue;
           }

        printf("Table ID: %d, Position: (%d, %d), Reserved: %s\n", i, table->x, table->y, table->reserved ? "Yes" : "No");
        table_count++;
        if (table->reserved) {
            printf("Customer Name: %s, Contact: %s\n\n", table->customer->name, table->customer->contact);

        } else {
            printf("\n");
        }
    }
    if (table_count == 0) {
    	printf("There are no tables.\n\n");
    }
}

float calculate_distance(Table *t1, Table *t2) {
    int x_diff = t1->x - t2->x;
    int y_diff = t1->y - t2->y;
    return sqrt(x_diff * x_diff + y_diff * y_diff);
}

void query_occupied_tables_within_distance(restaurant *r, int table_id, float distance) {
    Table *target_table = &r->tables[table_id];

    printf("Occupied tables within %.2f units from Table ID %d:\n\n", distance, table_id);

    int occupied_count = 0;
    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];
        if (table->reserved && calculate_distance(target_table, table) <= distance) {
            printf("Table ID: %d, Position: (%d, %d), Customer Name: %s, Contact: %s\n", i, table->x, table->y, table->customer->name, table->customer->contact);
            occupied_count++;
          }
    }

    if (occupied_count == 0) {
        printf("No occupied tables within the specified distance.\n");
    }
}

int main()
{
	restaurant r = {0};
	char input;
    char save_file[] = "restaurant_save.csv";
    char log_file[] = "restaurant_log.csv";
	do {
	 printf("Do you want to load the last save file or create a new one? (l = load, n = new): ");
	 scanf(" %c", &input);

	 if (input == 'l') {
	         // Load the last log file
	         if (load_from_file(&r, save_file)) {
	         	 break;
	         } else {
	        	printf("Creating a new log file as the previous one does not exist.\n\n");
	        	input = 'n';
	         }
	 	 } else if (input == 'n') {
	     // Reset the save file
	     FILE *save_file_handle = fopen(save_file, "w");
	     if (save_file_handle != NULL) {
	         fclose(save_file_handle);
	     }

	     // Reset the log file
	     FILE *log_file_handle = fopen(log_file, "w");
	     if (log_file_handle != NULL) {
	         fclose(log_file_handle);
	     }
	         printf("Starting with a new log file.\n\n");
	     } else {
	    	 printf("Invalid input. Please try again.\n\n");
	     }
	} while (input != 'l' && input != 'n');

	while (true)
	{
	    printf("\nEnter a function:\n(a = add Table, r = remove Table, b to book a table,"
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
	    	save_to_file(&r, save_file);


	    } else if (input == 'r') {
	        int x, y;
	        printf("\nEnter position of table to remove (x y): ");
	        scanf("%d %d", &x, &y);
	        tableRemove(&r, x, y);
	        save_to_file(&r, save_file);

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
        	 save_to_file(&r, save_file);

        } else if (input == 'f') {
        	int id;
        	printf("\nEnter the table ID to cancel the reservation: ");
        	scanf("%d", &id);
        	free_table(&r, id);
        	save_to_file(&r, save_file);

        } else if (input == 's') {
        	 printf("\nDo you want to see all tables, only reserved tables, \nor only free tables? (a = all, r = reserved, f = free): ");
        	 char filter_choice;
        	 int filter = 0;
        	 scanf(" %c", &filter_choice);

        	 if (filter_choice == 'r') {
        	     filter = 1;
        	 } else if (filter_choice == 'f') {
        	     filter = 2;
        	 } else if (filter_choice != 'a') {
        	     printf("Invalid input. Please try again.\n");
        	     break;
        	 }
        	 showReservationInfoFromFile(save_file, filter);

        }else if (input == 'd') {
        	printf("Enter the table ID and distance: ");
        	int table_id, distance;
        	scanf("%d %d", &table_id, &distance);
        	query_occupied_tables_within_distance(&r, table_id, distance);

        }else {
        	printf("Invalid input.\n");
        }
    }

	save_to_file(&r, save_file);
	return 0;
}

