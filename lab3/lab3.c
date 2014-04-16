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

//#define QUEUE_SIZE 2
#define QUEUE_SIZE 8



/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/

typedef struct queueNode {
    Event event;
    struct queueNode *next;
} QueueNode;

// Keeps track of which events to process next
// Allocates all memory upfront -- never deletes "served" events
// Queue is empty when (head > tail)
typedef struct queue {
  //int head,tail,size;
  int size;
  //Event Events[QUEUE_SIZE];
  //QueueNode *events;
  QueueNode *head;
  QueueNode *tail;
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
void totalDeviceStatistics(int deviceNum);
Event* enqueue(Event* event, int deviceNum);
//Event* dequeue(int deviceNum);
void dequeue(int deviceNum);
Event* getFirst(int deviceNum);
int isFull(int deviceNum);

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

  Event* event = NULL;
  // init the global devices
  for(i = 0; i < Number_Devices; i++)
  {
		devices[i].eventQueue.size = 0;
        devices[i].eventQueue.head = NULL;
        devices[i].eventQueue.tail = NULL;

        devices[i].turnaroundTotal = 0;
        devices[i].turnarounds = 0;
        devices[i].responses = 0;
        devices[i].eventsProcessed = 0;
        devices[i].responseTotal = 0;
  }

  while (1)
  {     
        
  		deviceNum = 0;
        while(deviceNum < Number_Devices)
		{
            // Get next event in queue, if any, then process it
            event = getFirst(deviceNum);
            
            if(event != NULL && event != 0)
			{
   
		      printf("Servicing event %d on device %d\n", event->EventID, deviceNum);
		      Server(event);

		      devices[deviceNum].turnaroundTotal += Now() - event->When;
              dequeue(deviceNum);
              devices[deviceNum].turnarounds++;
		      devices[deviceNum].eventsProcessed++;

              //Reset priority to give the highest priority another chance to go
              deviceNum = 0;
			}

            deviceNum++;
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
    Event* event = NULL;
    Status tempFlags = Flags;
    int deviceNum = 0;

    Flags = 0;

    // Grab all events in sequential order
    while(tempFlags)
    {
        if(tempFlags & 1)
        {
            // Copy event from volatile memory and make it get in line
            event = &BufferLastEvent[deviceNum];

            if(!isFull(deviceNum)) {
              event = enqueue(event, deviceNum);
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
  // For the highest and lowest priority devices print the following statistics:
  // 1) the percentage of missed events, 2) the average response time, and
  // 3) the average turnaround time.

  printf("Doing bookkeeping");
  totalDeviceStatistics(0);
  totalDeviceStatistics(Number_Devices - 1);

  fflush(stdout);
}

void totalDeviceStatistics(int deviceNum) {
  int deviceMissed;
  double percentMissed;
  Timestamp averageTurnaround;
  Timestamp averageResponse;
  Device device;

  device = devices[deviceNum];
  averageResponse = devices[deviceNum].responseTotal / (double) devices[deviceNum].responses;
  averageTurnaround = devices[deviceNum].turnaroundTotal / (double) devices[deviceNum].turnarounds;

  deviceMissed = (100 - device.eventsProcessed);
  percentMissed = deviceMissed / 100.0;

  printf("\nDevice %d:\nAvg Response: %f\nAvg Turnaround: %f\nPercent Missed: %f\nMissed: %d\n",
          deviceNum, averageResponse, averageTurnaround, percentMissed, deviceMissed);
  //printf("Turnarounds: %d\nResponses: %d\nEvents Processed: %d\n", devices[deviceNum].turnarounds, devices[deviceNum].responses,
    //      devices[deviceNum].eventsProcessed);

}

/***********************************************************************\
* Input : none                         									*
* Output: Boolean value							                        *
* Function: Returns a boolean value depending on if the queue is full   *
\***********************************************************************/
int isFull(int deviceNum) {
	return devices[deviceNum].eventQueue.size == QUEUE_SIZE;
}

/***********************************************************************\
* Input : event in volatile memory to copy from                         *
* Output: pointer to event in local memory                              *
* Function: Copies an event from BufferLastEvent and saved so we can    *
*           call Server() on it later.                                  *
\***********************************************************************/
Event* enqueue(Event* event, int deviceNum) {
  QueueNode *temp = (QueueNode *) malloc(sizeof(QueueNode));
  memcpy(&(temp->event), event, sizeof(*event));
  temp->next = NULL;

  if(devices[deviceNum].eventQueue.head == NULL && devices[deviceNum].eventQueue.tail == NULL) {
    devices[deviceNum].eventQueue.head = devices[deviceNum].eventQueue.tail = temp;
  } else {
    devices[deviceNum].eventQueue.tail->next = temp;
    devices[deviceNum].eventQueue.tail = temp;
  }

  devices[deviceNum].eventQueue.size++;

  return &(temp->event);
}

/***********************************************************************\
* Input : none                                                          *
* Output: pointer to next event in queue                                *
* Function: Returns pointer to next event we should process (FIFO)      *
*           Returns NULL if all enqueued events have already been       *
*           dequeued.                                                   *
\***********************************************************************/
void dequeue(int deviceNum) {
  //Event* event = NULL;
  QueueNode *temp;
  temp = devices[deviceNum].eventQueue.head;
  if(devices[deviceNum].eventQueue.head == NULL || temp == 0) {
   return;
  } 
  

  if (devices[deviceNum].eventQueue.head == devices[deviceNum].eventQueue.tail) {
    devices[deviceNum].eventQueue.head = devices[deviceNum].eventQueue.tail = NULL;
  } else {
    devices[deviceNum].eventQueue.head = devices[deviceNum].eventQueue.head->next;
  }

  devices[deviceNum].eventQueue.size--;

  free(temp);
}

Event* getFirst(int deviceNum) {
    Event* event = NULL;
    if (devices[deviceNum].eventQueue.head != NULL) {
        event = (Event*) &(devices[deviceNum].eventQueue.head->event);
    }

    return event;
}
