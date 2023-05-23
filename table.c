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

typedef struct {
	char name[50];
	char contact[50];
	time_t time_of_reservation;
} Customer;

/**
 * represents information that is contained in one table
*/
typedef struct {

    int x;          ///< x coordinates of the table
    int y;          ///< y coordinates of the table
    bool reserved;  ///< reserverd or not
    Customer *customer;  ///<
    char covid_flag[4]; //for covid flagged
    time_t start_time;
    time_t end_time;
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

int save_to_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening save file.\n");
        return -1;
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
    return 0;
}

bool load_from_file(restaurant *r, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening save file: %s\n", strerror(errno));
        return false;
    }

    fscanf(file, "%d", &r->tablenumber);
    printf("\nLoading %d table(s).\n", r->tablenumber);

    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];
        int reserved;
        fscanf(file, "%d,%d,%d", &table->x, &table->y, &reserved);
        table->reserved = (bool)reserved;
        printf("\nLoading table %d: x=%d, y=%d, reserved=%d\n", i, table->x, table->y, table->reserved);

        if (table->reserved) {
            table->customer = (Customer *)malloc(sizeof(Customer));
            fscanf(file, ",%[^,],%s", table->customer->name, table->customer->contact);
            printf("Customer: name=%s, contact=%s\n", table->customer->name, table->customer->contact);
        } else {
            table->customer = NULL;
            fscanf(file, ",,");
        }
    }

    fclose(file);
    return true;
}

bool print_savefile_contents(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening save file.\n");
        return false;
    }

    printf("Contents of the log file:\n");
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
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

    printf("Table Nr. %d added at position x = %d, y = %d.\n", r->tablenumber - 1, x, y);
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

    new_customer->time_of_reservation = time(NULL);
    char reservation_time_str[20];
    strftime(reservation_time_str, sizeof(reservation_time_str), "%Y-%m-%d %H:%M:%S", localtime(&new_customer->time_of_reservation));

    //table is now reserved
    table->reserved = true;

    //associate the new_customer structure with the table structure (via pointer)

    table->customer = new_customer;
    printf("Table ID %d has been reserved for %s. Contact information: %s.\n", tableID, name, contact);
    char log_message[200];
    snprintf(log_message, 200,"Table ID %d has been reserved for %s. Contact information: %s.\nStart of "
    		"Reservation: %s\n", tableID, name, contact,reservation_time_str);
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
	    time_t freed_time = time(NULL);

	    char freed_time_str[20];
	    strftime(freed_time_str, sizeof(freed_time_str), "%Y-%m-%d %H:%M:%S", localtime(&freed_time));
	    char reservation_time_str[20];
	    strftime(reservation_time_str, sizeof(reservation_time_str), "%Y-%m-%d %H:%M:%S", localtime(&table->customer->time_of_reservation));
	    printf("Reservation time start: %s\nReservation time end: %s\n\n", reservation_time_str, freed_time_str);

	    double reservation_duration = difftime(freed_time, table->customer->time_of_reservation);
	    printf("Table Nr. %d was reserved for %.0f seconds.\n", tableID, reservation_duration);

	    char log_message[400];
	    snprintf(log_message, 400,"Table Nr. %d, reserved for %s (contact: %s) is now free.\nReservation time start: %s\nReservation time end: %s\nReservation duration: %.0f seconds.\n", tableID, table->customer->name, table->customer->contact,reservation_time_str, freed_time_str, reservation_duration);
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

void end_of_the_day(restaurant *r, const char *log_filename, const char *save_filename) {
    // Save the current state
    int save_result = save_to_file(r, save_filename);
    if (save_result == -1) {
        printf("Error saving the state.\n");
        return;
    }
    write_to_logfile("End of the day.\n");

    // Create the archive directory if it doesn't exist
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

    // Generate the new filenames
    char log_archive_filename[40];
    char save_archive_filename[40];

    snprintf(log_archive_filename, 40, "archive/%s_%s", date, log_filename);
    snprintf(save_archive_filename, 40, "archive/%s_%s", date, save_filename);

    // Print file paths before renaming
    printf("\nMoving '%s' to '%s'\n", log_filename, log_archive_filename);
    printf("Moving '%s' to '%s'\n", save_filename, save_archive_filename);

    // Move and rename the log and save files
    rename(log_filename, log_archive_filename);
    rename(save_filename, save_archive_filename);
}


