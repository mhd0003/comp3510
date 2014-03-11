
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
typedef struct node {
  struct node* next;
  Event* event;
} Node;

typedef struct queue {
  Node* head;
  Node* tail;
} Queue;

Queue* enqueue(Queue* queue, Event* event) {
  // memory problem?
  Node n;
  n.next = NULL;
  n.event = event;

  // shouldn't happen
  if (queue == NULL) {
    return NULL;
  }

  // queue is empty (head == NULL) too
  if (queue->tail == NULL) {
    queue->head = &n;
    queue->tail = &n;
    return queue;
  }

  queue->tail->next = &n;
  queue->tail = &n;
  return queue;
}

Event* dequeue(Queue* queue) {
  Node* n;

  // shouldn't happen
  if (queue == NULL) {
    return NULL;
  }

  n = queue->head;
  // queue is empty (head == NULL)
  if (n == NULL) {
    return NULL;
  }

  queue->head = queue->head->next;

  // queue has been cleared
  if (queue->head == NULL) {
    queue->tail == NULL;
  }

  return n->event;
}


/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/




/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/
Queue allEvents;



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
  allEvents.head = NULL;
  allEvents.tail = NULL;

  while (1) {
    event = dequeue(&allEvents);
    if (event != NULL) {
      Server(event);
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
  Event newEvent;
  temp = Flags;
  deviceNum = 1;
  while ((Flags & temp) != 0) {
    temp <<= 1;
    deviceNum += 1;
  }
  Flags &= Flags&(~temp);
  printf("It was device %d  Flags = %d \n", deviceNum, Flags);
  memcpy(&newEvent, &BufferLastEvent[deviceNum], sizeof(newEvent));
  // Put in queue
  enqueue(&allEvents, &newEvent);
  DisplayEvent('a', &newEvent);
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
}






