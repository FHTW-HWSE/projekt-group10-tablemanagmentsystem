# Project Name

Reservation System for Local Businesses

## Introduction

In light of recent events, it has become apparent that it will be important to keep track of where people have been in the future. To this end, a reservation system for local businesses must be developed. This reservation system must be able to record neighboring seating groups. Since not every venue looks the same, it must be possible to specify the position of each table. It must also be possible to determine retroactively which tables were occupied at a distance of less than a certain distance from a requested table. To keep the effort low, only one contact person must be saved per table. It can also be assumed that a reservation never includes more guests than the possible number of seats at the table.

## Requirements

The following tasks must be fulfilled by the program:

- Implementation of a user interface
- Adding and removing tables by specifying Cartesian coordinates
- Allocation of a table when specifying a contact person
- Saving the table and the contact person
- Querying all occupied tables when specifying a table and a distance.

## Usage

The program can be used by running the executable file. The user interface allows the user to perform the required tasks, such as adding and removing tables, allocating tables to customers, and querying occupied tables.

**# Table Managment System**
**1. Start
1. Initialize table list
1. Show main menu with options:
- Add table
- Remove table
- Reserve table
- View occupied tables within a certain distance of a given table
- Quit
1. If user selects "Add table", go to step 5, else go to step 6
1. Ask user for table coordinates, add table to list, return to main menu
1. If user selects "Remove table", go to step 7, else go to step 8
1. Ask user for table ID to remove, remove table from list, return to main menu
1. If user selects "Reserve table", go to step 9, else go to step 10
1. Ask user for table ID and contact person name, reserve table, return to main menu
1. If user selects "View occupied tables", go to step 11, else go to step 3
1. Ask user for table ID and distance, display occupied tables within distance, return to main menu
1. Quit program

# --------------------------------------------------------


**# USER STORIES**
# Table Reservation
### User Story
As an employee,
I want to reserve a table.
So that the customer can have a seat in the restaurant in advance.

### Acceptance Criteria
Given: empty places at the wished reservation time
When: the customer calls or walk in for the reservation
Then:
- Check if there is a table for the desired time
- Reserve the table for the customer
- Write down a name and a telephone number/e-mail

### Implementation
1. Check if there is a table available at the desired time.
2. If the table is available, reserve it for the customer.
3. Record the customer's name and contact information in a reservation system.


# Seating Customers
### User Story
As an employee,
I want seat the customers at their table.
So that the customer can have a seat in the restaurant.

### Acceptance Criteria
Given: the customer has a reservation or there are empty tables which aren’t reserved.
When: the customer arrives at the restaurant
Then:
- Check for a reservation -> table number
    - If no reservation -> check if there is available seating
- Check if there is a name and contact information in the database 
    - If not ask and enter the information right after seating
- Enter the time when the customer has been seated

### Implementation
1. Check if the customer has a reservation and if so, seat them at the reserved table.
2. If there is no reservation, check if there is available seating and assign a table number to the customer.
3. Check if the customer's name and contact information is in the database, and if not, prompt the customer for this information.
4. Record the time when the customer has been seated.


# Closing a Table
### User Story
As an employee,
I want to close a table.
So that in the system, the time a customer was in the restaurant is logged.

### Acceptance Criteria
Given: customer has been served and paid
When: After the customer leaves their table/restaurant
Then:
- Log the time they left -> into the system
- Clean the table

### Implementation
1. Check if the customer has paid and is ready to leave the table.
2. Record the time the customer left the table/restaurant.
3. Clean the table for the next customer.


# Notification of Close Contact with Infected Person
### User Story
As management,
I want to notify customers and employees who have been in close contact with an infected person.
So that they are aware that they might be infected.

### Acceptance Criteria
Given: someone infected was in the restaurant
When: after being notified someone infected was at the restaurant
Then:
- Look in the system when the infected customer was in the restaurant
- Find the tables in the risky area and notify all the contacts seated in those tables
- Find the employees who worked a shift during that time and notify them to reschedule their shifts.

