
/*****************************************************************************\
* Laboratory Exercises COMP 3510                                              *
* Author: Saad Biaz                                                           *
* Date  : January 18, 2014                                                    *
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

#define MAX_EVENTS_PER_DEVICE 100


/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/

typedef struct deviceEventsInfo {
    double responseTimes[MAX_EVENTS_PER_DEVICE];
    double turnAroundTimes[MAX_EVENTS_PER_DEVICE];
    double processedEventID[MAX_EVENTS_PER_DEVICE];
    double averageResponseTime;
    double averageTurnAroundTime;
    int missedEvents;
    int seenEvents;
} DeviceEventsInfo;


/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/

DeviceEventsInfo eventInfoArray[MAX_NUMBER_DEVICES];

/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/

void Control(void);
void computeAverages(int deviceId);
void computeAllAverages(void);
void InitializeDeviceEventsInfo(void);
void fillZeros(double *array);
void setEventInfo(Identifier deviceId, Identifier eventId, 
                  double responseTime, double turnAroundTime);
void printFinalAverages();

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
* WHILE JOBS STILL RUNNING                                                    *
*    CREATE PROCESSES                                                         *
*    RUN LONG TERM SCHEDULER                                                  *
*    RUN SHORT TERM SCHEDULER                                                 *
*    DISPATCH NEXT PROCESS                                                    *
\*****************************************************************************/


int main (int argc, char **argv) {
   if (Initialization(argc,argv)){
     Control();
   } 
} /* end of main function */

/***********************************************************************\
 * Input : none                                                          *
 * Output: None                                                          *
 * Function: Monitor Devices and process events                          *
 \***********************************************************************/
void Control(void) {
  int deviceID = 0;
  double eventStartTime, eventResponseTime, eventTurnAroundTime;
  Event e;
  //Set up DeviceEventsInfo structs
  // So we end up getting the same performance
  // even if Initialize() isn't called..
  InitializeDeviceEventsInfo();

  while (1) {
    // Find an device that has an event that needs processing
    while ((BufferLastEvent[deviceID].When == 0) || (eventInfoArray[deviceID].processedEventID[BufferLastEvent[deviceID].EventID])) {
      deviceID++;
      deviceID = deviceID % MAX_NUMBER_DEVICES;
    }

    //eventStartTime = Now(); // avg response = 0.000039, avg turn = 0.030
    // Copy the event into our memory before it changes
    e = BufferLastEvent[deviceID];
    eventStartTime = e.When; // avg response = 0.007799, avg turn = 0.037910

    DisplayEvent('a', &e);
    eventResponseTime = Now() - eventStartTime;

    // Serve the event
    Server(&e);
    eventTurnAroundTime = Now() - eventStartTime;

    // Set as processed
    eventInfoArray[e.DeviceID].processedEventID[e.EventID] = 1;
    setEventInfo(e.DeviceID, e.EventID, 
                 eventResponseTime, eventTurnAroundTime);
  }

}

/**********************************************************************\
 * Input:  None                                                         *
 * Output: None                                                         *
 * Function: Initialize all DeviceEventInfo structs within global array *
 \**********************************************************************/
void InitializeDeviceEventsInfo()
{
    int i = 0;
    for(i; i < MAX_NUMBER_DEVICES; i++)
    {
       DeviceEventsInfo eventInfo;
       eventInfo.responseTimes[MAX_EVENTS_PER_DEVICE];
       eventInfo.turnAroundTimes[MAX_EVENTS_PER_DEVICE];
       eventInfo.averageTurnAroundTime = 0;
       eventInfo.averageResponseTime = 0;
       eventInfo.missedEvents = 0;
       eventInfo.seenEvents = 0;

       fillZeros(eventInfo.responseTimes);
       fillZeros(eventInfo.turnAroundTimes);
       fillZeros(eventInfo.processedEventID);

       eventInfoArray[i] = eventInfo;
    }
}

void fillZeros(double *array)
{     
    int i = 0;
    for(i; i < MAX_EVENTS_PER_DEVICE; i++)
    {
       array[i] = 0;
    }
}

/***********************************************************************\
 * Input : None                                                          *
 * Output: None                                                          *
 * Function: Computes all averages of DeviceEventsInfo structs           *
 \***********************************************************************/
void computeAllAverages()
{
    int deviceId = 0;
    for(deviceId; deviceId < 1; deviceId++) {
        computeAverages(deviceId);
    }
}

/***********************************************************************\
 * Input : DeviceEventsInfo struct                                       *
 * Output: None                                                          *
 * Function: Computes average turn around time and response time for     *
 *            eventInfo. Assumed to be called at the end of Control.     *
 \***********************************************************************/;
void computeAverages(int deviceId)
{
    int i = 0;
    double responseSum = 0;
    double turnAroundTimeSum = 0;
    int number_of_misses = 0;
    DeviceEventsInfo eventInfo = eventInfoArray[deviceId];
    for(i; i < MAX_EVENTS_PER_DEVICE; i++) {          
       responseSum += eventInfo.responseTimes[i];
       turnAroundTimeSum += eventInfo.turnAroundTimes[i];

       if(eventInfo.responseTimes[i] == 0) {
           number_of_misses += 1;
       }
    }
   
    //int seenEvents = 100 - number_of_misses;
    int totalEvents = eventInfo.seenEvents;
    eventInfo.averageResponseTime = responseSum / (double)totalEvents;
    eventInfo.averageTurnAroundTime = turnAroundTimeSum / (double)totalEvents;
    eventInfoArray[deviceId] = eventInfo;
}

/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: Prints final averages of events                             *
\***********************************************************************/
void printFinalAverages()
{
    int totalEventCount = 0;
    int totalMissedCount = 0;
    double totalAverageResponseTime = 0;
    double totalAverageTurnAroundTime = 0;

    int i = 0;
    for(i; i < MAX_NUMBER_DEVICES; i++) 
    {            
        DeviceEventsInfo currEventInfo = eventInfoArray[i];
        totalEventCount += currEventInfo.seenEvents;
        totalMissedCount += currEventInfo.missedEvents;
        totalAverageResponseTime += currEventInfo.averageResponseTime;
        totalAverageTurnAroundTime += currEventInfo.averageTurnAroundTime;
        
    }

    printf("Average response time: %f\n", totalAverageResponseTime);
    printf("Average turn around time: %f\n", totalAverageTurnAroundTime);
}

/***********************************************************************\
* Input : eventId for DeviceEvntsInfo index, response time of event     *
          and turn around time of event.                                *
* Output: None                                                          *
* Function: Sets the responseTime and turnAroundTime for event          *
*           corresponding to eventId.                                   *
\***********************************************************************/
void setEventInfo(Identifier deviceId, Identifier eventId, 
                  double responseTime, double turnAroundTime)
{
    DeviceEventsInfo currEventInfo = eventInfoArray[deviceId];
    currEventInfo.responseTimes[eventId] = responseTime;
    currEventInfo.turnAroundTimes[eventId] = turnAroundTime;
    currEventInfo.seenEvents += 1;
    eventInfoArray[deviceId] = currEventInfo;
}

/***********************************************************************\
* Input : None                                                          *
* Output: None                                                          *
* Function: This must print out the number of Events buffered not yet   *
*           not yet processed (Server() function not yet called)        *
\***********************************************************************/
void BookKeeping(void){
  computeAllAverages();
  printFinalAverages();
  printf("\n >>>>>> Done\n");
}
