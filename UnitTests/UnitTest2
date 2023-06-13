#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <time.h>

//------------------------------- WICHTIG -------------------------------
//bitte mithilfe von
//							gcc -o UnitTest2 UnitTest2.c -lm
//wegen dem math.h
//-----------------------------------------------------------------------


typedef struct {
    char name[50];
    char contact[20];
} customer;

typedef struct {
    int x;
    int y;
    int reserved;
    customer *customer;
} Table;

typedef struct {
    int tablenumber;
    Table *tables;
} restaurant;

// Function to load restaurant data from a file
int load_from_file(restaurant *r, const char *filename) {
    // Check if the file exists
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }

    // Read the number of tables
    if (fscanf(file, "%d\n", &r->tablenumber) != 1) {
        fclose(file);
        return 0;
    }

    // Allocate memory for the tables
    r->tables = (Table *)malloc(r->tablenumber * sizeof(Table));
    if (r->tables == NULL) {
        fclose(file);
        return 0;
    }

    // Read table data
    for (int i = 0; i < r->tablenumber; i++) {
        Table *table = &r->tables[i];
        if (fscanf(file, "%d %d %d\n", &table->x, &table->y, &table->reserved) != 3) {
            fclose(file);
            free(r->tables);
            return 0;
        }
        if (table->reserved) {
            table->customer = (customer *)malloc(sizeof(customer));
            if (table->customer == NULL) {
                fclose(file);
                free(r->tables);
                return 0;
            }
            if (fscanf(file, "%s %s\n", table->customer->name, table->customer->contact) != 2) {
                fclose(file);
                free(table->customer);
                free(r->tables);
                return 0;
            }
        } else {
            table->customer = NULL;
        }
    }

    fclose(file);
    return 1;
}

// Function to save the current state of the restaurant to a file
int save_to_file(const restaurant *r, const char *filename) {
    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return -1;
    }

    // Write the number of tables
    fprintf(file, "%d\n", r->tablenumber);

    // Write table data
    for (int i = 0; i < r->tablenumber; i++) {
        const Table *table = &r->tables[i];
        fprintf(file, "%d %d %d\n", table->x, table->y, table->reserved);
        if (table->reserved) {
            fprintf(file, "%s %s\n", table->customer->name, table->customer->contact);
        }
    }

    fclose(file);
    return 0;
}

