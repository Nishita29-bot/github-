/* =========================
   PARKING RECORD SYSTEM
   ========================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>

#define TWO_WHEELER_SLOTS 10
#define FOUR_WHEELER_SLOTS 10

/* -------- STRUCTURES -------- */

struct User
{
    char username[20];
    char password[20];
    char role[10];
};

struct Parking
{
    char vehicleNo[20];
    int type;

    int inH, inM;
    int outH, outM;

    char status[5];

    int receiptNo;
};

/* -------- GLOBAL VARIABLES -------- */

int occupied2W = 0;
int occupied4W = 0;

int dailyIncome = 0;

int receiptCounter = 1;

char currentRole[10];

/* -------- FUNCTION DECLARATIONS -------- */

void gotoxy(int x, int y);

void pressAnyKey();

void inputHiddenPassword(char pass[]);

void registerUser();

int loginUser();

int calculateFee(int type,
                 int inH,
                 int inM,
                 int outH,
                 int outM);

int isDuplicateVehicle(char vno[]);

void vehicleEntry();

void vehicleExit();

void viewRecords();

void searchRecord();

void viewDailyIncome();

void updateRecord();

void deleteRecord();

void adminMenu();

void userMenu();

/* -------- GOTOXY -------- */
void gotoxy(int x, int y)
{
    COORD c;

    c.X = x;
    c.Y = y;

    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE), c);
}

/*------------PRESS ANY KEY-------------*/

void pressAnyKey()
{
    gotoxy(25,22);
    
    printf("Press any key to continue...............");
    
    getch();
}

/* -------- HIDDEN PASSWORD -------- */

void inputHiddenPassword(char pass[])
{
    char ch;

    int i = 0;

    while (1)
    {
        ch = getch();

        if (ch == 13)
            break;

        else if (ch == 8 && i > 0)
        {
            printf("\b \b");
            i--;
        }

        else if (i < 19)
        {
            pass[i++] = ch;
            printf("*");
        }
    }

    pass[i] = '\0';
}

/* -------- REGISTER USER -------- */

void registerUser()
{
    struct User u;

    FILE *fp;

    fp = fopen("users.txt", "a");

    if(fp == NULL)
    {
        gotoxy(30,10);
        printf("File Error!");

        getch();

        return;
    }

    system("cls");

    gotoxy(30,4);
    printf("******** USER REGISTRATION ********");

    gotoxy(30,6);
    printf("Enter Username: ");
    scanf("%s", u.username);

    gotoxy(30,7);
    printf("Enter Password: ");
    inputHiddenPassword(u.password);

    gotoxy(30,8);
    printf("Enter Role (admin/user): ");
    scanf("%s", u.role);

    fprintf(fp,
            "%s %s %s\n",
            u.username,
            u.password,
            u.role);

    fclose(fp);

    gotoxy(30,10);
    printf("User Registered Successfully!");

    getch();
}

/* -------- LOGIN FUNCTION -------- */

int loginUser()
{
    struct User u;

    char uname[20];
    char pass[20];

    int attempts = 3;

    while(attempts > 0)
    {
        FILE *fp;

        fp = fopen("users.txt", "r");

        if(fp == NULL)
        {
            gotoxy(30,10);
            printf("No Users Found!");

            getch();

            return 0;
        }

        system("cls");

        gotoxy(30,4);
        printf("******** LOGIN ********");

        gotoxy(30,6);
        printf("Enter Username: ");
        scanf("%s", uname);

        gotoxy(30,7);
        printf("Enter Password: ");
        inputHiddenPassword(pass);

        int valid = 0;

        while(fscanf(fp,
                     "%s %s %s",
                     u.username,
                     u.password,
                     u.role) != EOF)
        {
            if(strcmp(u.username, uname) == 0 &&
               strcmp(u.password, pass) == 0)
            {
                valid = 1;

                strcpy(currentRole, u.role);

                break;
            }
        }

        fclose(fp);

        if(valid)
        {
            gotoxy(30,9);
            printf("Login Successful!");

            gotoxy(30,10);
            printf("Role: %s", currentRole);

            getch();

            return 1;
        }

        else
        {
            attempts--;

            gotoxy(30,9);
            printf("Invalid Credentials!");

            gotoxy(30,10);
            printf("Attempts Left: %d", attempts);

            getch();
        }
    }

    gotoxy(30,12);
    printf("Too Many Failed Attempts!");

    getch();

    exit(0);
}