### Implementation
1. Retrieve information from the system about the infected customer's visit to the restaurant.
2. Identify the tables in the risky area, which are close to the infected customer's table.
3. Notify all customers who were seated at the identified tables about the potential exposure to the infected person.
4. Identify the employees who worked a shift during the time the infected customer was in the restaurant.
5. Notify the identified employees to reschedule their shifts.

# Seating Arrangement
### User Story
As management,
I want to rearrange the seatings.
So that if a bigger group needs more tables together they can sit and the restaurant can still track who was close to the table.

### Acceptance Criteria
Given: a bigger table is needed or change back to default table setting
When: a big group or default or a table has to be added for more people
Then:
- Change the table seatings in the program
- The program calculates which tables are in the possible infected area
- Change the table in the restaurant

### Implementation
1. Receive a request to change the seating arrangement.
2. Change the table seatings in the system according to the request.
3. Calculate the tables that are in the possible infected area.
4. Update the seating arrangement in the restaurant.

# Monitoring the Number of People in the Restaurant
### User Story
As a system,
I want to know always how many people are in the restaurant.
So that management can be informed if the headcount exceeds the limit.

### Acceptance Criteria
Given: the restaurant is open and all customers and employees who are there are in the system
When: the number of people exceeds the limit allowed in the restaurant
Then: alert management that there are too many people in the restaurant

### Implementation
1. Continuously monitor the number of people in the restaurant.
2. Compare the number of people with the limit allowed in the restaurant.
3. If the number of people exceeds the limit, alert the management.

# Customer table check online - Reservation
### User Story
As a customer,
I want to check if there is available seating even if I don’t have a reservation.
So that I can spontaneously walk into the restaurant without reserving before.

### Acceptance Criteria
Given: customers can check how many tables are free before being there in person or call
When: the restaurant is open

Then:
-Check online how many people are in the restaurant (if it is even possible)
-State how many guests are planned
-Check if it is possible
-If yes, offer the option to reserve it for a short period of time directly online.

### Implementation
1.	Continuously monitor the number of people in the restaurant.
2.	Check if the customer has a reservation.
3.	If not, provide the customer with the option to check how many tables are free.
4.	If tables are free, provide the customer with the option to reserve it for a short period of time directly online

# Infrom the restaurant when infected
### User Story
As a customer,
I want to inform the restaurant I was infected
So that the restaurant knows who to contact.

### Acceptance Criteria
Given: Customer was infected
When: After knowing the customer was infected
Then:
-	Either call the restaurant and tell them or write in the system that one has been infected.
-	Give name and contact information for the restaurant for follow-up questions.

### Implementation
1.	Create a form where the customer can fill in their name, contact information, and date of visit.
2.	Notify the restaurant via email or SMS about the customer's infection and provide them with the customer's details.
3.	Provide the restaurant with a way to follow up with the customer for contact tracing purposes.


# Reservation Notification
### User Story
As a system,
I want to notify customers about their reservations coming up soon
So that the person is notified an hour before the reservation.

### Acceptance Criteria
Given: Customer has a reservation for a table and a contact info was given
When: An hour before the reservation
Then: Write a short email/SMS that their reservation is coming up.

### Implementation
1.	Store the customer's reservation details, including their contact information and reservation time.
2.	Set up a system to send a notification via email or SMS to the customer an hour before their reservation time.
3.	Ensure that the notification includes the reservation time and the restaurant's contact information for any changes or cancellations.



# --------------------------------------------------------

**# Functions and what they could look like**

### main: 
The main function initializes the table list and displays the main menu. It then waits for the user to select an option from the menu and calls the appropriate function based on the user's choice.

### add table:
This function prompts the user for the coordinates of the new table and adds it to the table list.

### remove table: 
This function prompts the user for the ID of the table to remove and removes it from the table list.

### reserve table: 
This function prompts the user for the ID of the table to reserve and the contact person's name. It then marks the table as occupied in the table list.

### view_occupied_tables: 
This function prompts the user for the ID of the table and a distance. It then searches through the table list to find all occupied tables within that distance of the given table and displays them to the user.

### calculate_distance:
This function calculates the distance between two tables based on their coordinates.
