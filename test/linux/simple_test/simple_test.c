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
boolean g_b_cyclic = TRUE; /// True is CYC and false is ACYC

/**
 * @brief Set the output int16 object
 * 
 * @param slave_no: slave number in ethercat network
 * @param module_index: module index as index internal to the slave in case more than one
 *  channel
 * @param value: value to write
 */
void set_output_int16 (uint16 slave_no, uint8 module_index, int16 value)
{
   uint8 *data_ptr;

   data_ptr = ec_slave[slave_no].outputs;
   /* Move pointer to correct module index*/
   data_ptr += module_index * 2;
   /* Read value byte by byte since all targets can't handle misaligned
    * addresses
    */
   *data_ptr++ = (value >> 0) & 0xFF;
   *data_ptr++ = (value >> 8) & 0xFF;
}


void simpletest(char *ifname)
{
   int i, oloop, iloop;
   /// int j;
   needlf = FALSE;
   inOP = FALSE;

   int rdl = 0;
   uint8 nSM = 0;
   int iRefSpeed = 30;
   uint16_t  iLastOut = 0x0ABC;
   int iWelCurr = -1;
   int iTargetRefSpeedAttempt = 100; // 10000;
   int iTargetRefPos = -1;
   int iTargetWelCurPos = -1;
   int iTargetLastPos = -1;
   static char testRevision[100] = {0};
   static char tagCommit[100] = {0};
   ec_OElistt OElistTemp;


   sprintf(testRevision, "tag@0903_00.txt");
   sprintf(tagCommit, "tag_0903_0x");
   printf("\n");
   printf("Starting simple test revision %s\n", testRevision);
   printf("Against Slave with tag; %s\n", tagCommit);
   printf("\n");

#ifdef PRINT_MAPPED_OELIST
   printf("We mapped %d items in the OEList Array\n",iOelArryItems);
   for (i = 0; i < iOelArryItems; i++){
      memset(&OElistTemp,0,sizeof(ec_OElistt));
      memcpy(&OElistTemp,&OElistArrayTecna[i].OElistArrayItem,sizeof(ec_OElistt));
      printf("OElist item at %d entry.\n",i);
      for(int h = 0 ; h < OElistTemp.Entries ; h++)
      {
      printf("OElist.Name[i] is %s at %d entry.\n",OElistTemp.Name[h],h);
      printf("OElist Name lenght %d\n",(int)strlen(OElistTemp.Name[h]));
      printf("OElist.DataType[i] is %d at %d entry\n",OElistTemp.DataType[h],h);
      printf("OElist.BitLength[i] is %d at %d entry\n",OElistTemp.BitLength[h] ,h);
      printf("OElist.ObjAccess[i] is %d at %d entry\n",OElistTemp.ObjAccess[h],h);
      printf("OElist.ValueInfo[i] is %d at %d entry\n",OElistTemp.ValueInfo[h],h);
      printf("Absolute value is %d at %d entry\n",OElistArrayTecna[i].absAddress,h);
      printf("\n");
      } /// End for
   } /// end for
   #endif

#ifdef ODLIST_SCAN_AND_PRINT
   printf("Scan and Print full ODList\n");
   printf("\n");
   for( i = 0 ; i < ODlist.Entries ; i++)
   {
      printf("ODlist.Name[i] is %s at %d entry. Len is % d\n",ODlist.Name[i],i,(int)strlen(ODlist.Name[i]));
      printf("ODlist.DataType[i] is %d at %d entry\n",ODlist.DataType[i],i);
      printf("ODlist.ObjectCode[i] is %d at %d entry\n",ODlist.ObjectCode[i],i);
      printf("ODlist.Index[i] is %d at %d entry\n",ODlist.Index[i],i);
      printf("ODlist.MaxSub[i] is %d at %d entry\n",ODlist.MaxSub[i],i);
      printf("\n");
   } // End for
   printf("Scan and Print full ODList END\n");
   printf("\n");
#endif

#ifdef OELIST_AND_PRINT
            /// Print OEList - Start
            printf("\n");
            printf("Scan Print full OEList\n");
            printf("OElist.Entries is %d \n",OElist.Entries);
            printf("\n");

            for( i = 0 ; i < OElist.Entries ; i++)
            {
            printf("OElist.Name[i] is %s at %d entry. Len is % d\n",OElist.Name[i],i,(int)strlen(OElist.Name[i]));
            printf("OElist.DataType[i] is %d at %d entry\n",OElist.DataType[i],i);
            printf("OElist.BitLength[i] is %d at %d entry\n",OElist.BitLength[i] ,i);
            printf("OElist.ObjAccess[i] is %d at %d entry\n",OElist.ObjAccess[i],i);
            printf("OElist.ValueInfo[i] is %d at %d entry\n",OElist.ValueInfo[i],i);
            } // End for
            printf("\n");
            
#endif



   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n", ifname);
      /* find and auto-config slaves */
      if (ec_config_init(FALSE) > 0)
      {
         printf("%d slaves found and configured.\n", ec_slavecount);
         printf("\n");

/*****************************
 * 
 *  printf("ec_init on %s succeeded.\n",ifname);
      if ( ec_config(FALSE, &IOmap) > 0 )
      {
         printf("%d slaves found and configured.\n",ec_slavecount);
         ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);

         ec_configdc();

         ec_readstate();
 * ****************************/
         ec_config_map(&IOmap);

         ec_configdc();

         printf("Slaves mapped, state to SAFE_OP.\n");
         printf("\n");

         if (g_b_cyclic == TRUE)
         {
             printf("CYCLIC TEST not implemented in this branch\n");
         } 
         else 
         {
            /// Acyclic
            printf("Start ACYCLIC TEST\n");
            /// Print OEList - Start
            printf("\n");
            printf("Search for %s which length is %d\n",TECNA_ACYC_PARAM,(int)strlen(TECNA_ACYC_PARAM));
               
            /// Find the reference speed TECNA_ACYC_PARAM
            ec_OElistt OEListRefSpeed;
            int iTargetRefEntryPos = -1;
            for (i = 0; i < iOelArryItems; i++){
               memset(&OElistTemp,0,sizeof(ec_OElistt));
               memcpy(&OElistTemp,&OElistArray[i],sizeof(ec_OElistt));
               /// printf("OElist ACYC item at %d entry.\n",i);
               for(int h = 0 ; h < OElistTemp.Entries ; h++)
               {
                  // #define PRINT_OELIST_TEMP
                  #ifdef PRINT_OELIST_TEMP
                     printf("OElist.Name[i] is %s at %d entry.\n",OElistTemp.Name[h],h);
                     printf("OElist Name lenght %d\n",(int)strlen(OElistTemp.Name[h]));
                     printf("OElist.DataType[i] is %d at %d entry\n",OElistTemp.DataType[h],h);
                     printf("\n");
                  #endif
                  if (!strncmp(OElistTemp.Name[h], TECNA_ACYC_PARAM, (int)strlen(TECNA_ACYC_PARAM) ) ){
                     printf("Compared OK %s to string found %s at %d entry\n",TECNA_ACYC_PARAM,OElistTemp.Name[h], i);
                     iTargetRefPos = i;
                     iTargetRefEntryPos = h;
                     memcpy(&OEListRefSpeed,&OElistTemp,sizeof(ec_OElistt));
                     h = OElistTemp.Entries;
                  }  /// else
               } /// end for
            } /// End for

            if (iTargetRefPos == -1){
               printf(" %s Did not find\n",TECNA_ACYC_PARAM);
            } 
            else
            {
               printf(" %s FOUND\n",TECNA_ACYC_PARAM);
               printf("OElist.Name[i] is %s  \n",OEListRefSpeed.Name[iTargetRefEntryPos]);
               printf("OElist.DataType[i] is %d \n",OEListRefSpeed.DataType[iTargetRefEntryPos]);
               printf("OElist.BitLength[i] is %d \n",OEListRefSpeed.BitLength[iTargetRefEntryPos]);
               printf("OElist.ObjAccess[i] is %d \n",OEListRefSpeed.ObjAccess[iTargetRefEntryPos]);
               printf("OElist.ValueInfo[i] is %d \n",OEListRefSpeed.ValueInfo[iTargetRefEntryPos]);
               /// (ec_slave[0].outputs[iTargetRefPos]) = 15;
            }
            printf("\n");


            /****Acyclic Test Start Here*/
             /* wait for all slaves to reach SAFE_OP state */
            ec_config_map(&IOmap);

            ec_configdc();

            printf("Slaves REmapped, state to SAFE_OP.\n");
            /* wait for all slaves to reach SAFE_OP state */
            ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);

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

            printf("Number of ouptut Bytes is %d but oloop is %d\n",ec_slave[0].Obytes, oloop );
            printf("Number of input Bytes is %d but iloop is %d\n",ec_slave[0].Ibytes, iloop );
            
            /*
            if ((OEListRefSpeed.ObjAccess[iTargetRefEntryPos] & 0x007f))
            {
               uint16_t u16val = 13; /// Counter to set
               uint16_t index = 0x200d; // + 0x10;
               printf("iTargetRefEntryPos is %d and right access are 0x%x\n",iTargetRefEntryPos,(OEListRefSpeed.ObjAccess[iTargetRefEntryPos] ));
               /// ec_SDOwrite(slave, 0x8010, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);
               ec_SDOwrite(0, index, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTTXM); 
            }
            */

            /* acyclic loop 500 x 20ms = 10s */
            
            /*
            for(i = 1; i <= 1; i++)
            {
               printf("Processdata cycle %d , DCtime %12lld, O:", i, ec_DCtime);
               for(j = 0 ; j < oloop; j++)
               {
                  *(ec_slave[0].outputs + j) = j;
                  printf(" %2.2x", *(ec_slave[0].outputs + j));
               }
               printf(" I:");
               for(j = 0 ; j < iloop; j++)
               {
                  printf(" %2.2x", *(ec_slave[0].inputs + j));
               }
               printf("\n");
               osal_usleep(20000);
            }
            */
           printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_SAFE_OP;
         /* request OP state for all slaves */
         ec_writestate(0);
         osal_usleep(20000);
            for (int k=0; k<256; k++){
               printf("set_output_int16(0,k,23); k=0x%x\n",k);
               set_output_int16(0,k,23);
               osal_usleep(20000);
            }            

         }
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

      /// Diff between CYC and ACYC tag@2702_00
      if (strncmp(&argvMain[2][0], "-CYC",4)){
         if (strncmp(&argvMain[2][0], "-ACY",4)){
            printf("Please select if is a cyclic test (-CYC) or acyclic test (-ACY)\n");
            return -1;
         } else {
            g_b_cyclic = FALSE;
            printf("You selected an acyclic test (-ACY)\n");
         }
      }  else {
         g_b_cyclic = TRUE;
         printf("You selected a cyclic test (-CYC)\n");
      }


      if (slavemain((char*) &argvMain[1][0]) == -1){
         printf("Slavemain returned -1\n");
         return -1;
      } /// 
            

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

