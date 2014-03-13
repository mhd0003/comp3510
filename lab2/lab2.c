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

  while(n < Number_Devices)
  {
	devices[n].responseTotal = devices[n].responseTotal / (double) devices[n].responses;
	devices[n].turnaroundTotal = devices[n].turnaroundTotal / (double) devices[n].turnarounds++;
  	avgResponse += devices[n].responseTotal;
	avgTurnaround += devices[n].turnaroundTotal;

    deviceMissed = (100 - devices[n].eventsProcessed);
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
}