void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int load_files_from_archive(restaurant *r) {
    DIR *dir;
    struct dirent *entry;
    char archive_dir[] = "archive";

    dir = opendir(archive_dir);
    if (dir == NULL) {
        printf("Error opening archive directory.\n");
        return -1;
    }

    printf("\nAvailable files in the archive:\n\n");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (strstr(entry->d_name, "_restaurant_save.csv") != NULL) {
                printf("Save file: %s\n", entry->d_name);
            } else if (strstr(entry->d_name, "_restaurant_log.csv") != NULL) {
                printf("Log file: %s\n", entry->d_name);
            }
        }
    }

        closedir(dir);


        char filename[256];
        printf("\nEnter the filename you want to load: ");
        scanf("%s", filename);

        char full_path[256];
        snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, filename);

        if (strstr(filename, "_restaurant_save.csv") != NULL) {
                printf("Detected save file.\n"); // Add this line for debugging
                return load_from_file(r, full_path) ? 0 : 1;
            } else if (strstr(filename, "_restaurant_log.csv") != NULL) {
                printf("Detected log file.\n"); // Add this line for debugging
                return print_savefile_contents(full_path) ? 0 : 2;
            } else {
                printf("Invalid file type.\n"); // Add this line for debugging
                return -1;
            }
        }

void display_available_log_files(const char *folder_path) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(folder_path);
    if (dir == NULL) {
        printf("Error opening archive directory.\n");
        return;
    }

    printf("Available log files in the archive:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, "_restaurant_log.csv") != NULL) {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

void search_customer_in_log(const char *name) {
    char archive_dir[] = "archive";
    char filename[256];

    display_available_log_files(archive_dir);

    printf("Enter the filename you want to load: ");
    scanf("%s", filename);

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, filename);

    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        printf("Error opening log file: %s\n", full_path);
        return;
    }


    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, name) != NULL && strstr(line, "Table ID") != NULL && strstr(line, "has been reserved") != NULL) {
            printf("\nReservation found in file %s:\n%s\n", full_path, line);
        }
    }

    fclose(file);
}


void process_covid_flagging_by_name(restaurant *r, time_t *k1_start, time_t *k1_end) {
    char archive_dir[] = "archive";
    char name[256];
    char log_file_name[256];
    char line[512];
    struct tm tm;

    printf("Enter the name of the customer you want to flag for COVID: ");
    scanf("%s", name);
    printf("Enter the name of the log file (e.g., 2023-05-07_restaurant_log.csv): ");
    scanf("%s", log_file_name);

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, log_file_name);
    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        printf("Error opening log file: %s\n", full_path);
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, name) != NULL) {
            // Extract table ID from the line
            char *table_id_str = strstr(line, "Table Nr. ");
            if (table_id_str != NULL) {
                table_id_str += strlen("Table Nr. "); // Skip the "Table Nr. " part
                int table_id = atoi(table_id_str); // Convert to integer

                if (table_id < 0) {
                    printf("Table ID %d is not valid.\n", table_id);
                    continue;
                }

                // Extract start and end times of reservation
                char *res_start_str = strstr(line, "Start of Reservation: ");
                char *res_end_str = strstr(line, "Reservation time end: ");
                if (res_start_str != NULL && strptime(res_start_str + strlen("Start of Reservation: "), "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
                    *k1_start = mktime(&tm);
                } else {
                    printf("Could not find or parse reservation start time in line: %s\n", line);
                }
                if (res_end_str != NULL && strptime(res_end_str + strlen("Reservation time end: "), "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
                    *k1_end = mktime(&tm);
                } else {
                    printf("Could not find or parse reservation end time in line: %s\n", line);
                }

                // Write to COVID.csv file
                FILE *covid_file = fopen("COVID.csv", "a");
                if (covid_file == NULL) {
                    printf("Error opening COVID file: %s\n", strerror(errno));
                    fclose(file);
                    return;
                }

                fprintf(covid_file, "%d,%s,K1\n", table_id, name);
                fclose(covid_file);

                printf("COVID file has been updated.\n");
            }
        }
    }

    fclose(file);
}

