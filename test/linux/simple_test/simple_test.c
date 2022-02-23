/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "ethercat.h"

#include "slaveinfo_bis.h"

#define EC_TIMEOUTMON 500

char IOmap[4096];
OSAL_THREAD_HANDLE thread1;
int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;



void simpletest(char *ifname)
{
   int i, j, oloop, iloop, chk;
   needlf = FALSE;
   inOP = FALSE;

   int rdl = 0;
   uint8 nSM = 0;
   int iRefSpeed = 30;
   int iWelCurr = -1;
   int iTargetRefSpeedAttempt = 100; // 10000;
   int iTargetRefPos = -1;
   int iTargetWelCurPos = -1;


   printf("Starting simple test\n");

   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n", ifname);
      /* find and auto-config slaves */

      if (ec_config_init(FALSE) > 0)
      {
         printf("%d slaves found and configured.\n", ec_slavecount);

         /*TO DELETE
         wkc = ec_SDOread(0, ECT_SDO_SMCOMMTYPE, 0x00, FALSE, &rdl, &nSM, EC_TIMEOUTRXM);
         printf("--------FIRST--------------\n");
         printf("Slave State is : %d\n",ec_slave[0].state);
         printf ("Workcunter now is %d\n",wkc);
         printf ("Bytes read from SDO are %d\n",rdl);
         printf ("Pointer to %d\n",nSM);
         */


         ec_config_map(&IOmap);

         /*TO DELETE
         wkc = ec_SDOread(0, ECT_SDO_SMCOMMTYPE, 0x00, FALSE, &rdl, &nSM, EC_TIMEOUTRXM);
         printf("--------SECOND--------------\n");
         printf("Slave State is : %d\n",ec_slave[0].state);
         printf ("Workcunter now is %d\n",wkc);
         printf ("Bytes read from SDO are %d\n",rdl);
         printf ("Pointer to %d\n",nSM);
         */
         ec_configdc();

         /*TO DELETE
         wkc = ec_SDOread(0, ECT_SDO_SMCOMMTYPE, 0x00, FALSE, &rdl, &nSM, EC_TIMEOUTRXM);
         printf("--------THIRD--------------\n");
         printf("Slave State is : %d\n",ec_slave[0].state);
         printf ("Workcunter now is %d\n",wkc);
         printf ("Bytes read from SDO are %d\n",rdl);
         printf ("Pointer to %d\n",nSM);
         */

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);

         /* read SyncManager Communication Type object count */
         wkc = ec_SDOread(0, ECT_SDO_SMCOMMTYPE, 0x00, FALSE, &rdl, &nSM, EC_TIMEOUTRXM);
         printf("--------FOURTH--------------\n");
         printf("Slave State is : %d\n",ec_slave[0].state);
         printf ("Workcunter now is %d\n",wkc);
         printf ("Bytes read from SDO are %d\n",rdl);
         printf ("Pointer to %d\n",nSM);

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0))
            oloop = 1;
         if (oloop > 8)
            oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0))
            iloop = 1;
         if (iloop > 8)
            iloop = 8;

         printf("segments : %d : %d %d %d %d\n", ec_group[0].nsegments, ec_group[0].IOsegment[0], ec_group[0].IOsegment[1], ec_group[0].IOsegment[2], ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;
         /* send one valid process data to make outputs in slaves happy*/
         ec_send_processdata();
         ec_receive_processdata(EC_TIMEOUTRET);
         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 200;
         /* wait for all slaves to reach OP state */
         do
         {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
         if (ec_slave[0].state == EC_STATE_OPERATIONAL)
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;

            // printf("Slave Cycle Time is: %d \n ",  ec_slave[0].DCcycle); // is 0
            // printf("Slave Type is: %d \n ",  ec_slave[0].Dtype); // is 0
            printf("Num of output bytes: %d \n ",ec_slave[0].Obytes); 
            
            *(ec_slave[0].outputs + 4) = 3;
            (ec_slave[0].outputs[9]) = 15;

#ifdef FINDREF_ON_ODLIST_AND_PRINT
            /// Find the REF_SPEED name - Start
            for( i = 0 ; i < ODlist.Entries ; i++)
            {
            char name[128] = { 0 };
            snprintf(name, sizeof(name) - 1, "\"%s\"", ODlist.Name[i]);
//            if (strncmp(name, TECNA_REF_SPEED, (int)strlen(name) ) )
               printf("ODlist.Name[i] is %s at %d entry. Len is % d\n",ODlist.Name[i],i,(int)strlen(ODlist.Name[i]));
               printf("ODlist.DataType[i] is %d at %d entry\n",ODlist.DataType[i],i);
               printf("ODlist.ObjectCode[i] is %d at %d entry\n",ODlist.ObjectCode[i],i);
               printf("ODlist.Index[i] is %d at %d entry\n",ODlist.Index[i],i);
               printf("ODlist.MaxSub[i] is %d at %d entry\n",ODlist.MaxSub[i],i);
            if (strncmp(ODlist.Name[i], TECNA_REF_SPEED, (int)strlen(ODlist.Name[i]) ) )
               {
                  printf("NOT Found REF %s at %d entry. strlen(name) is %d , (int)strlen(TECNA_REF_SPEED) is %d    \n",name,i,(int)strlen(name),(int)strlen(TECNA_REF_SPEED));
               }  else {
                  printf("Find REF %s at %d entry\n",name,i);
                  iTargetRefPos = i;
                  /// i = ODlist.Entries;
               }
            } // End for

            if (iTargetRefPos > -1){
               (ec_slave[0].outputs[iTargetRefPos]) = iRefSpeed;
            }
            /// Find the REF_SPEED name - End
            #endif

            /// Find the WEL_CUR name - Start
            for( i = 0 ; i < OElist.Entries ; i++)
            {
            /// char name[128] = { 0 };
            /// snprintf(name, sizeof(name) - 1, "\"%s\"", OElist.Name[i]);
            if (!strncmp(OElist.Name[i], TECNA_WEL_CUR, (int)strlen(TECNA_WEL_CUR) ) )
               {
                  printf("Compared OK %s to string found %s at %d entry\n",TECNA_WEL_CUR,OElist.Name[i], i);
                  iTargetWelCurPos = i;
                  // i = ODlist.Entries;
               } else {
                  if (!strncmp(OElist.Name[i], TECNA_REF_SPEED, (int)strlen(TECNA_REF_SPEED) ) ){
                     printf("Compared OK %s to string found %s at %d entry\n",TECNA_REF_SPEED,OElist.Name[i], i);
                     iTargetRefPos = i;
                  } else {
                     printf("Compared KO string found %s at %d entry\n",OElist.Name[i], i);
                  }
               }
            } // End for

            if (iTargetWelCurPos > -1){
               printf("Target wel curr is at: %d\n", iWelCurr);
               iWelCurr = (ec_slave[0].inputs[iTargetWelCurPos]);
            } else {
                  printf("Target wel curr NOT FOUND\n");
            }

            if (iTargetRefPos > -1){
               printf("Target ref speed  is at: %d\n", iTargetRefPos);
            } else {
                  printf("Target ref speed NOT FOUND\n");
            }

            /// Find the WEL_CUR name - End

#ifdef OELIST_AND_PRINT
            /// Print OEList - Start
            printf("OElist.Entries is %d \n",OElist.Entries);
            for( i = 0 ; i < OElist.Entries ; i++)
            {
            char name[128] = { 0 };
            snprintf(name, sizeof(name) - 1, "\"%s\"", OElist.Name[i]);
            printf("OElist.Name[i] is %s at %d entry. Len is % d\n",OElist.Name[i],i,(int)strlen(OElist.Name[i]));
            printf("OElist.DataType[i] is %d at %d entry\n",OElist.DataType[i],i);
            printf("OElist.BitLength[i] is %d at %d entry\n",OElist.BitLength[i] ,i);
            printf("OElist.ObjAccess[i] is %d at %d entry\n",OElist.ObjAccess[i],i);
            printf("OElist.ValueInfo[i] is %d at %d entry\n",OElist.ValueInfo[i],i);
            } // End for
#endif



            /* cyclic loop */
            i = 0;
            while (i <= iTargetRefSpeedAttempt)
            {
               ec_send_processdata();
               wkc = ec_receive_processdata(EC_TIMEOUTRET);

               if (wkc >= expectedWKC)
               {
                  printf("Processdata cycle %4d, WKC %d , O:", i, wkc);

                  for (j = 0; j < oloop; j++)
                  {
                     printf(" %2.2x", *(ec_slave[0].outputs + j));
                  }

                  printf(" I:");
                  for (j = 0; j < iloop; j++)
                  {
                     printf(" %2.2x", *(ec_slave[0].inputs + j));
                  }
                  printf(" T:%" PRId64 "\r", ec_DCtime);
                  needlf = TRUE;

                  if (ec_slave[0].inputs[iTargetRefPos] == iRefSpeed){
                     printf("Target reached at: %d\n", i);
                     i = iTargetRefSpeedAttempt;
                  } ///

/*
                  iWelCurr = ec_slave[0].inputs[iTargetWelCurPos];
                  if (iWelCurr != -1){
                     printf("Target wel curr is at: %d\n", iWelCurr);
                  } ///
                  */
               }
               osal_usleep(5000);
               i++;
            } // end while cycle
            inOP = FALSE;
         }
         else
         {
            printf("Not all slaves reached operational state.\n");
            ec_readstate();
            for (i = 1; i <= ec_slavecount; i++)
            {
               if (ec_slave[i].state != EC_STATE_OPERATIONAL)
               {
                  printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                         i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
               }
            }
         }
         printf("\nRequest init state for all slaves\n");
         ec_slave[0].state = EC_STATE_INIT;
         /* request INIT state for all slaves */
         ec_writestate(0);
      }
      else
      {
         printf("No slaves found!\n");
      }
      printf("End simple test, close socket\n");
      /* stop SOEM, close socket */
      ec_close();
   }
   else
   {
      printf("No socket connection on %s\nExecute as root\n", ifname);
   }
}

