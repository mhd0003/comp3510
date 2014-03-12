
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
#define MAXEVENTID 100

Event allEvents[MAXEVENTID*MAX_NUMBER_DEVICES];
typedef int Index;

// init head = 0, tail = -1;
typedef struct queue {
  Index head,tail;
} Queue;

Event* enqueue(Queue* queue, Event* event) {
  // shouldn't happen
  if (queue == NULL) {
    return NULL;
  }
  // printf("\n\n enqueue -- head: %d, tail: %d, eID: %d, dID: %d\n",
  // queue->head, queue->tail, event->EventID, event->DeviceID);

  queue->tail += 1;
  memcpy(&allEvents[queue->tail], event, sizeof(*event));

  // printf(" enqueue -- head: %d, tail: %d, Now: %d, eWhen: %d\n\n",
  // queue->head, queue->tail, Now(), allEvents[queue->tail].When);
  return &allEvents[queue->tail];
}

Event* dequeue(Queue* queue) {
  Event* event;

  // shouldn't happen
  if (queue == NULL) {
    return NULL;
  }

  // queue is empty -- if (head == tail) then last event in queue
  if (queue->head > queue->tail) {
    return NULL;
  }

  event = &allEvents[queue->head];
  // printf(" dequeue -- head: %d, tail: %d, eID: %d, dID: %d\n\n",
  // queue->head, queue->tail, event->EventID, event->DeviceID);
  queue->head += 1;
  return event;
}


/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/




/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/
Queue eventQueue;


/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/

void Control(void);
void InterruptRoutineHandlerDevice(void);
void BookKeeping();


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
  Event* event;
  // init the global queue
  eventQueue.head = 0;
  eventQueue.tail = -1;

  while (1) {
    event = dequeue(&eventQueue);
    if (event != NULL) {
      printf("**********Serving eID: %d for dID: %d, Now(): %f, eWhen: %f\n",
             event->EventID, event->DeviceID, Now(), event->When);

      Server(event);

      printf("**********Finish eID: %d for dID: %d, Now(): %f, eWhen: %f\n",
             event->EventID, event->DeviceID, Now(), event->When);
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
	// Put Here the most urgent steps that cannot wait
  int deviceNum, temp;
  Event* newEvent;

  temp = 1;
  deviceNum = 0;
  while ((Flags & temp) == 0) {
    temp <<= 1;
    deviceNum += 1;
  }

  Flags = Flags&(~temp);
  // printf("It was device %d  Flags = %d \n", deviceNum, Flags);

  // Put in queue
  newEvent = enqueue(&eventQueue, &BufferLastEvent[deviceNum]);

  DisplayEvent('a', newEvent);
  return;
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
  fflush(stdout);
}






