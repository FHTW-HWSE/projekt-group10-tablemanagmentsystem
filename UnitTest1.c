#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>

// Wir haben die UnitTests in 2 geteilt, da wir zu einigen Problemen gekommen sind die sich durch das in zwei Teilen gelöst haben

// Structure to represent a customer
typedef struct {
    char name[50];
    char contact[20];
    time_t time_of_reservation;
} Customer;

// Structure to represent a table
typedef struct {
    int x;
    int y;
    bool reserved;
    Customer* customer;
} Table;

// Structure to represent the restaurant
typedef struct {
    int tablenumber;
    Table tables[100];
} restaurant;

// Function to write a log message to a log file
bool write_to_logfile(const char* message) {
    FILE* file = fopen("restaurant_log.csv", "a");
    if (file == NULL) {
        printf("Error opening log file.\n");
        return false;
    }

    fprintf(file, "%s\n", message);

    fclose(file);
    return true;
}

// Function to save the restaurant data to a file
int save_to_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening save file.\n");
        return -1;
    }

    fprintf(file, "%d\n", r->tablenumber);

    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];
        fprintf(file, "%d,%d,%d,", table->x, table->y, table->reserved);

        if (table->reserved) {
            fprintf(file, "%s,%s\n", table->customer->name, table->customer->contact);
        } else {
            fprintf(file, ",\n");
        }
    }

    fclose(file);
    return 0;
}


bool load_from_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening save file.\n");
        return false;
    }

    fscanf(file, "%d", &r->tablenumber);
    printf("\nLoading %d table(s).\n", r->tablenumber);

    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];
        int reserved;
        fscanf(file, "%d,%d,%d,", &table->x, &table->y, &reserved);
        table->reserved = (reserved == 1);
        printf("\nLoading table %d: x=%d, y=%d, reserved=%d\n", i, table->x, table->y, table->reserved);

        if (table->reserved) {
            table->customer = (Customer *)malloc(sizeof(Customer));
            fscanf(file, " %[^\n],%s\n", table->customer->name, table->customer->contact);
            printf("Customer: name=%s, contact=%s\n", table->customer->name, table->customer->contact);
        } else {
            table->customer = NULL;
            fscanf(file, ",\n");
        }
    }

    fclose(file);
    return true;
}




// Function to print the contents of the save file
bool print_savefile_contents(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening save file.\n");
        return false;
    }

    printf("Contents of the save file:\n");
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
    return true;
}

// Function to add a table to the restaurant
void tableAdd(restaurant* r, int x, int y)
{
    // Check if the restaurant is already full
    if (r->tablenumber == 100)
    {
        printf("We have no room for extra tables!!\n");
        return;
    }

    // Check if there is already a table at that position
    for (int i = 0; i < r->tablenumber; i++)
    {
         if (r->tables[i].x == x && r->tables[i].y == y)
         {
             printf("\nNo free space at x = %d, y = %d.\n", x, y);
             return;
         }
     }

    // Create a new table
    Table tablenew = {x, y, false, NULL};

    // Add the new table to the restaurant
    r->tables[r -> tablenumber++] = tablenew;

    // Log the addition of the new table
    printf("Table Nr. %d added at position x = %d, y = %d.\n", r->tablenumber - 1, x, y);
    char log_message[100];
    snprintf(log_message, 100, "Table Nr. %d added at position x = %d, y = %d.\n", r -> tablenumber - 1, x, y);
    write_to_logfile(log_message);
}

// Function to remove a table from the restaurant
void tableRemove(restaurant* r, int x, int y)
{
    int index = -1;
    // Find the table at the given position
    for (int i = 0; i < r -> tablenumber; i++)
    {
        if (r -> tables[i].x == x && r -> tables[i].y == y)
        {
            index = i;
            break;
        }
    }

    // If the table was not found, print an error message and return
    if (index == -1) {
        printf("Table at position (%d, %d) does not exist.\n", x, y);
        return;
    }

    // Shift all tables after the removed table one position to the left
    for (int i = index; i < r -> tablenumber - 1; i++)
    {
        r -> tables[i] = r -> tables[i+1];
    }

    // Decrease the table count
    r -> tablenumber--;

    // Log the removal of the table
    printf("Table at position x = %d, y = %d has been removed.\n", x, y);
    char log_message[100];
    snprintf(log_message, 100,"Table at position x = %d, y = %d has been removed.\n", x, y);
    write_to_logfile(log_message);
    return;
}

