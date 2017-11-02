#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

pthread_t thread;
int c;
pthread_mutex_t lock;

// Struct used to store the name, number of connections, connections, and type of a room.
struct room {
  char name[100];
  int count;
  char connections[6][100];
  int type;
}; 

void* getTime() {
    pthread_mutex_lock(&lock);
    // Creates a file timestamps to write our current time to
    FILE* timestamps;
    timestamps = fopen("currentTime.txt","w");
    time_t t = time(NULL);                        // Initializes t with the raw time data at the moment
    struct tm *timeinfo;                        
    timeinfo = localtime(&t);                     // Extracrts the local time from the raw time data
    fprintf(timestamps,"%s", asctime(timeinfo));  // Prints the local time to the file
    fclose(timestamps);                           // Closes the file
    pthread_mutex_unlock(&lock);  
}

int main() {
  pthread_mutex_init(&lock, NULL);
  pthread_create(&(thread), NULL, &getTime, NULL);
  // Initizalizes various variables for use in finding the newest "loughrao.rooms." file
  int newTime = 0;
  char dir[] = "loughrao.rooms.";   // Target prefix for my directories 
  char newDir[100];                 // Holds our newest direcotires name.
  memset(newDir, '\0', 100);        // Initializes the variable newDir
  DIR* curr;                        // Variable of type DIR used for travesring current directories.
  struct dirent* check;             // Variable of type dirent, used to get info on specific "loughrao.rooms." directories
  struct stat checkInfo;            // Variable of type stat, used to store the info on check.
  curr = opendir(".");              // Opens the current dirrectory
  if (curr > 0) {
    while ((check = readdir(curr)) != NULL) {     // Runs through all of the files in the current directory
      if (strstr(check->d_name, dir) != NULL) {   // Only runs the following code if the file matches "loughrao.rooms."
        stat(check->d_name, &checkInfo);          // Gets the stats on the file being checked
        if((int)checkInfo.st_mtime > newTime) {   // Compares the time of the current file with the newest time save
          newTime = (int)checkInfo.st_mtime;      // Updates the time if it is newer
          memset(newDir, '\0', sizeof(newDir));
          strcpy(newDir, check->d_name);          // Updates the name
        }
      }
    }
  }
  closedir(curr);                                 // Closes the Stream

  struct room map[7];                             // Declares and array of 7 rooms
  int i;
  int j;
  // Initializes all the variables for the seven rooms.
  for (i = 0; i < 7; i++) {
    memset(map[i].name, '\0', 100);
    for (j = 0; j < 6; j++) {
      memset(map[i].connections[j], '\0', 100);
    }
    map[i].count = 0;
  }
  int currentRoom = -1;

  
  FILE* rooms;                                    // Creates a temp room file for use in file reading.
  curr = opendir(newDir);                         // Opens the newest "loughrao.rooms." dir
  char temp[100];                                 // Temp string to be used whenever needed
  memset(temp, '\0', 100);
  i=0;
  while ((check = readdir(curr)) != NULL) {       // While there are unread files in the dir "loughrao.rooms." do this
    if (!strcmp(check->d_name, ".")) {            // If the file is the current derictory of the previous directory ignore it.
       continue;
    }
    if (!strcmp(check->d_name, "..")) {
      continue;
    }
    sprintf(temp, "./%s/%s", newDir, check->d_name); // Save the current file name to the temp string
    rooms = fopen(temp, "r");                        // Open the file for reading
    fscanf(rooms, "%*s %*s %s", map[i].name);        // Read the name from the file, skipping "ROOM" and "NAME"
    fscanf(rooms, "%s", temp);                       // Read the next string into the temp string
    while(strcmp(temp,"CONNECTION") == 0) {          // Runs the following code as long as the string read in is CONNECTION
      fscanf(rooms,"%*s %s", map[i].connections[map[i].count]);   // Read the connections into the first available conection in the current rooom struct
      map[i].count++;                                // Iterate room count
      fscanf(rooms,"%s", temp);                      // Read the next string, checking if it is connection
    }
    fscanf(rooms, "%*s %s", temp);                   // Once the string is not equal to connection read the type into a temp variable
    // Check the types read in, and set the appropriate value in the room struct
    if (strcmp(temp,"MID_ROOM")==0) {
      map[i].type = 1;
    }
    else if (strcmp(temp,"END_ROOM")==0) {
      map[i].type = 2;
    }
    else {
      currentRoom = i;
      map[i].type = 0;
    }
    fclose(rooms);
    i++; // Iterate to the next room struct
  }
  closedir(curr);

  // Prepare various variables to store the input, path to vectory, steps to victory, as well as a flag to tell the game it has ended.
  int path[100];
  int steps=0;
  int stop = 0;
  char input[100];
  memset(input, '\0', 100);
  while (stop == 0) {                                               // Run as long as the stop flag has not been triggered
    printf("\nCURRENT LOCATION: %s\n", map[currentRoom].name);      // Print out the name of the current room
    printf("POSSIBLE CONNECTIONS: ");
    for (i = 0; i < map[currentRoom].count; i++) {                  // Print out all the connection for the current room
      printf("%s", map[currentRoom].connections[i]);
      if (i < map[currentRoom].count-1) {
        printf(", ");   
      }
    }
    // Promt the user to input the room they wish to go to
    printf(".\nWHERE TO? >");
    scanf("%s",input);
    
    int found = 0;                                                  // Found is used to tell the program whether or not the string inputed was found
    for (i = 0; i < map[currentRoom].count; i++) {
      if (strcmp(input,map[currentRoom].connections[i]) == 0) {     // Compare the input with each connection, flag found as true if it matches
        for (j = 0; j < 7; j++) {
          if (strcmp(input,map[j].name) == 0) {
            currentRoom = j;
            // Add the room to the path and increment steps
            path[steps] = j;
            steps++;
          }
        }
        found = 1;
      }
      if (strcmp(input,"time") == 0) {
        pthread_mutex_unlock(&lock);
        pthread_join(thread, NULL);
        pthread_mutex_lock(&lock);
        FILE* file = fopen("currentTime.txt","r");
        memset(temp,'\0',100);
        fread(temp,1,100,file);
        printf("\n  %s\n",temp);
        found=1;
        break;
      }

    }
    // Print out an error if the input is invalid
    if (found == 0){
      printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
    }
    if (map[currentRoom].type == 2) {                               // Check if the current room is the end room, if it is trigger the stop flag
      stop = 1;
    }
  }

  // Print out the victory message, the steps to victory, and the path to victory
  printf("\nYOU HAVE FOUND THE END ROOM. CONGRADULATIONS!\n");
  printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
  for (i = 0; i < steps; i++) {
    printf("%s\n",map[path[i]].name);
  }
  pthread_mutex_destroy(&lock);
  return 0;
}
