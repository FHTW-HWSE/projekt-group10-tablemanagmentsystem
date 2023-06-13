/*
 * theworld.c
 *
 *  Created on: Feb 27, 2023
 *      Author: osboxes
 */

#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

//Define a struct for customer information
typedef struct {
    char name[50];                      // Holds the name of the customer
    char contact[50];                   // Holds the contact information of the customer
    time_t time_of_reservation;         // Holds the time of reservation
} Customer;

// Define a struct to represent information that is contained in one table
typedef struct {
    int x;                  // X coordinates of the table
    int y;                  // Y coordinates of the table
    bool reserved;          // Holds a flag to indicate if the table is reserved or not
    Customer *customer;     // Holds a pointer to the customer who has reserved the table
    char covid_flag[4];     // Flag to indicate if the table is Covid flagged
    time_t start_time;      // Start time of the reservation
    time_t end_time;        // End time of the reservation
} Table;

// Define a struct to represent the information of the restaurant
typedef struct {
    Table tables[100];       // Array of 100 tables
    int tablenumber;         // Total number of tables in the restaurant
} restaurant;

// Function to write a log message to the logfile
void write_to_logfile(const char* message) {
	// Get the current timestamp
	time_t rawtime;
	struct tm* timeinfo;
	char timestamp[20];

	time(&rawtime);                       // Get the current time
	timeinfo = localtime(&rawtime);       // Convert the time to local time

	strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", timeinfo); // Format the time into a string

	// Open or Create the logfile in append mode
	FILE* logfile = fopen("restaurant_log.csv", "a");
	if (logfile == NULL) {
		printf("Error opening log file.\n");  // Print an error message if the file cannot be opened
		return;
	}

	//Write the log entry to the file
	fprintf(logfile, "%s: %s", timestamp, message);

	//Close the log File
	fclose(logfile);
}

// Function to save restaurant data to a file
int save_to_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "w");  // Open the file in write mode
    if (file == NULL) {
        printf("Error opening save file.\n");  // Print an error message if the file cannot be opened
        return -1;
    }

    // Write the number of tables to the file
    fprintf(file, "%d\n", r->tablenumber);

    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];  // Get the table at index i
        fprintf(file, "%d,%d,%d,%s,%s\n",
                table->x, table->y, table->reserved,
                table->reserved ? table->customer->name : "",  // Write the customer name if the table is reserved
                table->reserved ? table->customer->contact : "");  // Write the contact info if the table is reserved
    }

    fclose(file);  // Close the file
    return 0;
}

// Function to load restaurant data from a file
bool load_from_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "r");  // Open the file in read mode
    if (file == NULL) {
        printf("Error opening save file: %s\n", strerror(errno));  // Print an error message if the file cannot be opened
        return false;
    }

    fscanf(file, "%d", &r->tablenumber);  // Read the number of tables from the file
    printf("\nLoading %d table(s).\n", r->tablenumber);

    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];  // Get the table at index i
        int reserved;
        fscanf(file, "%d,%d,%d", &table->x, &table->y, &reserved);  // Read the table data from the file
        table->reserved = (bool)reserved;  // Convert the reserved flag to a boolean
        printf("\nLoading table %d: x=%d, y=%d, reserved=%d\n", i, table->x, table->y, table->reserved);

        if (table->reserved) {  // If the table is reserved
            table->customer = (Customer *)malloc(sizeof(Customer));  // Allocate memory for the customer
            fscanf(file, ",%[^,],%s", table->customer->name, table->customer->contact);  // Read the customer data from the file
            printf("Customer: name=%s, contact=%s\n", table->customer->name, table->customer->contact);
        } else {
            table->customer = NULL;  // If the table is not reserved, set the customer to NULL
            fscanf(file, ",,");
        }
    }

    fclose(file);  // Close the file
    return true;
}

