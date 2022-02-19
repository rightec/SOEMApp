#ifndef _slaveinfo_bis_h_
#define _slaveinfo_bis_h_

#define TECNA_REF_SPEED     "REF_SPEED"
#define TECNA_ENABLE_VERBOSE
#undef TECNA_ENABLE_VERBOSE

extern ec_ODlistt ODlist;
extern ec_OElistt OElist;


int slavemain(char *argv);


#endif