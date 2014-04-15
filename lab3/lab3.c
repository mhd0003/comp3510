/*****************************************************************************\
* Laboratory Exercises COMP 3510                                              *
* Author: Saad Biaz                                                           *
* Date  : March 5, 2013                                                   *
\*****************************************************************************/

/*****************************************************************************\
*                             Global system headers                           *
\*****************************************************************************/


#include "common.h"

/*****************************************************************************\
*                             Global data types                               *
\*****************************************************************************/



/*****************************************************************************\
*                             Global definitions                              *
\*****************************************************************************/
#define MAX_EVENT_ID 100

#define QUEUE_SIZE 2



/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/

typedef struct queueNode {
    Event *event;
    struct queueNode *next;
} QueueNode;

// Keeps track of which events to process next
// Allocates all memory upfront -- never deletes "served" events
// Queue is empty when (head > tail)
typedef struct queue {
  //int head,tail,size;
  int size;
  //Event Events[QUEUE_SIZE];
  QueueNode *events;
} Queue;


//to keep track of events, response, and turnaround for each device.
typedef struct DeviceTag {
    Timestamp responseTotal;
    Timestamp turnaroundTotal;
    int eventsProcessed;
    int responses;
    int turnarounds;
	Queue eventQueue;
} Device;

/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/
Device devices[MAX_NUMBER_DEVICES];

/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/

void Control(void);
void InterruptRoutineHandlerDevice(void);
void BookKeeping();
Event* enqueue(Event* event, int deviceNum);
Event* dequeue(int deviceNum);
int isFull(int deviceNum);
int isEmpty(int deviceNum);

/*****************************************************************************\
* function: main()                                                            *
* usage:    Create an artificial environment for embedded systems. The parent *
*           process is the "control" process while children process will gene-*
*           generate events on devices                                        *
*******************************************************************************
* Inputs: ANSI flat C command line parameters                                 *
* Output: None                                                                *
*                                                                             *
* INITIALIZE PROGRAM ENVIRONMENT                                              *
* START CONTROL ROUTINE                                                       *
\*****************************************************************************/

int main (int argc, char **argv) {

   if (Initialization(argc,argv)){
     Control();
   }
} /* end of main function */

/***********************************************************************\
 * Input : none                                                          *
 * Output: None                                                          *
 * Function: Monitor Devices and process events (written by students)    *
 \***********************************************************************/
void Control(void){
  int deviceNum;
  int i = 0;
  //boolean EP = false;
  Event* event;
  // init the global devices
  for(i; i < Number_Devices; i++)
  {
//  		devices[i].eventQueue.head = 0;
//		devices[i].eventQueue.tail = 1;
		devices[i].eventQueue.size = 0;
        devices[i].eventQueue.events = (QueueNode *) malloc( sizeof(QueueNode));
        devices[i].eventQueue.events->next = 0;
        devices[i].eventQueue.events->event = NULL;
  }

  // Get next event in queue, if any, then process it
  int loop = 0;
  while (1)
  {
        loop++;
        //if((loop % 1000000) == 0)
          //  printf("Main loop\n");
  		deviceNum = 0;
        while(deviceNum < Number_Devices)
		{
            event = dequeue(deviceNum);
            if(event != 0)
			{

		      printf("Servicing event %d on device %d\n", event->EventID, deviceNum);
		      Server(event);
              sleep(1);
		      devices[deviceNum].turnaroundTotal += Now() - event->When;
		      devices[deviceNum].turnarounds++;
		      devices[deviceNum].eventsProcessed++;
			}
			else 
			{
				deviceNum++;
			}
		}
	}
}

/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: This routine is run whenever an event occurs on a device    *
*           The id of the device is encoded in the variable flag        *
\***********************************************************************/
void InterruptRoutineHandlerDevice(void){
    printf("An event occured at %f  Flags = %d \n", Now(), Flags);
    Event* event;
    Status tempFlags = Flags;
    int deviceNum = 0;

    Flags = 0;

    // Grab all events in sequential order
    //printf("Operating on flags\n");
    while(tempFlags)
    {
        if(tempFlags & 1)
        {
            // Copy event from volatile memory and make it get in line
            //printf("Found an event\n");
            event = &BufferLastEvent[deviceNum];

            if(!isFull(deviceNum)) {
              enqueue(event, deviceNum);
            }
            devices[deviceNum].responseTotal += Now() - event->When;
            devices[deviceNum].responses++;
            DisplayEvent('c', event);
        }

        tempFlags = tempFlags >> 1;
        deviceNum++;
    }

}