// Function to print the contents of the save file
bool print_savefile_contents(const char *filename) {
    FILE *file = fopen(filename, "r");  // Open the file in read mode
    if (file == NULL) {
        printf("Error opening save file.\n");  // Print an error message if the file cannot be opened
        return false;
    }

    printf("Contents of the log file:\n");
    char line[256];
    while (fgets(line, sizeof(line), file)) {  // Read each line from the file
        printf("%s", line);  // Print the line
    }

    fclose(file);  // Close the file
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
void reserveTable(restaurant *r, int tableID, const char *name, const char *contact)
{
    // Check if the table ID is valid
    if (tableID >= r->tablenumber)
    {
        printf("Table ID %d does not exist.\n", tableID);
        return;
    }

    // Get the table with the given ID
    Table *table = &r->tables[tableID];

    // Check if the table is already reserved
    if (table->reserved)
    {
        printf("Table ID %d is already reserved.\n", tableID);
        return;
    }

    // Allocate memory for a new customer
    Customer *new_customer = (Customer *)malloc(sizeof(Customer));

    // Copy the name and contact data into the customer struct
    strncpy(new_customer->name, name, sizeof(new_customer->name) - 1);
    new_customer->name[sizeof(new_customer->name) - 1] = '\0';
    strncpy(new_customer->contact, contact, sizeof(new_customer->contact) - 1);
    new_customer->contact[sizeof(new_customer->contact) - 1] = '\0';

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

// Function to show reservation information from a file
void showReservationInfoFromFile(const char *filename, int filter) {
    // Declare a new restaurant
    restaurant r;

    // Load restaurant data from file, return error message if unsuccessful
    if (!load_from_file(&r, filename)) {
        printf("Error loading reservations from the save file.\n");
        return;
    }

    printf("\nTable reservation information:\n");

    // Initialize table count
    int table_count = 0;

    // Loop through tables in the restaurant
    for (int i = 0; i < r.tablenumber; i++) {
        // Get reference to current table
        Table *table = &r.tables[i];

        // Skip printing unreserved tables if filter is 1
        // Skip printing reserved tables if filter is 2
        if (filter == 1 && !table->reserved) {
            continue;
        } else if (filter == 2 && table->reserved) {
            continue;
        }

        // Print table information
        printf("Table ID: %d, Position: (%d, %d), Reserved: %s\n", i, table->x, table->y, table->reserved ? "Yes" : "No");
        table_count++;

        // If table is reserved, print customer details
        if (table->reserved) {
            printf("Customer Name: %s, Contact: %s\n\n", table->customer->name, table->customer->contact);
        } else {
            printf("\n");
        }
    }

    // If no tables are present, print message
    if (table_count == 0) {
        printf("There are no tables.\n\n");
    }
}

// Function to calculate the distance between two tables
float calculate_distance(Table *t1, Table *t2) {
    // Calculate difference in x and y coordinates
    int x_diff = t1->x - t2->x;
    int y_diff = t1->y - t2->y;

    // Return Euclidean distance
    return sqrt(x_diff * x_diff + y_diff * y_diff);
}

// Function to print occupied tables within a certain distance from a target table
void query_occupied_tables_within_distance(restaurant *r, int table_id, float distance) {
    // Get reference to target table
    Table *target_table = &r->tables[table_id];

    printf("Occupied tables within %.2f units from Table ID %d:\n\n", distance, table_id);

    // Initialize occupied table count
    int occupied_count = 0;

    // Loop through tables in the restaurant
    for (int i = 0; i < r->tablenumber; i++) {
        // Get reference to current table
        Table *table = &r->tables[i];

        // If table is reserved and within specified distance from target table, print its details
        if (table->reserved && calculate_distance(target_table, table) <= distance) {
            printf("Table ID: %d, Position: (%d, %d), Customer Name: %s, Contact: %s\n", i, table->x, table->y, table->customer->name, table->customer->contact);
            occupied_count++;
        }
    }

    // If no tables are within specified distance, print message
    if (occupied_count == 0) {
        printf("No occupied tables within the specified distance.\n");
    }
}

// Function to handle end-of-day tasks
void end_of_the_day(restaurant *r, const char *log_filename, const char *save_filename) {
    // Save the current state of restaurant, print error message if unsuccessful
    int save_result = save_to_file(r, save_filename);
    if (save_result == -1) {
        printf("Error saving the state.\n");
        return;
    }

    // Log that it's the end of the day
    write_to_logfile("End of the day.\n");

    // Create the "archive" directory if it doesn't exist
    struct stat st = {0};
    if (stat("archive", &st) == -1) {
        mkdir("archive", 0700);
    }

    // Get the current date
    time_t rawtime;
    struct tm *timeinfo;
    char date[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date, 20, "%Y-%m-%d", timeinfo);

    // Generate new filenames for archiving the log and save files
    char log_archive_filename[40];
    char save_archive_filename[40];

    snprintf(log_archive_filename, 40, "archive/%s_%s", date, log_filename);
    snprintf(save_archive_filename, 40, "archive/%s_%s", date, save_filename);

    // Print new file paths before renaming
    printf("\nMoving '%s' to '%s'\n", log_filename, log_archive_filename);
    printf("Moving '%s' to '%s'\n", save_filename, save_archive_filename);

    // Rename (move) the log and save files to the archive directory
    rename(log_filename, log_archive_filename);
    rename(save_filename, save_archive_filename);
}

// Function to clear input buffer
void clear_input_buffer() {
    int c;
    // Loop until the newline character or EOF (End Of File) is found
    while ((c = getchar()) != '\n' && c != EOF);
}

// Function to load files from an archive directory
int load_files_from_archive(restaurant *r) {
    DIR *dir; // Directory stream
    struct dirent *entry; // Structure representing directory entry
    char archive_dir[] = "archive";

    // Open a directory stream to the archive directory
    dir = opendir(archive_dir);
    if (dir == NULL) { // If the directory stream couldn't be opened, print an error and return -1
        printf("Error opening archive directory.\n");
        return -1;
    }

    printf("\nAvailable files in the archive:\n\n");

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // If the directory entry is a regular file and it matches a specific pattern, print its name
        if (entry->d_type == DT_REG) {
            if (strstr(entry->d_name, "_restaurant_save.csv") != NULL) {
                printf("Save file: %s\n", entry->d_name);
            } else if (strstr(entry->d_name, "_restaurant_log.csv") != NULL) {
                printf("Log file: %s\n", entry->d_name);
            }
        }
    }

    closedir(dir); // Close the directory stream

    // Input filename
    char filename[256];
    printf("\nEnter the filename you want to load: ");
    scanf("%s", filename);

    // Build full path to the file
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, filename);

    // Based on the filename, load the file content or print its content
    if (strstr(filename, "_restaurant_save.csv") != NULL) {
        printf("Detected save file.\n");
        return load_from_file(r, full_path) ? 0 : 1;
    } else if (strstr(filename, "_restaurant_log.csv") != NULL) {
        printf("Detected log file.\n");
        return print_savefile_contents(full_path) ? 0 : 2;
    } else {
        printf("Invalid file type.\n");
        return -1;
    }
}

