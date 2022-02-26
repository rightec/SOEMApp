#ifndef _slaveinfo_bis_h_
#define _slaveinfo_bis_h_

#define TECNA_REF_SPEED     "REF_SPEED"
#define TECNA_WEL_CUR       "WEL_CUR"
#define TECNA_LAST          "LAST_OUT"
#define TECNA_ENABLE_VERBOSE
#undef TECNA_ENABLE_VERBOSE

#define FINDREF_ON_ODLIST_AND_PRINT
#undef FINDREF_ON_ODLIST_AND_PRINT
#define OELIST_AND_PRINT
#undef OELIST_AND_PRINT
#define ODLIST_SCAN_AND_PRINT
#undef ODLIST_SCAN_AND_PRINT
#define SCAN_OE_TO_FIND_REFERENCES
#undef SCAN_OE_TO_FIND_REFERENCES

#define TECNA_NUM_MAX_ADI   1524 // For ethercat and anybus

typedef struct{
    /// tag@2602_01
    ec_OElistt OElistArrayItem;
    uint32_t absAddress;
}ec_OElistt_Tecna_t;

extern ec_ODlistt ODlist;
extern ec_OElistt OElist;
extern int iOelArryItems;
extern ec_OElistt OElistArray[TECNA_NUM_MAX_ADI];
extern ec_OElistt_Tecna_t OElistArrayTecna[TECNA_NUM_MAX_ADI]; /// tag@2602_01



int slavemain(char *argv);


#endif