/* -------- CALCULATE FEE -------- */

int calculateFee(int type,
                 int inH,
                 int inM,
                 int outH,
                 int outM)
{
    int totalInMinutes =
        inH * 60 + inM;

    int totalOutMinutes =
        outH * 60 + outM;

    int totalMinutes =
        totalOutMinutes - totalInMinutes;

    if(totalMinutes <= 0)
        return -1;

    int hours = totalMinutes / 60;

    if(totalMinutes % 60 != 0)
        hours++;

    if(type == 2)
        return hours * 20;

    else
        return hours * 40;
}

/* -------- DUPLICATE VEHICLE -------- */

int isDuplicateVehicle(char vno[])
{
    FILE *fp;

    struct Parking p;

    fp = fopen("parking.txt", "r");

    if(fp == NULL)
        return 0;

    while(fscanf(fp,
                 "%s %d %d %d %d %d %s %d",
                 p.vehicleNo,
                 &p.type,
                 &p.inH,
                 &p.inM,
                 &p.outH,
                 &p.outM,
                 p.status,
                 &p.receiptNo) != EOF)
    {
        if(strcmp(p.vehicleNo, vno) == 0 &&
           strcmp(p.status, "IN") == 0)
        {
            fclose(fp);

            return 1;
        }
    }

    fclose(fp);

    return 0;
}

/* -------- VEHICLE ENTRY -------- */

void vehicleEntry()
{
    struct Parking p;

    FILE *fp;

    fp = fopen("parking.txt", "a");

    if(fp == NULL)
    {
        gotoxy(30,10);
        printf("File Error!");

        pressAnyKey();
        return;
    }

    system("cls");

    gotoxy(30,4);
    printf("******** VEHICLE ENTRY ********");

    gotoxy(30,6);
    printf("Enter Vehicle Number: ");

    scanf(" %[^\n]", p.vehicleNo);

    if(isDuplicateVehicle(p.vehicleNo))
    {
        gotoxy(30,8);
        printf("Vehicle Already Inside!");

        fclose(fp);

        pressAnyKey();
        return;
    }

    gotoxy(30,7);
    printf("Vehicle Type (2/4): ");
    scanf("%d", &p.type);

    /* SLOT CHECK */

    if(p.type == 2)
    {
        if(occupied2W >= TWO_WHEELER_SLOTS)
        {
            gotoxy(30,9);
            printf("2 Wheeler Slots Full!");

            fclose(fp);

            pressAnyKey();
            return;
        }
    }

    else if(p.type == 4)
    {
        if(occupied4W >= FOUR_WHEELER_SLOTS)
        {
            gotoxy(30,9);
            printf("4 Wheeler Slots Full!");

            fclose(fp);

            pressAnyKey();
            return;
        }
    }

    else
    {
        gotoxy(30,9);
        printf("Invalid Vehicle Type!");

        fclose(fp);

       pressAnyKey();
        return;
    }

    gotoxy(30,8);
    printf("Enter Entry Time (HH MM): ");
    scanf("%d %d", &p.inH, &p.inM);

    p.outH = 0;
    p.outM = 0;

    strcpy(p.status, "IN");

    p.receiptNo = receiptCounter++;

    fprintf(fp,
            "%s %d %d %d %d %d %s %d\n",
            p.vehicleNo,
            p.type,
            p.inH,
            p.inM,
            p.outH,
            p.outM,
            p.status,
            p.receiptNo);

    fclose(fp);

    if(p.type == 2)
        occupied2W++;

    else
        occupied4W++;

    gotoxy(30,10);
    printf("Vehicle Entered Successfully!");

    pressAnyKey();
}


/* -------- VEHICLE EXIT -------- */

