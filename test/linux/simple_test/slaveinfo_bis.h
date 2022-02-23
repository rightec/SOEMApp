#ifndef _slaveinfo_bis_h_
#define _slaveinfo_bis_h_

#define TECNA_REF_SPEED     "REF_SPEED"
#define TECNA_WEL_CUR       "WEL_CUR"
#define TECNA_ENABLE_VERBOSE
#undef TECNA_ENABLE_VERBOSE

#define FINDREF_ON_ODLIST_AND_PRINT
// #undef FINDREF_ON_ODLIST_AND_PRINT
#define OELIST_AND_PRINT
// #undef OELIST_AND_PRINT

extern ec_ODlistt ODlist;
extern ec_OElistt OElist;


int slavemain(char *argv);


#endif