// Function to write a message to the log file
void write_to_logfile(const char *message) {
    // Open the log file in append mode
    FILE *file = fopen("log.txt", "a");
    if (file == NULL) {
        printf("Error opening the log file.\n");
        return;
    }

    // Write the message to the log file
    fprintf(file, "%s", message);

    fclose(file);
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

// Unit Test for load_from_file function
void test_load_from_file() {
    printf("Testing load_from_file function...\n");

    // Create a test file with reservation data
    const char *filename = "test_data.txt";
    FILE *file = fopen(filename, "w");
    fprintf(file, "3\n");
    fprintf(file, "1 2 0\n");
    fprintf(file, "2 3 1\n");
    fprintf(file, "3 4 1\n");
    fprintf(file, "John Doe johndoe@example.com\n");
    fprintf(file, "Jane Smith janesmith@example.com\n");
    fclose(file);

    // Create a restaurant structure
    restaurant r;
    int result = load_from_file(&r, filename);

    // Check if the loading was successful
    if (result) {
        printf("Load from file successful.\n");
        printf("Number of tables: %d\n", r.tablenumber);
        for (int i = 0; i < r.tablenumber; i++) {
            Table *table = &r.tables[i];
            printf("Table ID: %d, Position: (%d, %d), Reserved: %s\n", i, table->x, table->y, table->reserved ? "Yes" : "No");
            if (table->reserved) {
                printf("Customer Name: %s, Contact: %s\n", table->customer->name, table->customer->contact);
            }
        }
    } else {
        printf("Error loading from file.\n");
    }

    // Clean up the test file
    remove(filename);

    printf("Test complete.\n\n");
}

// Unit Test for save_to_file function
void test_save_to_file() {
    printf("Testing save_to_file function...\n");

    // Create a test restaurant structure
    restaurant r;
    r.tablenumber = 3;
    r.tables = (Table *)malloc(3 * sizeof(Table));

    Table *table1 = &r.tables[0];
    table1->x = 1;
    table1->y = 2;
    table1->reserved = 0;
    table1->customer = NULL;

    Table *table2 = &r.tables[1];
    table2->x = 2;
    table2->y = 3;
    table2->reserved = 1;
    table2->customer = (customer *)malloc(sizeof(customer));
    strcpy(table2->customer->name, "John Doe");
    strcpy(table2->customer->contact, "johndoe@example.com");

    Table *table3 = &r.tables[2];
    table3->x = 3;
    table3->y = 4;
    table3->reserved = 1;
    table3->customer = (customer *)malloc(sizeof(customer));
    strcpy(table3->customer->name, "Jane Smith");
    strcpy(table3->customer->contact, "janesmith@example.com");

    // Save the restaurant data to a file
    int result = save_to_file(&r, "test_data.txt");

    // Check if the saving was successful
    if (result == 0) {
        printf("Save to file successful.\n");
    } else {
        printf("Error saving to file.\n");
    }

    // Clean up the test restaurant structure
    free(table2->customer);
    free(table3->customer);
    free(r.tables);

    printf("Test complete.\n\n");
}

// Unit Test for showReservationInfoFromFile function
void test_showReservationInfoFromFile() {
    printf("Testing showReservationInfoFromFile function...\n");

    // Create a test file with reservation data
    const char *filename = "test_data.txt";
    FILE *file = fopen(filename, "w");
    fprintf(file, "3\n");
    fprintf(file, "1 2 0\n");
    fprintf(file, "2 3 1\n");
    fprintf(file, "3 4 1\n");
    fprintf(file, "John Doe johndoe@example.com\n");
    fprintf(file, "Jane Smith janesmith@example.com\n");
    fclose(file);

    // Call the function to show reservation information
    printf("Filter: All tables\n");
    showReservationInfoFromFile(filename, 0);
    printf("\nFilter: Only reserved tables\n");
    showReservationInfoFromFile(filename, 1);
    printf("\nFilter: Only unreserved tables\n");
    showReservationInfoFromFile(filename, 2);

    // Clean up the test file
    remove(filename);

    printf("Test complete.\n\n");
}

// Unit Test for calculate_distance function
void test_calculate_distance() {
    printf("Testing calculate_distance function...\n");

    // Create two test tables
    Table t1, t2;

    t1.x = 1;
    t1.y = 2;
    t1.reserved = 0;
    t1.customer = NULL;

    t2.x = 4;
    t2.y = 6;
    t2.reserved = 1;
    t2.customer = (customer *)malloc(sizeof(customer));
    strcpy(t2.customer->name, "John Doe");
    strcpy(t2.customer->contact, "johndoe@example.com");

    // Calculate the distance between the tables
    float distance = calculate_distance(&t1, &t2);
    printf("Distance between tables: %.2f units\n", distance);

    // Clean up the test table
    free(t2.customer);

    printf("Test complete.\n\n");
}

// Unit Test for query_occupied_tables_within_distance function
void test_query_occupied_tables_within_distance() {
    printf("Testing query_occupied_tables_within_distance function...\n");

    // Create a test restaurant structure
    restaurant r;
    r.tablenumber = 4;
    r.tables = (Table *)malloc(4 * sizeof(Table));

    Table *table1 = &r.tables[0];
    table1->x = 1;
    table1->y = 2;
    table1->reserved = 0;
    table1->customer = NULL;

    Table *table2 = &r.tables[1];
    table2->x = 2;
    table2->y = 3;
    table2->reserved = 1;
    table2->customer = (customer *)malloc(sizeof(customer));
    strcpy(table2->customer->name, "John Doe");
    strcpy(table2->customer->contact, "johndoe@example.com");

    Table *table3 = &r.tables[2];
    table3->x = 3;
    table3->y = 4;
    table3->reserved = 1;
    table3->customer = (customer *)malloc(sizeof(customer));
    strcpy(table3->customer->name, "Jane Smith");
    strcpy(table3->customer->contact, "janesmith@example.com");

    Table *table4 = &r.tables[3];
    table4->x = 5;
    table4->y = 6;
    table4->reserved = 1;
    table4->customer = (customer *)malloc(sizeof(customer));
    strcpy(table4->customer->name, "Alice Johnson");
    strcpy(table4->customer->contact, "alicejohnson@example.com");

    // Query occupied tables within a distance from a target table
    query_occupied_tables_within_distance(&r, 0, 4.0);

    // Clean up the test restaurant structure
    free(table2->customer);
    free(table3->customer);
    free(table4->customer);
    free(r.tables);

    printf("Test complete.\n\n");
}

// Unit Test for end_of_the_day function
void test_end_of_the_day() {
    printf("Testing end_of_the_day function...\n");

    // Create a test restaurant structure
    restaurant r;
    r.tablenumber = 2;
    r.tables = (Table *)malloc(2 * sizeof(Table));

    Table *table1 = &r.tables[0];
    table1->x = 1;
    table1->y = 2;
    table1->reserved = 0;
    table1->customer = NULL;

    Table *table2 = &r.tables[1];
    table2->x = 2;
    table2->y = 3;
    table2->reserved = 1;
    table2->customer = (customer *)malloc(sizeof(customer));
    strcpy(table2->customer->name, "John Doe");
    strcpy(table2->customer->contact, "johndoe@example.com");

    // Perform end-of-day tasks
    end_of_the_day(&r, "test_log.txt", "test_save.txt");

    // Clean up the test restaurant structure
    free(table2->customer);
    free(r.tables);

    printf("Test complete.\n\n");
}

int main() {
    // Run unit tests
    test_load_from_file();
    test_save_to_file();
    test_showReservationInfoFromFile();
    test_calculate_distance();
    test_query_occupied_tables_within_distance();
    test_end_of_the_day();

    return 0;
}