OSAL_THREAD_FUNC ecatcheck(void *ptr)
{
   int slave;
   (void)ptr; /* Not used */

   while (1)
   {
      if (inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
      {
         if (needlf)
         {
            needlf = FALSE;
            printf("\n");
         }
         /* one ore more slaves are not responding */
         ec_group[currentgroup].docheckstate = FALSE;
         ec_readstate();
         for (slave = 1; slave <= ec_slavecount; slave++)
         {
            if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
            {
               ec_group[currentgroup].docheckstate = TRUE;
               if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
               {
                  printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                  ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                  ec_writestate(slave);
               }
               else if (ec_slave[slave].state == EC_STATE_SAFE_OP)
               {
                  printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                  ec_slave[slave].state = EC_STATE_OPERATIONAL;
                  ec_writestate(slave);
               }
               else if (ec_slave[slave].state > EC_STATE_NONE)
               {
                  if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d reconfigured\n", slave);
                  }
               }
               else if (!ec_slave[slave].islost)
               {
                  /* re-check state */
                  ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                  if (ec_slave[slave].state == EC_STATE_NONE)
                  {
                     ec_slave[slave].islost = TRUE;
                     printf("ERROR : slave %d lost\n", slave);
                  }
               }
            }
            if (ec_slave[slave].islost)
            {
               if (ec_slave[slave].state == EC_STATE_NONE)
               {
                  if (ec_recover_slave(slave, EC_TIMEOUTMON))
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d recovered\n", slave);
                  }
               }
               else
               {
                  ec_slave[slave].islost = FALSE;
                  printf("MESSAGE : slave %d found\n", slave);
               }
            }
         }
         if (!ec_group[currentgroup].docheckstate)
            printf("OK : all slaves resumed OPERATIONAL.\n");
      }
      osal_usleep(10000);
   }
}