// Function to reserve a table in the restaurant
void reserveTable(restaurant *r, int tableID, const char *name, const char *contact) {
    // Check if the table ID is valid
    if (tableID >= r->tablenumber) {
        printf("Table ID %d does not exist.\n", tableID);
        return;
    }

    // Get the table with the given ID
    Table *table = &r->tables[tableID];

    // Check if the table is already reserved
    if (table->reserved) {
        printf("Table ID %d is already reserved.\n", tableID);
        return;
    }

    // Allocate memory for a new customer
    Customer *new_customer = (Customer *)malloc(sizeof(Customer));

    // Copy the name and contact data into the customer struct
    strcpy(new_customer->name, name);
    strcpy(new_customer->contact, contact);

    // Get the current time for the reservation time
    new_customer->time_of_reservation = time(NULL);
    char reservation_time_str[20];
    strftime(reservation_time_str, sizeof(reservation_time_str), "%Y-%m-%d %H:%M:%S", localtime(&new_customer->time_of_reservation));

    // Reserve the table and assign the customer to the table
    table->reserved = true;
    table->customer = new_customer;

    // Log the table reservation
    printf("Table ID %d has been reserved for %s. Contact information: %s.\n", tableID, name, contact);
    char log_message[200];
    snprintf(log_message, 200,"Table ID %d has been reserved for %s. Contact information: %s.\nStart of "
            "Reservation: %s\n", tableID, name, contact,reservation_time_str);
    write_to_logfile(log_message);
}


// Function to free a table in the restaurant
void free_table(restaurant* r, int tableID) {
    // Check if the table ID is valid
    if (tableID < 0 || tableID >= r->tablenumber) {
            printf("Invalid table ID.\n\n");
            return;
    }

    // Get the table with the given ID
    Table* table = &r->tables[tableID];

    // Check if the table is not reserved
    if (!table->reserved)
    {
        printf("Table ID %d is not reserved.\n\n", tableID);
        return;
    }

    // Print the details of the table being freed
    printf("Table Nr. %d, reserved for %s (contact: %s) is now free.\n\n", tableID, table->customer->name, table->customer->contact);
    time_t freed_time = time(NULL);

    // Get the current time and the reservation start time
    char freed_time_str[20];
    strftime(freed_time_str, sizeof(freed_time_str), "%Y-%m-%d %H:%M:%S", localtime(&freed_time));
    char reservation_time_str[20];
    strftime(reservation_time_str, sizeof(reservation_time_str), "%Y-%m-%d %H:%M:%S", localtime(&table->customer->time_of_reservation));
    printf("Reservation time start: %s\nReservation time end: %s\n\n", reservation_time_str, freed_time_str);

    // Calculate the reservation duration
    double reservation_duration = difftime(freed_time, table->customer->time_of_reservation);
    printf("Table Nr. %d was reserved for %.0f seconds.\n", tableID, reservation_duration);

    // Log the table freeing
    char log_message[400];
    snprintf(log_message, 400,"Table Nr. %d, reserved for %s (contact: %s) is now free.\nReservation time start: %s\nReservation time end: %s\nReservation duration: %.0f seconds.\n", tableID, table->customer->name, table->customer->contact,reservation_time_str, freed_time_str, reservation_duration);
    write_to_logfile(log_message);

    // Free the customer from the table and mark the table as not reserved
    free(table->customer);
    table->customer = NULL;
    table->reserved = false;
}

// Unit tests for the newly added functions
void run_tests() {
    restaurant r;
    r.tablenumber = 0;

    // Test tableAdd function
    tableAdd(&r, 1, 1);
    tableAdd(&r, 2, 2);
    assert(r.tablenumber == 2);
    assert(r.tables[0].x == 1 && r.tables[0].y == 1);
    assert(r.tables[1].x == 2 && r.tables[1].y == 2);

    // Test tableRemove function
    tableRemove(&r, 1, 1);
    assert(r.tablenumber == 1);
    assert(r.tables[0].x == 2 && r.tables[0].y == 2);

    // Test reserveTable function
    reserveTable(&r, 0, "John Doe", "john.doe@example.com");
    assert(r.tables[0].reserved);
    assert(strcmp(r.tables[0].customer->name, "John Doe") == 0);
    assert(strcmp(r.tables[0].customer->contact, "john.doe@example.com") == 0);

    // Test free_table function
    free_table(&r, 0);
    assert(!r.tables[0].reserved);
}

int main()
{
    // Initialize the restaurant
    restaurant r;
    r.tablenumber = 0;

    // Run the unit tests
    run_tests();

    // Load data from a file (if available)
    load_from_file(&r, "restaurant_save.txt");

    // Print the current status of the restaurant
    printf("Current status of the restaurant:\n");
    for (int i = 0; i < r.tablenumber; i++) {
        Table table = r.tables[i];
        printf("Table ID: %d, Position: (%d, %d), Reserved: %s\n", i, table.x, table.y, table.reserved ? "Yes" : "No");
    }

    // Add some tables to the restaurant
    tableAdd(&r, 1, 1);
    tableAdd(&r, 2, 2);
    tableAdd(&r, 3, 3);

    // Reserve a table
    reserveTable(&r, 0, "John Doe", "john.doe@example.com");

    // Free a table
    free_table(&r, 0);

    // Save the restaurant data to a file
    save_to_file(&r, "restaurant_save.txt");

    // Print the contents of the save file
    print_savefile_contents("restaurant_save.txt");

    return 0;
}