// Function to display available log files in a given directory
void display_available_log_files(const char *folder_path) {
    DIR *dir; // Directory stream
    struct dirent *entry; // Structure representing directory entry

    // Open a directory stream
    dir = opendir(folder_path);
    if (dir == NULL) { // If the directory stream couldn't be opened, print an error and return
        printf("Error opening archive directory.\n");
        return;
    }

    printf("Available log files in the archive:\n");

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // If the directory entry is a regular file and it matches the log file pattern, print its name
        if (entry->d_type == DT_REG && strstr(entry->d_name, "_restaurant_log.csv") != NULL) {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir); // Close the directory stream
}

// Function to search for a customer's reservation in log files
void search_customer_in_log(const char *name) {
    char archive_dir[] = "archive";
    char filename[256];

    // Display available log files
    display_available_log_files(archive_dir);

    // Input filename
    printf("Enter the filename you want to load: ");
    scanf("%s", filename);

    // Build full path to the file
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, filename);

    // Open the log file
    FILE *file = fopen(full_path, "r");
    if (file == NULL) { // If the file couldn't be opened, print an error and return
        printf("Error opening log file: %s\n", full_path);
        return;
    }

    // Search for the customer's name in each line of the log file
    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, name) != NULL && strstr(line, "Table ID") != NULL && strstr(line, "has been reserved") != NULL) {
            printf("\nReservation found in file %s:\n%s\n", full_path, line);
        }
    }

    fclose(file); // Close the file
}