void vehicleExit()
{
    struct Parking p[100];
    FILE *fp;

    char vno[20];
    int n = 0, i;
    int found = 0;

    system("cls");

    gotoxy(30,4);
    printf("******** VEHICLE EXIT ********");

    gotoxy(30,6);
    printf("Enter Vehicle Number: ");

    scanf(" %[^\n]", vno);

    fp = fopen("parking.txt", "r");

    if(fp == NULL)
    {
        gotoxy(30,8);
        printf("No Records Found!");
        getch();
        return;
    }

    while(fscanf(fp,
        "%s %d %d %d %d %d %s %d",
        p[n].vehicleNo,
        &p[n].type,
        &p[n].inH,
        &p[n].inM,
        &p[n].outH,
        &p[n].outM,
        p[n].status,
        &p[n].receiptNo) != EOF)
    {
        n++;
    }

    fclose(fp);

    for(i = 0; i < n; i++)
    {
        if(strcmp(p[i].vehicleNo, vno) == 0)
        {
            found = 1;

            if(strcmp(p[i].status, "OUT") == 0)
            {
                gotoxy(30,8);
                printf("Vehicle Already Exited!");
                pressAnyKey();
                return;
            }

            gotoxy(30,7);
            printf("Enter Exit Time (HH MM): ");
            scanf("%d %d", &p[i].outH, &p[i].outM);

            int fee = calculateFee(
                p[i].type,
                p[i].inH,
                p[i].inM,
                p[i].outH,
                p[i].outM
            );

            if(fee < 0)
            {
                gotoxy(30,9);
                printf("Invalid Exit Time!");
               pressAnyKey();
                return;
            }

            strcpy(p[i].status, "OUT");

            dailyIncome += fee;

            if(p[i].type == 2)
                occupied2W--;
            else
                occupied4W--;

            gotoxy(30,10);
            printf("Parking Fee: Rs. %d", fee);
            
            FILE *fp2 = fopen("income.txt", "a");
           if(fp2 != NULL)
            {
            fprintf(fp2, "%d\n", fee);
            fclose(fp2);
            }
            pressAnyKey();
            break;
        }
    }

    if(!found)
    {
        gotoxy(30,10);
        printf("Vehicle Not Found!");
        getch();
        return;
    }

    fp = fopen("parking.txt", "w");

    for(i = 0; i < n; i++)
    {
        fprintf(fp,
            "%s %d %d %d %d %d %s %d\n",
            p[i].vehicleNo,
            p[i].type,
            p[i].inH,
            p[i].inM,
            p[i].outH,
            p[i].outM,
            p[i].status,
            p[i].receiptNo);
    }

    fclose(fp);
}

/* -------- VIEW RECORDS -------- */

void viewRecords()
{
    struct Parking p;
    FILE *fp;
    int y = 9;
    int found = 0;

    system("cls");

    gotoxy(25,2);
    printf("============= PARKING RECORDS =============");

    gotoxy(5,5);
    printf("--------------------------------------------------------------------------");

    gotoxy(5,6);
    printf("| %-15s | %-5s | %-8s | %-8s | %-8s | %-7s |",
           "Vehicle No",
           "Type",
           "Entry",
           "Exit",
           "Status",
           "Receipt");

    gotoxy(5,7);
    printf("--------------------------------------------------------------------------");

    fp = fopen("parking.txt", "r");

    if(fp == NULL)
    {
        gotoxy(30,10);
        printf("No Records Found!");
        pressAnyKey();
        return;
    }

    while(fscanf(fp,
                 "%s %d %d %d %d %d %s %d",
                 p.vehicleNo,
                 &p.type,
                 &p.inH,
                 &p.inM,
                 &p.outH,
                 &p.outM,
                 p.status,
                 &p.receiptNo) == 8)
    {
        found = 1;

        gotoxy(5, y);

        printf("| %-15s | %-5d | %02d:%02d    | %02d:%02d    | %-8s | %-7d |",
               p.vehicleNo,
               p.type,
               p.inH,
               p.inM,
               p.outH,
               p.outM,
               p.status,
               p.receiptNo);

        y++;

        if(y > 20)   
            break;
    }

    fclose(fp);

    gotoxy(5, y);
    printf("--------------------------------------------------------------------------");

    if(!found)
    {
        gotoxy(30,10);
        printf("No Records Available!");
    }

    pressAnyKey();
}

/* -------- SEARCH RECORD -------- */

