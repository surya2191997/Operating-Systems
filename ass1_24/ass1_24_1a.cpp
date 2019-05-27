#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#define SIZE 100
using namespace std;

//Sorting algorithm

void sortArray(int a[SIZE]) {
    for(int i = 0; i < SIZE; i++) {
        int minNo = a[i];
        int ind = i, temp;
        for(int j = i; j < SIZE; j++) {
            if(a[j] < minNo) {
                ind = j;
                minNo = a[j];
            }
        }
        temp = a[i];
        a[i] = a[ind];
        a[ind] = temp;
    }
}

int main() {
    pid_t pid;
    int p1[2], p2[2], p3[2];

    pipe(p1);
    pipe(p2);
    pipe(p3);

    //Forking the current process into parent and child processes
    pid = fork();

    if(!pid) {
        //Forking again
        pid_t pidParent = fork();

        if(!pidParent) {
            //Process A
            int randInt[SIZE];
            srand(time(NULL) + 1);
            
            //Generating a random array of 100 integers
            for(int i = 0; i < SIZE; i++)
                randInt[i] = abs(rand()) % 1000;
            /*
            //Printing out the elements of the array
            cout << "Elements of Array in process A\n";
            for(int i = 0; i < SIZE; i++) 
                cout << randInt[i] << " ";
            cout << "\n";
            */

            //Sorting the array
            sortArray(randInt);

            //Passing the elements of the array to the D process via p1 pipe
            for(int i = 0; i < SIZE; i++) {
                int *p = new int;
                *p = randInt[i];
                write(p1[1], p, 4);
            }
            wait(NULL);
        }
        else {
            //Process B
            int randInt[SIZE];
            srand(time(NULL) + 2);

            //Generating a random array of 100 integers
            for(int i = 0; i < SIZE; i++)
                randInt[i] = abs(rand()) % 1000;

            /*
            //Printing out the elements of the array
            cout << "Elements of Array in process B\n";
            for(int i = 0; i < SIZE; i++) 
                cout << randInt[i] << " ";
            cout << "\n";
            */

            //Sorting the array
            sortArray(randInt);

            //Passing the elements of the array to the D process via p2 pipe
            for(int i = 0; i < SIZE; i++) {
                int *p = new int;
                *p = randInt[i];
                write(p2[1], p, 4);
            }
            wait(NULL);
        }
    }
    else {
        //Forking again
        pid_t pidParent = fork();
        
        if(!pidParent) {
            //Process C
            int randInt[SIZE];
            srand(time(NULL) + 3);

            //Generating a random array of 100 integers
            for(int i = 0; i < SIZE; i++)
                randInt[i] = abs(rand()) % 1000;

            /*
            //Printing out the elements of the array
            cout << "Elements of Array in process C\n";
            for(int i = 0; i < SIZE; i++) 
                cout << randInt[i] << " ";
            cout << "\n";
            */
            
            //Sorting the array
            sortArray(randInt);

            //Passing the elements of the array to the D process via p2 pipe
            for(int i = 0; i < SIZE; i++) {
                int *p = new int;
                *p = randInt[i];
                write(p3[1], p, 4);
            }
            wait(NULL);
        }
        else {
            int buff[300];
            int *x1, *x2, *x3;
            int i = 0, j = 0, k = 0;
            int n = 0;

            x1 = new int;
            x2 = new int;
            x3 = new int;

            //reading the elements of each array
            read(p1[0], x1, 4);
            read(p2[0], x2, 4);
            read(p3[0], x3, 4);

            //reading and merging the three arrays
            while(n < 3 * SIZE) {
                while(i < SIZE && j < SIZE && k < SIZE) {
                    buff[n] = min(*x1, min(*x2, *x3));
                    if(buff[n] == *x1) {
                        if(i < SIZE - 1)
                        read(p1[0], x1, 4);
                        i++;
                    }
                    else if(buff[n] == *x2) {
                        if(j < SIZE - 1)
                        read(p2[0], x2, 4);
                        j++;
                    }
                    else if(buff[n] == *x3) {
                        if(k < SIZE - 1)
                        read(p3[0], x3, 4);
                        k++;
                    }
                    n++;
                }
                while(i < SIZE && j < SIZE) {
                    buff[n] = min(*x1, *x2);
                    if(buff[n] == *x1) {
                        if(i < SIZE - 1)
                        read(p1[0], x1, 4);
                        i++;
                    }
                    else if(buff[n] == *x2) {
                        if(j < SIZE - 1)
                        read(p2[0], x2, 4);
                        j++;
                    }
                    n++;
                }
                while(i < SIZE && k < SIZE) {
                    buff[n] = min(*x1, *x3);
                    if(buff[n] == *x1) {
                        if(i < SIZE - 1)
                        read(p1[0], x1, 4);
                        i++;
                    }
                    else if(buff[n] == *x3) {
                        if(k < SIZE - 1)
                        read(p3[0], x3, 4);
                        k++;
                    }
                    n++;
                }
                while(k < SIZE && j < SIZE) {
                    buff[n] = min(*x3, *x2);
                    if(buff[n] == *x3) {
                        if(k < SIZE - 1)
                        read(p3[0], x3, 4);
                        k++;
                    }
                    else if(buff[n] == *x2) {
                        if(j < SIZE - 1)
                        read(p2[0], x2, 4);
                        j++;
                    }
                    n++;
                }
                while(i < SIZE) {
                    buff[n] = *x1;
                    if(i < SIZE - 1)
                    read(p1[0], x1, 4);
                    i++;
                    n++;
                }
                while(j < SIZE) {
                    buff[n] = *x2;
                    if(j < SIZE - 1)
                    read(p2[0], x2, 4);
                    j++;
                    n++;
                }
                while(k < SIZE) {
                    buff[n] = *x3;
                    if(k < SIZE - 1)
                    read(p3[0], x3, 4);
                    k++;
                    n++;
                }
            }

            //Printing out the sorted array
            cout << "Sorted Array is : \n";
            for(int i = 0; i < 3 * SIZE; i++)
                cout << buff[i] << " ";
            cout << "\n";

            //freeing out dynamically allotted integers
            free(x1);
            free(x2);
            free(x3);
            exit(0);
        }
    }
    return 0;
}