// Function to process a customer's COVID flag by their name
void process_covid_flagging_by_name(restaurant *r, time_t *k1_start, time_t *k1_end) {
    char archive_dir[] = "archive";
    char name[256];
    char log_file_name[256];
    char line[512];
    struct tm tm;

    // Input customer's name and log file name
    printf("Enter the name of the customer you want to flag for COVID: ");
    scanf("%s", name);
    printf("Enter the name of the log file (e.g., 2023-05-07_restaurant_log.csv): ");
    scanf("%s", log_file_name);

    // Build full path to the log file
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, log_file_name);

    // Open the log file
    FILE *file = fopen(full_path, "r");
    if (file == NULL) { // If the file couldn't be opened, print an error and return
        printf("Error opening log file: %s\n", full_path);
        return;
    }

    // Search for the customer's name in each line of the log file
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, name) != NULL) {
            // Parse the line to extract table ID, start and end times of reservation
            // If successful, write the customer's info into a COVID.csv file
            // Details of parsing and writing are beyond this comment
            // ...
        }
    }

    fclose(file); // Close the file
}

void process_covid_flagging_around_k1() {
    char archive_dir[] = "archive";
    char log_file_name[256];
    char line[512];
    int k1_table_id;
    int k1_x = -1, k1_y = -1; // -1 means not found

    // Prompt user to enter the table ID of the K1 table and the log file name
    printf("Enter the table ID of the K1 table: ");
    scanf("%d", &k1_table_id);
    printf("Enter the name of the log file (e.g., 2023-05-07_restaurant_log.csv): ");
    scanf("%s", log_file_name);

    // Open COVID.csv and check if the K1 table id exists
    FILE *covid_file = fopen("COVID.csv", "r");
    if (covid_file == NULL) {
        printf("Error opening COVID file: %s\n", strerror(errno));
        return;
    }

    // Loop through each line in the COVID.csv file
    while (fgets(line, sizeof(line), covid_file) != NULL) {
        char line_copy[512];
        strncpy(line_copy, line, sizeof(line_copy));  // Create a copy of the line

        // If the line contains the table id for K1
        if (atoi(strtok(line, ",")) == k1_table_id) {
            // Check if the line also contains the string "K1"
            if (strstr(line_copy, "K1") != NULL) {
                fclose(covid_file);

                // Open the log file and get the position of the K1 table
                char full_path[256];
                snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, log_file_name);
                FILE *file = fopen(full_path, "r");
                if (file == NULL) {
                    printf("Error opening log file: %s\n", full_path);
                    return;
                }

                // Loop through each line in the log file
                while (fgets(line, sizeof(line), file) != NULL) {
                    if (strstr(line, "Table Nr. ") != NULL) {
                        // Extract table ID from the line
                        char *table_id_str = strstr(line, "Table Nr. ");
                        table_id_str += strlen("Table Nr. "); // Skip the "Table Nr. " part
                        int table_id = atoi(table_id_str); // Convert to integer

                        // If the table ID matches the K1 table ID, extract its position
                        if (table_id == k1_table_id) {
                            char *x_str = strstr(line, "x = ");
                            char *y_str = strstr(line, "y = ");
                            if (x_str != NULL && y_str != NULL) {
                                x_str += strlen("x = ");
                                y_str += strlen("y = ");
                                k1_x = atoi(x_str);
                                k1_y = atoi(y_str);
                                break;
                            }
                        }
                    }
                }
                fclose(file);
                break;
            }
        }
    }

    // If the K1 table position was not found, print an error message and return
    if (k1_x == -1 || k1_y == -1) {
        printf("Could not find the position of the K1 table.\n");
        return;
    }

    // Open the log file again and flag tables around K1 as K2
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, log_file_name);
    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        printf("Error opening log file: %s\n", full_path);
        return;
    }

    // Loop through each line in the log file
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, "Table Nr. ") != NULL) {
            // Extract table ID from the line
            char *table_id_str = strstr(line, "Table Nr. ");
            table_id_str += strlen("Table Nr. "); // Skip the "Table Nr. " part
            int table_id = atoi(table_id_str); // Convert to integer

            // If the table ID is different from the K1 table ID, extract its position
            if (table_id != k1_table_id) {
                char *x_str = strstr(line, "x = ");
                char *y_str = strstr(line, "y = ");
                if (x_str != NULL && y_str != NULL) {
                    x_str += strlen("x = ");
                    y_str += strlen("y = ");
                    int x = atoi(x_str);
                    int y = atoi(y_str);

                    // If the table is around the K1 table, flag it as K2
                    if (abs(x - k1_x) <= 1 && abs(y - k1_y) <= 1) {
                        FILE *covid_file = fopen("COVID.csv", "a");
                        if (covid_file == NULL) {
                            printf("Error opening COVID file: %s\n", strerror(errno));
                            fclose(file);
                            return;
                        }
                        fprintf(covid_file, "%d,,K2\n", table_id); // we don't know the customer name and contact for K2
                        fclose(covid_file);

                        printf("Table ID %d has been flagged as K2.\n", table_id);
                    }
                }
            }
        }
    }
    fclose(file);
}

