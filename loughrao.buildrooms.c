#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
  // Initialize an Array of all the possible room names.
  const char *Arr[10];
  Arr[0] =  "Brain";
  Arr[1] =  "Blader";
  Arr[2] =  "Brocolli"; 
  Arr[3] =  "Spiders"; 
  Arr[4] =  "Bits"; 
  Arr[5] =  "Blast-Zone"; 
  Arr[6] =  "Bomb-Factory"; 
  Arr[7] =  "Bronchial-Tubes"; 
  Arr[8] =  "Bro-Room"; 
  Arr[9] =  "Broom-Room";

  // Create a temporary C String to be used throught the program
  char temp[100];
  memset(temp, '\0', 100);

  // Setup a string to hold the name of the directory to create the rooms in, then make the directory.
  char Dir[100];
  memset(Dir, '\0', 100); //Initialize Dir to be null characters at every element.
  sprintf(Dir, "loughrao.rooms.%d", getpid()); //Assign the current process ID to for the directory name
  mkdir(Dir,0777);  //Makes a Directory with the default permissions.

  // Choose three random numbers to be excluded, as we only need 7 of 10.
  srand(time(NULL));
  int x1 = (rand()%10);
  int x2 = (rand()%10);
  int x3 = (rand()%10);
  while (x1 == x2) {  //Checks to make sure x1 and x2 are not the same
    x2 = (rand()%9);
  }
  while ((x1 == x3) || (x2 == x3)) { //Checks to make sure x3 is not the same as x1 or x2
    x3 = (rand()%10);
  }

  // Initialize three variables for the creation of the room files.
  int i;
  FILE* rooms[7];
  int roomcount=0;

  // Create 7 rooms files, and fill in the first line with their room name.
  int linkforlater[7];
  for (i=0; i < 10; i++) {
    if (i != x1 && i != x2 && i != x3) {
      sprintf(temp, "./%s/%s", Dir, Arr[i]); // Create the name to be called for the current room file
      rooms[roomcount] = fopen(temp, "w");
      linkforlater[roomcount] = i;
      sprintf(temp, "ROOM NAME: %s\n", Arr[i]); // Put Room Name: and the name of the room into the first line of the file.
      fwrite(temp, 1, strlen(temp), rooms[roomcount]);
      roomcount++; // Used to select the correct element of rooms, as the for loop runs 10 times and we only use 7 rooms
    }
  }

  // Create connections for each room. Admitably not the best implementation, I would do this using structs next time, but it works and for the time being it will have to do.
  int currC = 0;
  int connections[7][7]; // This 2d array is created so that the first dimension is all the rooms, and the second dimensions points to each connection, the last element of the second
  int j;                 // dimension hold the count for how many connections.
  for (i = 0; i < 7; i++) { // Initialize the array to hold -1 in all locations
    for (j = 0; j < 6; j++) {
      connections[i][j] = -1;
    }
  }
  for (i = 0; i < 7; i++) {
    connections[i][6] = 0; // Set the last element of connections to 0, as this is the count.
  }
  for (i = 0; i < 7; i++) {    
    while (connections[i][6] < 3) {
      // Randomizes the connection until it is not equal to the room or a previous connection in the room.
      while (currC == i || currC == connections[i][0] || currC == connections[i][1] || currC == connections[i][2] || currC == connections[i][3] || currC == connections[i][4]) {
        currC = rand()%7;
      }
      // Sets the connection for room i as well as room currC effectively linking them.
      connections[i][connections[i][6]] = currC;
      connections[i][6]++;
      connections[currC][connections[currC][6]] = i; 
      connections[currC][6]++;
    }
  }

  // Put the connections into the files.
  for (i = 0; i < 7; i++) {
    for (j = 0; j < connections[i][6]; j++) {
      sprintf(temp, "CONNECTION %d: %s\n", j+1, Arr[linkforlater[connections[i][j]]]);
      fwrite(temp, 1, strlen(temp), rooms[i]);
    }
  }


  // Choose a random start and end point
  int start = (rand()%7);
  int end   = (rand()%7);
  while (start == end) {
    end = (rand()%7);
  }

  // Assign the types of rooms, and close our file streams
  for ( i = 0; i < 7; i++) {
    if (i == start) {
      fwrite("ROOM TYPE: START_ROOM", 1, strlen("ROOM TYPE: START_ROOM"), rooms[i]);
    } 
    else if (i == end) {
      fwrite("ROOM TYPE: END_ROOM", 1, strlen("ROOM TYPE: END_ROOM"), rooms[i]);
    } 
    else {
      fwrite("ROOM TYPE: MID_ROOM", 1, strlen("ROOM TYPE: MID_ROOM"), rooms[i]);
    }
    fclose(rooms[i]);
  }
  return 0;
}