void process_covid_flagging_around_k1() {
    char archive_dir[] = "archive";
    char log_file_name[256];
    char line[512];
    int k1_table_id;
    int k1_x = -1, k1_y = -1; // -1 means not found

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
    printf("COVID.csv file opened successfully.\n");  // Debug print statement

    while (fgets(line, sizeof(line), covid_file) != NULL) {
        printf("Test1.\n");  // Debug print statement
        char line_copy[512];
        strncpy(line_copy, line, sizeof(line_copy));  // Create a copy of the line
        if (atoi(strtok(line, ",")) == k1_table_id) { // strtok splits the line at the comma
        	printf("Test2\n");  // Debug print statement
        	        if (strstr(line_copy, "K1") != NULL) { // check if the copy of the line has "K1"
        	            printf("Test3\n");  // Debug print statement
        	            fclose(covid_file);
        	            printf("Test4\n");  // Debug print statement

                // Open the log file and get the position of the K1 table
                char full_path[256];
                snprintf(full_path, sizeof(full_path), "%s/%s", archive_dir, log_file_name);
                FILE *file = fopen(full_path, "r");
                if (file == NULL) {
                    printf("Error opening log file: %s\n", full_path);
                    return;
                }
                printf("Log file opened successfully.\n");  // Debug print statement
                while (fgets(line, sizeof(line), file) != NULL) {
                    printf("Checking line in log file: %s", line);  // Print each line that is checked

                    if (strstr(line, "Table Nr. ") != NULL) {
                        // Extract table ID from the line
                        char *table_id_str = strstr(line, "Table Nr. ");
                        table_id_str += strlen("Table Nr. "); // Skip the "Table Nr. " part
                        int table_id = atoi(table_id_str); // Convert to integer

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
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, "Table Nr. ") != NULL) {
            // Extract table ID from the line
            char *table_id_str = strstr(line, "Table Nr. ");
            table_id_str += strlen("Table Nr. "); // Skip the "Table Nr. " part
            int table_id = atoi(table_id_str); // Convert to integer

            if (table_id != k1_table_id) {
            char *x_str = strstr(line, "x = ");
            char *y_str = strstr(line, "y = ");
            if (x_str != NULL && y_str != NULL) {
                x_str += strlen("x = ");
                y_str += strlen("y = ");
                int x = atoi(x_str);
                int y = atoi(y_str);

                if (abs(x - k1_x) <= 1 && abs(y - k1_y) <= 1) {
                    // The table is around the K1 table, flag it as K2
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
	restaurant r = {0};
	char input;
    char save_file[] = "restaurant_save.csv";
    char log_file[] = "restaurant_log.csv";
    time_t k1_start = -1;
    time_t k1_end = -1;
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
	         printf("Starting with a new log file.\n");
	     } else {
	    	 printf("Invalid input. Please try again.\n");
	     }
	} while (input != 'l' && input != 'n');

	while (true)
	{
	    printf("\nEnter a function:\n(a = add Table, r = remove Table, b to book a table,"
	    		"f to cancel a reservation, \n s to show all reservations or q to exit):");
	    scanf(" %c", &input);

	    if (input == 'q') {
	        printf("Do you want to end the day? (y/n): ");
	        char end_day_input;
	        scanf(" %c", &end_day_input);

	    if (end_day_input == 'y' || end_day_input == 'Y') {
	        end_of_the_day(&r, log_file, save_file);
	        printf("\nSee you soon!\n\n");
	        break;
	      } else {
	        printf("\nSee you soon!\n\n");
	        break;
	      }

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
        	 printf("Enter the customer name: ");
        	 scanf(" %[^\n]%*c", name);
        	 printf("Enter the customer contact: ");
        	 scanf(" %[^\n]%*c", contact);
        	 printf("\n");
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

        }else if (input == 'z') {

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

        }else if (input == 'P') {
        	char customer_name[256];
        	printf("Enter the name of the customer you want to search for: ");
        	scanf("%s", customer_name);

        	search_customer_in_log(customer_name);

	 	}else if (input == 'F') {
	 	    process_covid_flagging_by_name(&r, &k1_start, &k1_end);
	 	   if (k1_start == -1 || k1_end == -1) {
	 	       printf("No reservation was found for the specified customer.\n");
	 	   } else {
	 	       printf("K1 reservation start: %s", ctime(&k1_start));
	 	       printf("K1 reservation end: %s", ctime(&k1_end));
	 	   }


	 	} else if (input == 'K') {
	 	    process_covid_flagging_around_k1();

	 	} else {
        	printf("Invalid input.\n");
        }
    }
	return 0;
}