int main()
{
    // Declare and initialize restaurant variable
    restaurant r = {0};
    char input;
    // Save and log file names
    char save_file[] = "restaurant_save.csv";
    char log_file[] = "restaurant_log.csv";
    // Variables for K1 start and end times, initialize to -1
    time_t k1_start = -1;
    time_t k1_end = -1;
    
    do {
        // Ask if user wants to load previous save file or start a new one
        printf("Do you want to load the last save file or create a new one? (l = load, n = new): ");
        scanf(" %c", &input);

        // If user wants to load a previous save
        if (input == 'l') {
            // Load the last log file
            if (load_from_file(&r, save_file)) {
                break;
            } else {
                // If previous log file doesn't exist, start a new one
                printf("Creating a new log file as the previous one does not exist.\n\n");
                input = 'n';
            }
        // If user wants to start a new file
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
            printf("Starting with a new log file.\n");
        } else {
            printf("Invalid input. Please try again.\n");
        }
    } while (input != 'l' && input != 'n');

    while (true)
    {
        // Display menu for user to choose action
        printf("\nEnter a function:\n(a = add Table, r = remove Table, b to book a table,"
	    		"f to cancel a reservation,\n P to search for a customer in an old log file. \n s to show all tables. \n z to show archive logs or q to exit):");
        scanf(" %c", &input);

        // If user chooses to quit
        if (input == 'q') {
            printf("Do you want to end the day? (y/n): ");
            char end_day_input;
            scanf(" %c", &end_day_input);

            if (end_day_input == 'y' || end_day_input == 'Y') {
                // If user chooses to end the day, call end_of_the_day function and exit
                end_of_the_day(&r, log_file, save_file);
                printf("\nSee you soon!\n\n");
                break;
            } else {
                // If user does not choose to end the day, simply exit
                printf("\nSee you soon!\n\n");
                break;
            }

        // If user chooses to add a table
        } else if (input == 'a'){
            int x, y;
            // Get position for new table from user
            printf("\nEnter the position of the new table (x y): ");
            scanf("%d %d", &x, &y);
            // Call function to add table
            tableAdd(&r, x, y);
            // Save the changes to the file
            save_to_file(&r, save_file);

        // If user chooses to remove a table
        } else if (input == 'r') {
            int x, y;
            // Get position of table to be removed from user
                        printf("\nEnter position of table to remove (x y): ");
            scanf("%d %d", &x, &y);
            // Call function to remove table
            tableRemove(&r, x, y);
            // Save the changes to the file
            save_to_file(&r, save_file);

        // If user chooses to book a table
        } else if (input == 'b') {
            int id;
            char name[50];
            char contact[20];
            // Get booking details from user
            printf("\nEnter the table ID to book: ");
            scanf("%d", &id);
            printf("Enter the customer name: ");
            scanf(" %[^\n]%*c", name);
            printf("Enter the customer contact: ");
            scanf(" %[^\n]%*c", contact);
            printf("\n");
            // Call function to reserve table
            reserveTable(&r, id, name, contact);
            // Save the changes to the file
            save_to_file(&r, save_file);

        // If user chooses to cancel a reservation
        } else if (input == 'f') {
            int id;
            // Get ID of table to cancel reservation
            printf("\nEnter the table ID to cancel the reservation: ");
            scanf("%d", &id);
            // Call function to cancel reservation
            free_table(&r, id);
            // Save the changes to the file
            save_to_file(&r, save_file);

        // If user wants to see reservation information
        } else if (input == 's') {
            printf("\nDo you want to see all tables, only reserved tables, \nor only free tables? (a = all, r = reserved, f = free): ");
            char filter_choice;
            int filter = 0;
            scanf(" %c", &filter_choice);

            // Determine filter based on user input
            if (filter_choice == 'r') {
                filter = 1;
            } else if (filter_choice == 'f') {
                filter = 2;
            } else if (filter_choice != 'a') {
                printf("Invalid input. Please try again.\n");
                break;
            }
            // Show reservation information based on filter
            showReservationInfoFromFile(save_file, filter);

        // If user chooses to query occupied tables within a distance
        } else if (input == 'd') {
            printf("Enter the table ID and distance: ");
            int table_id, distance;
            scanf("%d %d", &table_id, &distance);
            // Call function to query occupied tables within a distance
            query_occupied_tables_within_distance(&r, table_id, distance);

        // If user chooses to load save file from archive
        } else if (input == 'z') {

            int result = load_files_from_archive(&r);
            if (result == 0) {
                printf("\nLoaded save file from archive successfully.\n");
            } else if (result == 1) {
                printf("Error loading save file from archive.\n");
            } else if (result == 2) {
                printf("Error printing log file contents.\n");
            } else {
                printf("Invalid file selected.\n");
            }

        // If user chooses to search for a customer in the log
        } else if (input == 'P') {
            char customer_name[256];
            printf("Enter the name of the customer you want to search for: ");
            scanf("%s", customer_name);

            // Call function to search for customer in the log
            search_customer_in_log(customer_name);

        // If user chooses to flag by name for Covid-19 exposure
        } else if (input == 'F') {
            // Call function to process Covid-19 flagging by name
            process_covid_flagging_by_name(&r, &k1_start, &k1_end);
            // If no reservation was found for specified customer, print error message
            if (k1_start == -1 || k1_end == -1) {
                printf("No reservation was found for the specified customer.\n");
            } else {
                // Otherwise, print start and end times of K1 reservation
                printf("K1 reservation start: %s", ctime(&k1_start));
                printf("K1 reservation end: %s", ctime(&k1_end));
            }

        // If user chooses to process Covid-19 flagging around K1
        } else if (input == 'K') {
            // Call function to process Covid-19 flagging around K1
            process_covid_flagging_around_k1();

        // If user enters invalid input
        } else {
            printf("Invalid input.\n");
        }
    }
    return 0;
}

