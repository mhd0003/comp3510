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





/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/
//to keep track of events, response, and turnaround for each device. 
typedef struct DeviceTag {
    Timestamp responseTotal;
    Timestamp turnaroundTotal;
    int eventsProcessed;
    int responses;
    int turnarounds;
} Device;

/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/
Device devices[MAX_NUMBER_DEVICES];
//Event events[32000];
//int front;
//int back;
//int deviceNum;

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
    Event e;
    Status tempFlags;
    int deviceNum = 0;
	while (1)
	{
		if (Flags)
		{
			tempFlags = Flags;
            Flags = 0;
            deviceNum = 0;		

			while(tempFlags)
			{

				if (tempFlags & 1)
				{
                    e = BufferLastEvent[deviceNum];
                    //printf("Servicing event %d on device %d\n", e.EventID, deviceNum);
                    Server(&e);                  
                    devices[deviceNum].turnaroundTotal += Now() - e.When;
                    devices[deviceNum].turnarounds++;
                    devices[deviceNum].eventsProcessed++;
				}
				tempFlags = tempFlags >> 1;
				deviceNum++;
			}

            //Event e = BufferLastEvent[deviceNum];
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

    Status tempFlags = Flags;
    int deviceNum = 0;
   
    while(tempFlags)    
    {
        if(tempFlags & 1) 
        {
            Event e = BufferLastEvent[deviceNum];

            devices[deviceNum].responseTotal += Now() - e.When;
            devices[deviceNum].responses++;
            DisplayEvent('c', &e);
        }
        deviceNum = 0;
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
  Timestamp avgResponse = 0;
  Timestamp avgTurnaround = 0;
  float percentMissed;
  float avgPercentMissed = 0.0;
  while(n < Number_Devices)
  {
  	avgResponse += devices[n].responseTotal;
	avgTurnaround += devices[n].turnaroundTotal;
	devices[n].responseTotal = devices[n].responseTotal / devices[n].responses;
	devices[n].turnaroundTotal = devices[n].turnaroundTotal / devices[n].turnarounds++;
	percentMissed = (100.0 - devices[n].eventsProcessed) / 100.0;
	avgPercentMissed += percentMissed;
		
	printf("\n Device %d: Avg Response: %4.3f Avg Turnaround: %4.3f Percent Missed: %4.3f\n",
        n, devices[n].responseTotal, devices[n].turnaroundTotal, percentMissed);
	n++;
  }

  printf("\n Averages over all devices: Avg Response: %4.3f Avg Turnaround: %4.3f Avg Percent Missed: %4.3f\n",
  avgResponse/n, avgTurnaround/n, avgPercentMissed/n);
}




