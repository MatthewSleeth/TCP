/***************Server program**************************/

/* new_server.c is on eros.cs.txstate.edu
   open a window on eros.
   compile and run the server program first:
   gcc -O3 -g -std=c99 new_server.c -o new_server

   $./new_server
*/

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 1024
#define PORT 8080
#define SA struct sockaddr



void addUser(char *fname, char *lname) {
 
    FILE* f = fopen("students.txt", "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    for (int i = 0; i < 11; i++) {
        if (fname[i] == '\n') {
            fname[i] = fname[i + 1];
        }
        if (lname[i] == '\n') {
            lname[i] = lname[i + 1];
        }
    }

    int id = rand() % (900000) + 100000;  // generates random 6 digit id number
    fprintf(f, "%d ", id);  // write student id number to file

    fprintf(f, "%s ", fname);   // write student first name to file

    fprintf(f, "%s ", lname);   // write student last name to file

    int score = rand() % (41) + 60;  // generates random score between 60 and 99
    fprintf(f, "%d\n", score);  // write student score to file

    fclose(f);
}

char * displayID(int id) {
    
    char buff[MAX];
    char sid[7];
    char first6[7];
    char returnInfo[MAX];
    char studentInfo[MAX];
    int count = 0;
    bool keepReading = true;

    FILE* f = fopen("students.txt", "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    sprintf(sid, "%d", id);
    
    // searches through file to find and display user with matching id
    do {
        fgets(studentInfo, 50, f);

        for (int i = 0; i < 6; i++) {
            first6[i] = studentInfo[i];
        }


        if (feof(f)) {
            fclose(f);

            FILE* f = fopen("students.txt", "r");
            if (f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }
            count++;
        }
        else if (strcmp(first6, sid) == 0) {
            keepReading = false;
            strncpy(returnInfo, studentInfo, MAX);
        }

        if (count == 2) {
            strncpy(returnInfo, "Student not found...\n", MAX);
            keepReading = false;
        }

        memset(studentInfo, 0, sizeof(studentInfo));
    } while (keepReading);

    fclose(f);

    return returnInfo;
}

char * displayScore(int score) {

    char buff[MAX];
    char myScore[4];
    int j = 0, studentScore = 0, space = 0;
    char returnCopy[MAX];
    char studentInfo[50];
    char studentCopy[50];
    bool keepReading = true;

    FILE* f = fopen("students.txt", "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    bzero(returnCopy, MAX);
    strcat(returnCopy, "\n");
    
    // searches through file to check if a students score is greater than the desired score
    do {
        fgets(studentInfo, 50, f);

        for (int i = 0; i < 50; i++) {
            studentCopy[i] = studentInfo[i];
            if (space == 3) {
                myScore[j] = studentInfo[i];
                j++;
            }
            else {
                if (studentInfo[i] == ' ') {
                    space++;
                }
            }
        }

        studentScore = atoi(myScore);

        if (feof(f)) {
            keepReading = false;
        }
        else if (studentScore > score) {
            strcat(returnCopy, studentCopy);
        }

        space = 0;
        j = 0;
        memset(myScore, 0, sizeof(myScore));
        memset(studentInfo, 0, sizeof(studentInfo));
        memset(studentCopy, 0, sizeof(studentCopy));
    } while (keepReading);

    fclose(f);

    printf("%s\n", returnCopy);

    return returnCopy;
}

char * displayAll() {

    char returnCopy[MAX];
    char studentInfo[50];
    bool keepReading = true;

    FILE* f = fopen("students.txt", "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    bzero(returnCopy, MAX);
    strcat(returnCopy, "\n");
    
    do {
        fgets(studentInfo, 50, f);

        if (feof(f)) {
            keepReading = false;
        }
        else {
            strcat(returnCopy, studentInfo);
        }

        memset(studentInfo, 0, sizeof(studentInfo));

    } while (keepReading);

    fclose(f);

    return returnCopy;

}

void deleteUser(int id) {

    char buff[MAX];
    int count = 0;
    char sid[7];
    char first6[7];
    char studentInfo[50];
    bool keepReading = true;

    FILE* f = fopen("students.txt", "r");
    FILE* temp = fopen("temp____students.txt", "w");
    if (f == NULL || temp == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    do {
        fgets(studentInfo, 50, f);

        for (int i = 0; i < 6; i++) {
            first6[i] = studentInfo[i];
        }
        sprintf(sid, "%d", id);

        if (feof(f)) {
            keepReading = false;

        }
        else if (strcmp(first6, sid) != 0) {
            fputs(studentInfo, temp);
        }
        else {
            // Do nothing
        }

        memset(studentInfo, 0, sizeof(studentInfo));
        count++;
    } while (keepReading);

    fclose(f);
    fclose(temp);

    remove("students.txt");
    rename("temp____students.txt", "students.txt");
}

// Function designed to send menu to client.
void menu(int connfd)
{
    char buff[MAX];

    strcpy(buff, "Select a command to run.\n0. Menu \n1. ADD \n2. Display ID \n3. Display Score \n4. Display All \n5. Delete ID \n6. Exit \n ");
    
    // and send that buffer to client
    write(connfd, buff, sizeof(buff));
    
}

// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    int n, choice, studentID, studentScore;
    char firstName[11];
    char lastName[11];
    char student[MAX];
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s", buff);
        choice = atoi(buff);
        bzero(buff, MAX);
        switch (choice) {
        case 1:
            // get first name
            strcpy(buff, "Enter the student's First name (10 characters max): ");
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);
            read(connfd, buff, sizeof(buff));
            strncpy(firstName, buff, 11);
            bzero(buff, MAX);

            // get last name
            strcpy(buff, "Enter the student's last name (10 characters max): ");
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);
            read(connfd, buff, sizeof(buff));
            strncpy(lastName, buff, 11);
            bzero(buff, MAX);

            // Add student to data base
            addUser(firstName, lastName);

            // confirm student was added
            strcpy(buff, "Student added... ");
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);

            break;

        case 2:
            // get id number
            bzero(student, MAX);
            strcpy(buff, "Enter the student's ID whose profile you want to see(6 characters max): ");
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);
            read(connfd, buff, sizeof(buff));
            studentID = atoi(buff);
            bzero(buff, MAX);

            strncpy(student, displayID(studentID), MAX);
            studentID = 0;

            write(connfd, student, sizeof(student));

            break;

        case 3:
            // get score for comparison
            bzero(student, MAX);
            strcpy(buff, "Enter a benchmark score (0-100) to see who scored higher : ");
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);
            read(connfd, buff, sizeof(buff));
            studentScore = atoi(buff);
            bzero(buff, MAX);

            strncpy(student, displayScore(studentScore), MAX);
            
            write(connfd, student, sizeof(student));
            studentScore = 0;

            break;

        case 4:
            // displays all students
            strncpy(student, displayAll(), MAX);
            write(connfd, student, sizeof(student));
            bzero(buff, MAX);

            break;

        case 5:
            // deletes a user
            bzero(student, MAX);
            strcpy(buff, "Enter the student's ID whose profile you want to delete(6 characters max): ");
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);
            read(connfd, buff, sizeof(buff));
            studentID = atoi(buff);
            bzero(buff, MAX);

            deleteUser(studentID);

            strcpy(buff, "The student was deleted...\n ");
            write(connfd, buff, sizeof(buff));

            break;

        case 6:
            // exits program
            strcpy(buff, "Exiting...\n ");
            write(connfd, buff, sizeof(buff));
            return 0;
            break;

        case 0:
            // shows the menu again if needed
            menu(connfd);
            break;

        default:
            strcpy(buff, "Error: Not an option \n ");
            write(connfd, buff, sizeof(buff));
            break;

        }
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    srand(time(NULL));  /* seed for random number */

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else {
        printf("server accept the client...\n");
        menu(connfd);
    }
    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);

    return 0;
}
