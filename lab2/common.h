/*****************************************************************************\
 * Laboratory Exercises COMP 3510                                              *
 * Author: Saad Biaz                                                           *
 * Date  : February 20, 2009 (Improved compared to the version for lab2)       *
 *         January 18, 2012  (Improved it)                                     *
 * File  : common.h                                                            *
 \*****************************************************************************/

/*****************************************************************************\
*                             Global system headers                           *
\*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
/*****************************************************************************\
*                             Global data types                               *
\*****************************************************************************/

typedef int             Identifier;
typedef int             Priority;
typedef short           Flag;
typedef unsigned        Status;
typedef double          Timestamp;
typedef int             Quantity;
typedef double          Average;

/*****************************************************************************\
*                             Global definitions                              *
\*****************************************************************************/

#define  MAX_NUMBER_DEVICES 32     /* Number of Devices                      */
#define  MAX_PRIORITIES      4     /* Number of Priority Levels              */
#define  MAX_MSG_LENGTH     64     /* Max message length                     */
#define  FALSE               0     /* False condition                        */
#define  TRUE                1     /* True condition                         */


/*****************************************************************************\
*                            Global data structures                           *
\*****************************************************************************/

typedef struct EventTag{
  Identifier EventID;
  Identifier DeviceID;
  Timestamp  When;
  Priority   priority;
  char       msg[MAX_MSG_LENGTH];
} Event;

/*****************************************************************************\
*                                  Global data                                *
\*****************************************************************************/

extern Status         Flags;
extern Event          BufferLastEvent[MAX_NUMBER_DEVICES];
extern Flag           Show;
extern Quantity       Number_Devices;


/*****************************************************************************\
*                               Function prototypes                           *
\*****************************************************************************/
extern Timestamp   Now(void);
extern void        Server(Event *whichEvent);
extern void        InterruptRoutineHandlerDevice(void);
extern void        BookKeeping(void);
extern Flag        Initialization(int argc, char **argv);
extern void        DisplayEvent(char c,Event *whichEvent);

