void searchRecord()
{
    struct Parking p;
    FILE *fp;
    char vno[20];
    int found = 0;

    system("cls");

    gotoxy(30,4);
    printf("******** SEARCH RECORD ********");

    gotoxy(30,6);
    printf("Enter Vehicle Number: ");

    scanf(" %[^\n]", vno);

    fp = fopen("parking.txt", "r");

    if(fp == NULL)
    {
        gotoxy(30,8);
        printf("No Records Found!");
        getch();
        return;
    }

    while(fscanf(fp,
        "%s %d %d %d %d %d %s %d",
        p.vehicleNo,
        &p.type,
        &p.inH,
        &p.inM,
        &p.outH,
        &p.outM,
        p.status,
        &p.receiptNo) != EOF)
    {
        if(_stricmp(p.vehicleNo, vno) == 0)
        {
            found = 1;

            gotoxy(30,9);
            printf("Receipt No : %d", p.receiptNo);

            gotoxy(30,10);
            printf("Vehicle No : %s", p.vehicleNo);

            gotoxy(30,11);
            printf("Type : %d", p.type);

            gotoxy(30,12);
            printf("Status : %s", p.status);

            break;
        }
    }

    fclose(fp);

    if(!found)
    {
        gotoxy(30,10);
        printf("Vehicle Not Found!");
    }

   pressAnyKey();
}

/* -------- DAILY INCOME -------- */

void viewDailyIncome()
{
    FILE *fp;
    int fee;
    int total = 0;

    system("cls");

    gotoxy(30,4);
    printf("******** DAILY INCOME ********");

    fp = fopen("income.txt", "r");

    if(fp == NULL)
    {
        gotoxy(30,8);
        printf("No Income Records Found!");
        getch();
        return;
    }

    while(fscanf(fp, "%d", &fee) == 1)
    {
        total += fee;
    }

    fclose(fp);

    gotoxy(30,8);
    printf("Total Income: Rs. %d", total);

   pressAnyKey();
}


/* -------- ADMIN MENU -------- */

void adminMenu()
{
    int choice;

    do
    {
        system("cls");

        gotoxy(30,4);
        printf("******** ADMIN MENU ********");

        gotoxy(30,6);
        printf("1. Vehicle Entry");

        gotoxy(30,7);
        printf("2. Vehicle Exit");

        gotoxy(30,8);
        printf("3. View Records");

        gotoxy(30,9);
        printf("4. Search Record");

        gotoxy(30,10);
        printf("5. Update Record");

        gotoxy(30,11);
        printf("6. Delete Record");

        gotoxy(30,12);
        printf("7. View Daily Income");

        gotoxy(30,13);
        printf("8. Logout");

        gotoxy(30,15);
        printf("Enter Choice: ");

        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                vehicleEntry();
                break;

            case 2:
                vehicleExit();
                break;

            case 3:
                viewRecords();
                break;

            case 4:
                searchRecord();
                break;

            case 5:
                updateRecord();
                break;

            case 6:
                deleteRecord();
                break;

            case 7:
                viewDailyIncome();
                break;

            case 8:
                break;

            default:
                gotoxy(30,17);
                printf("Invalid Choice!");

                pressAnyKey();
        }

    }
    while(choice != 8);
}
/* -------- USER MENU -------- */

void userMenu()
{
    int choice;

    do
    {
        system("cls");

        gotoxy(30,4);
        printf("******** USER MENU ********");

        gotoxy(30,6);
        printf("1. Vehicle Entry");

        gotoxy(30,7);
        printf("2. Vehicle Exit");

        gotoxy(30,8);
        printf("3. View Status");

        gotoxy(30,9);
        printf("4. Logout");

        gotoxy(30,11);
        printf("Enter Choice: ");

        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                vehicleEntry();
                break;

            case 2:
                vehicleExit();
                break;

            case 3:
                searchRecord();
                break;

            case 4:
                break;

            default:
                gotoxy(30,13);
                printf("Invalid Choice!");

                pressAnyKey();
        }

    }
    while(choice != 4);
}
 /*----------UPDATE RECORD-----------*/
 