int main(int argc, char *argv[])
{
   printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");
   char argvMain[5][300];
   int iC= 0;
   if (argc > 1)
   {

      while (*argv != NULL)
    {
            printf("ARGV %s\n", *argv);
            printf("strlen(*argv) %d\n", (int)strlen(*argv));
            strcpy(&argvMain[iC][0],*argv);
            printf("argvMain %d is %s\n",iC, &argvMain[iC][0]);
             argv++;
            iC++;
    }
      if (slavemain((char*) &argvMain[1][0]) == -1)
            return -1;
      
      /* create thread to handle slave error handling in OP */
      //      pthread_create( &thread1, NULL, (void *) &ecatcheck, (void*) &ctime);
      osal_thread_create(&thread1, 128000, &ecatcheck, (void *)&ctime);
      /* start cyclic part */
      simpletest((char*) &argvMain[1][0]);
   }
   else
   {
      ec_adaptert *adapter = NULL;
      printf("Usage: simple_test ifname1\nifname = eth0 for example\n");

      printf("\nAvailable adapters:\n");
      adapter = ec_find_adapters();
      while (adapter != NULL)
      {
         printf("    - %s  (%s)\n", adapter->name, adapter->desc);
         adapter = adapter->next;
      }
      ec_free_adapters(adapter);
   }

   printf("End program\n");
   return (0);
}

