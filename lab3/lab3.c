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
void totalDeviceStatistics(int deviceNum);
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
		devices[i].eventQueue.size = 0;
        devices[i].eventQueue.events = (QueueNode *) malloc( sizeof(QueueNode));
        devices[i].eventQueue.events->next = 0;
        devices[i].eventQueue.events->event = NULL;
  }

  while (1)
  {     
        
  		deviceNum = 0;
        while(deviceNum < Number_Devices)
		{
            // Get next event in queue, if any, then process it
            event = dequeue(deviceNum);

            
            if(event != NULL)
			{
                
		      printf("Servicing event %d on device %d\n", event->EventID, deviceNum);
		      Server(event);
		      devices[deviceNum].turnaroundTotal += Now() - event->When;
		      devices[deviceNum].turnarounds++;
		      devices[deviceNum].eventsProcessed++;
              //Reset priority to give the highest priority another chance to go
              deviceNum = 0;
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
    while(tempFlags)
    {
        if(tempFlags & 1)
        {
            // Copy event from volatile memory and make it get in line
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
  // For the highest and lowest priority devices print the following statistics:
  // 1) the percentage of missed events, 2) the average response time, and
  // 3) the average turnaround time.

  printf("Doing bookkeeping");
  totalDeviceStatistics(0);
  totalDeviceStatistics(Number_Devices - 1);

  fflush(stdout);
}

void totalDeviceStatistics(int deviceNum) {
  Timestamp avgResponse;
  Timestamp avgTurnaround;
  int deviceMissed = 0;
  float percentMissed = 0.0;
  //float avgPercentMissed = 0.0;
  Device device = devices[deviceNum];

  device.responseTotal = device.responseTotal / (double) device.responses;
  device.turnaroundTotal = device.turnaroundTotal / (double) device.turnarounds;
  avgResponse += device.responseTotal;
  avgTurnaround += device.turnaroundTotal;

  deviceMissed = (100 - device.eventsProcessed);
  percentMissed = deviceMissed / 100.0;
  //avgPercentMissed += percentMissed;

  printf("\nDevice %d:\nAvg Response: %f\nAvg Turnaround: %f\nPercent Missed: %f\nMissed: %d\n",
      deviceNum, device.responseTotal, device.turnaroundTotal, percentMissed, deviceMissed);

}

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
  Event* eventOut;
  //eventOut = event;
  QueueNode *iterator;

  iterator = devices[deviceNum].eventQueue.events;

  //Find the end of the list
  if(iterator != 0) {
    while(iterator->next != 0) {
        iterator = iterator->next;
    }
    
  }
  //Allocate memory for the next pointer

  //devices[i].eventQueue.events = (QueueNode *) malloc( sizeof(QueueNode));
  iterator->next = (QueueNode *) malloc(sizeof(QueueNode));
  
  //Copy event to end of list
  iterator->event = event;
  devices[deviceNum].eventQueue.events->next = iterator->next;
  devices[deviceNum].eventQueue.events->event = iterator->event;
  devices[deviceNum].eventQueue.size += 1;
      
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
  Event* event = NULL;
  QueueNode *iterator;
  iterator = devices[deviceNum].eventQueue.events;
  if(iterator != 0) {
    if(iterator->event != NULL) {
        event = iterator->event;
        iterator->event = NULL;
        iterator->next = iterator->next->next;
        devices[deviceNum].eventQueue.size -= 1;
        devices[deviceNum].eventQueue.events->next = iterator->next;
    }
  }

  return event;
}