/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: This must print out the number of Events buffered not yet   *
*           not yet processed (Server() function not yet called)        *
\***********************************************************************/
void BookKeeping(void){
  // For EACH device, print out the following metrics :
  // 1) the percentage of missed events, 2) the average response time, and
  // 3) the average turnaround time.
  // Print the overall averages of the three metrics 1-3 above
  int n = 0;
  Timestamp avgResponse;
  Timestamp avgTurnaround;
  int deviceMissed = 0;
  int totalMissed = 0;
  float percentMissed = 0.0;
  float avgPercentMissed = 0.0;

  printf("Doing bookkeeping");
  while(n < Number_Devices)
  {
	devices[n].responseTotal = devices[n].responseTotal / (double) devices[n].responses;
	devices[n].turnaroundTotal = devices[n].turnaroundTotal / (double) devices[n].turnarounds++;
  	avgResponse += devices[n].responseTotal;
	avgTurnaround += devices[n].turnaroundTotal;

    deviceMissed = (100 - devices[n].responses);
	percentMissed = deviceMissed / 100.0;
	avgPercentMissed += percentMissed;
    totalMissed += deviceMissed;

	printf("\nDevice %d:\nAvg Response: %f\nAvg Turnaround: %f\nPercent Missed: %f\nMissed: %d\n",
        n, devices[n].responseTotal, devices[n].turnaroundTotal, percentMissed, deviceMissed);
	n++;
  }

  avgResponse = avgResponse / (double) Number_Devices;
  avgTurnaround = avgTurnaround / (double) Number_Devices;
  avgPercentMissed = avgPercentMissed / (double) Number_Devices;

  printf("\nAverages over all devices:\nAvg Response: %f\nAvg Turnaround: %f\nAvg Percent Missed: %f\nTotal missed: %d\n",
  avgResponse, avgTurnaround, avgPercentMissed, totalMissed);

  fflush(stdout);
}
//These QUEUE functions need to be redone inorder to make my current solution work.

/***********************************************************************\
* Input : none                         											*
* Output: Boolean value							                              *
* Function: Returns a boolean value depending on if the queue is full   *
\***********************************************************************/
int isFull(int deviceNum) {
	return devices[deviceNum].eventQueue.size == QUEUE_SIZE;
}

/***********************************************************************\
* Input : none                         											*
* Output: Boolean value							                              *
* Function: Returns a boolean value depending on if the queue is empty  *
\***********************************************************************/
int isEmpty(int deviceNum) {
	return devices[deviceNum].eventQueue.size == 0;
}


/***********************************************************************\
* Input : event in volatile memory to copy from                         *
* Output: pointer to event in local memory                              *
* Function: Copies an event from BufferLastEvent and saved so we can    *
*           call Server() on it later.                                  *
\***********************************************************************/
Event* enqueue(Event* event, int deviceNum) {
  Queue* eventOut;
  QueueNode *iterator;

  //printf("Trying to enqueue\n");
  if(devices[deviceNum].eventQueue.size < QUEUE_SIZE) {
      iterator = devices[deviceNum].eventQueue.events;

      //Find the end of the list
      if(iterator != 0) {
        while(iterator->next != 0) {
            iterator = iterator->next;
        }
        
      }
      //printf("Found the head\n");
      //Allocate memory for the next pointer
      iterator->next = malloc(sizeof(QueueNode));
      //printf("Copying to memory\n");
      iterator->event = event;
      
      //Copy event to end of list

//      memcpy(iterator->event, event, sizeof(*event));
      //printf("Copied to memory\n");
  }

  return iterator->event;
}

/***********************************************************************\
* Input : none                                                          *
* Output: pointer to next event in queue                                *
* Function: Returns pointer to next event we should process (FIFO)      *
*           Returns NULL if all enqueued events have already been       *
*           dequeued.                                                   *
\***********************************************************************/
Event* dequeue(int deviceNum) {
  //printf("In dequeue\n");
  Event* event = NULL;
  QueueNode *iterator;
  iterator = devices[deviceNum].eventQueue.events;
  //printf("Got iterator\n");
  if(iterator != 0) {
    //printf("Copying event\n");
    if(iterator->event != NULL) {
        printf("Got an event\n");
        event = iterator->event;
        //iterator->event = event;
 //       memcpy(event, iterator->event, sizeof(*event));
        //printf("Event copied\n");
        iterator->event = NULL;
        iterator->next = iterator->next->next;
        devices[deviceNum].eventQueue.size -= 1;
    }
  }
  //printf("Exited dequeue if\n");

  return event;
}
