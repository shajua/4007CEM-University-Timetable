#include <iostream>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

using namespace std;

#define CLASS_CODE    (50)
#define CLASS_NAME     (100)
#define TEACHER_NAME   (50)
#define CLASS_LOCATION    (100)
#define CLASS_TIME    (50)

#define CLASS_INSER_QUERY_SIZE \
    (strlen("INSERT INTO time_table VALUES(\"\",\"\",\"\",\"\",\"\");") + \
    CLASS_CODE + CLASS_NAME + TEACHER_NAME + CLASS_LOCATION + CLASS_TIME)




// User info structure
struct TimeTable
{
    char class_code[CLASS_CODE];
    char class_name[CLASS_NAME];
    char class_teacher[TEACHER_NAME];
    char class_location[CLASS_LOCATION];
    char class_time[CLASS_TIME];
};
typedef struct TimeTable TimeTable;


bool table_exists(sqlite3* db, const char* table_name);
bool create_class_table(sqlite3* db);
bool insert_class(sqlite3* db, TimeTable* classs);
void display_class(sqlite3* db);
void fill_class(sqlite3* db);

int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;

    // SQLite database variable
    sqlite3* db;
    int rc;

    // Open the SQLite database 'class.db' (Create a new file if it does not exists)
    rc = sqlite3_open("class.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // If the 'class' table does not exists
    if(!table_exists(db, "class")) {
        // Create 'class' table
        if(!create_class_table(db)) {
            sqlite3_close(db);
            return 1;
        }
    }

   // run unitl user want to exit
    char choice;
    do {
        cout << "1. Insert Class" << endl;
        cout << "2. Display Class" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cout << endl;

        switch(choice) {
            case '1':
                fill_class(db);
                break;
            case '2':
                display_class(db);
                break;
            case '3':
                break;
            default:
                cout << "Invalid choice" << endl;
        }
    } while(choice != '3');
   

    


    putc('\n', stdout);


    // Close the SQLite database
    sqlite3_close(db);

    return 0;
}




// Check if a table exists
bool table_exists(sqlite3* db, const char* table_name)
{
    sqlite3_stmt* statement;
    int rc;

    const char* sql_table_list = "SELECT name FROM sqlite_master WHERE type='table'";

    rc = sqlite3_prepare_v2(db, sql_table_list, strlen(sql_table_list), &statement, NULL);
    if(rc == SQLITE_OK) {
        // Loop through all the tables
        while(sqlite3_step(statement) == SQLITE_ROW) {
            if(!strcmp((const char*) sqlite3_column_text(statement, 0), table_name))
                return true;
        }
    }

    return false;
}

// Create 'class' table
bool create_class_table(sqlite3* db)
{
    sqlite3_stmt* statement;
    int rc;

    // SQL query to create the 'class' table
    const char* sql_create_class_table = "CREATE TABLE class(" \
        "class_code TEXT PRIMARY KEY," \
        "class_name TEXT," \
        "class_teacher TEXT," \
        "class_location TEXT," \
        "class_time TEXT);";



    // Execute the previous query to create the 'class' table
    rc = sqlite3_prepare_v2(db, sql_create_class_table, strlen(sql_create_class_table), &statement, NULL);
    if(rc == SQLITE_OK) {
        rc = sqlite3_step(statement);
        sqlite3_finalize(statement);

    } else {
        fprintf(stderr, "Can't initialize the database: %s\n", sqlite3_errmsg(db));
        return false;
    }

    return true;
}

bool insert_class(sqlite3* db, TimeTable* classs)
{
    sqlite3_stmt* statement;
    int rc;

    // Prepare the SQL query to insert the new class
    const char* sql_insert_class = "INSERT INTO class VALUES(?, ?, ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sql_insert_class, strlen(sql_insert_class), &statement, NULL);
    if(rc == SQLITE_OK) {
        // Bind the values to the variables
        sqlite3_bind_text(statement, 1, classs->class_code, strlen(classs->class_code), SQLITE_STATIC);
        sqlite3_bind_text(statement, 2, classs->class_name, strlen(classs->class_name), SQLITE_STATIC);
        sqlite3_bind_text(statement, 3, classs->class_teacher, strlen(classs->class_teacher), SQLITE_STATIC);
        sqlite3_bind_text(statement, 4, classs->class_location, strlen(classs->class_location), SQLITE_STATIC);
        sqlite3_bind_text(statement, 5, classs->class_time, strlen(classs->class_time), SQLITE_STATIC);

        // Execute the query
        rc = sqlite3_step(statement);
        sqlite3_finalize(statement);

        if(rc == SQLITE_DONE) {
            return true;


        }
        else {
            fprintf(stderr, "Can't insert the class: %s\n", sqlite3_errmsg(db));
            return false;
        }
        
    }
    
    sqlite3_finalize(statement);

    return true;
}

void display_class(sqlite3* db)
{
    sqlite3_stmt* statement;
    int rc;

    // Prepare the SQL query to select all the classes
    const char* sql_select_class = "SELECT * FROM class;";

    rc = sqlite3_prepare_v2(db, sql_select_class, strlen(sql_select_class), &statement, NULL);
    if(rc == SQLITE_OK) {
        // Loop through all the classes
        while(sqlite3_step(statement) == SQLITE_ROW) {
           
            // Display table header
            printf("\n\n");
            printf("+------------------------------------------------------+\n");
            printf("|                                                      |\n");
            printf("|                                                      |\n");
            printf("|                                                      |\n");

            // Display the class information
            printf("|  Class Code: %-20s |\n", sqlite3_column_text(statement, 0));
            printf("|  Class Name: %-20s |\n", sqlite3_column_text(statement, 1));
            printf("|  Class Teacher: %-20s |\n", sqlite3_column_text(statement, 2));
            printf("|  Class Location: %-20s |\n", sqlite3_column_text(statement, 3));
            printf("|  Class Time: %-20s |\n", sqlite3_column_text(statement, 4));

           

        }
    }

    sqlite3_finalize(statement);
}

void fill_class(sqlite3* db){
    // Specify the number of class to be added
    int class_num = -1;

    
    do {
        printf("Please enter the number of class to be added: ");
        if(scanf("%d", &class_num) != 1) {
          fprintf(stderr, "Error, please try again!\n");
          scanf("%*c");
        }
    } while(class_num < 0);


    // Add class
    for(int i = 0; i < class_num; ++i) {

         TimeTable class_info;
        cin.ignore();
        printf("Please enter the Class (%d) code (%d max): ", i+1, CLASS_CODE);
        // scanf("%s", class_info.class_code);
        cin.getline(class_info.class_code, CLASS_CODE);

        printf("Please enter the  Class (%d) name (%d max): ", i+1, CLASS_NAME);
        // scanf("%s", class_info.class_name);
        cin.getline(class_info.class_name, CLASS_NAME);

        printf("Please enter the  Class (%d) teacher (%d max): ", i+1, TEACHER_NAME);
        // scanf("%s", class_info.class_teacher);
        cin.getline(class_info.class_teacher, TEACHER_NAME);

        printf("Please enter the  Class (%d) location (%d max): ", i+1, CLASS_LOCATION);
        // scanf("%s", class_info.class_location);
        cin.getline(class_info.class_location, CLASS_LOCATION);

        printf("Please enter the  Class (%d) time (%d max): ", i+1, CLASS_TIME);
        // scanf("%s", class_info.class_time);
        cin.getline(class_info.class_time, CLASS_TIME);



        if(!insert_class(db, &class_info)) {
            printf("Re-try to add class (%d)", i+1);
            i--;
        }
    }

   

    
}