void updateRecord()
{
    struct Parking p[100];
    FILE *fp;
    char vno[20];
    int n = 0, i, found = 0;

    system("cls");

    gotoxy(25,5);
    printf("******** UPDATE RECORD ********");

    gotoxy(25,7);
    printf("Enter Vehicle Number: ");
    scanf(" %[^\n]", vno);

    fp = fopen("parking.txt", "r");
    if(fp == NULL)
    {
        gotoxy(25,9);
        printf("No Records Found!");
        pressAnyKey();
        return;
    }

    while(fscanf(fp,
        "%s %d %d %d %d %d %s %d",
        p[n].vehicleNo,
        &p[n].type,
        &p[n].inH,
        &p[n].inM,
        &p[n].outH,
        &p[n].outM,
        p[n].status,
        &p[n].receiptNo) != EOF)
    {
        n++;
    }
    fclose(fp);

    for(i = 0; i < n; i++)
    {
        if(_stricmp(p[i].vehicleNo, vno) == 0)
        {
            found = 1;

            gotoxy(25,9);
            printf("New Vehicle No: ");
            scanf(" %[^\n]", p[i].vehicleNo);

            gotoxy(25,10);
            printf("Vehicle Type (2/4): ");
            scanf("%d", &p[i].type);

            gotoxy(25,11);
            printf("Entry Time (HH MM): ");
            scanf("%d %d", &p[i].inH, &p[i].inM);

            gotoxy(25,13);
            printf("Record Updated Successfully!");

            break;
        }
    }

    if(!found)
    {
        gotoxy(25,9);
        printf("Vehicle Not Found!");
        getch();
        return;
    }

    fp = fopen("parking.txt", "w");
    for(i = 0; i < n; i++)
    {
        fprintf(fp,
            "%s %d %d %d %d %d %s %d\n",
            p[i].vehicleNo,
            p[i].type,
            p[i].inH,
            p[i].inM,
            p[i].outH,
            p[i].outM,
            p[i].status,
            p[i].receiptNo);
    }
    fclose(fp);

    pressAnyKey();
}

/*----------DELETE RECORD-----------*/

void deleteRecord()
{
    struct Parking p[100];
    FILE *fp;
    char vno[20];
    int n = 0, i, found = 0;

    system("cls");

    gotoxy(25,5);
    printf("******** DELETE RECORD ********");

    gotoxy(25,7);
    printf("Enter Vehicle Number: ");
    scanf(" %[^\n]", vno);

    fp = fopen("parking.txt", "r");
    if(fp == NULL)
    {
        gotoxy(25,9);
        printf("No Records Found!");
        pressAnyKey();
        return;
    }

    while(fscanf(fp,
        "%s %d %d %d %d %d %s %d",
        p[n].vehicleNo,
        &p[n].type,
        &p[n].inH,
        &p[n].inM,
        &p[n].outH,
        &p[n].outM,
        p[n].status,
        &p[n].receiptNo) != EOF)
    {
        n++;
    }
    fclose(fp);

    fp = fopen("parking.txt", "w");

    for(i = 0; i < n; i++)
    {
        if(_stricmp(p[i].vehicleNo, vno) == 0)
        {
            found = 1;

            if(strcmp(p[i].status, "IN") == 0)
            {
                if(p[i].type == 2)
                    occupied2W--;
                else
                    occupied4W--;
            }
            continue;
        }

        fprintf(fp,
            "%s %d %d %d %d %d %s %d\n",
            p[i].vehicleNo,
            p[i].type,
            p[i].inH,
            p[i].inM,
            p[i].outH,
            p[i].outM,
            p[i].status,
            p[i].receiptNo);
    }

    fclose(fp);

    gotoxy(25,9);
    if(found)
        printf("Record Deleted Successfully!");
    else
        printf("Vehicle Not Found!");

   pressAnyKey();
}

/* -------- MAIN FUNCTION -------- */

int main()
{
    int choice;

    int loggedIn = 0;

    while(!loggedIn)
    {
        system("cls");

        gotoxy(25,4);
        printf("******** WELCOME TO PARKING SYSTEM ********");

        gotoxy(30,6);
        printf("1. Register");

        gotoxy(30,7);
        printf("2. Login");

        gotoxy(30,8);
        printf("3. Exit");

        gotoxy(30,10);
        printf("Enter Choice: ");

        scanf("%d", &choice);

        if(choice == 1)
        {
            registerUser();
        }

        else if(choice == 2)
        {
            loggedIn = loginUser();

            if(loggedIn)
            {
                if(strcmp(currentRole, "admin") == 0)
                {
                    adminMenu();
                }

                else
                {
                    userMenu();
                }

                loggedIn = 0;
            }
        }

        else if(choice == 3)
        {
            system("cls");

            gotoxy(25,10);
            printf("THANK YOU FOR USING SYSTEM");

            getch();

            exit(0);
        }

        else
        {
            gotoxy(30,12);
            printf("Invalid Choice!");

            pressAnyKey();
        }
    }

    return 0;
}
