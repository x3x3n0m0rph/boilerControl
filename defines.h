#define ___DEBUG
#define ___DEVICE_UID     "e6d26990-9c94-4b87-af96-869b0b0bed50"

#define GSM_GPRS_SETUP_1  "AT+SAPBR=3,1,\"Contype\",\"GPRS\""
#define GSM_GPRS_SETUP_2  "AT+SAPBR=3,1,\"APN\",\"internet.tele2.ru\""
#define GSM_GPRS_SETUP_3  "AT+SAPBR=1,1"
#define GSM_HTTP_INIT     "AT+HTTPINIT"
#define GSM_HTTP_REQUEST  "AT+HTTPPARA=\"URL\",\"194.87.93.249/store?json={%22dev%22:%22"
#define GSM_HTTP_SUBMIT   "AT+HTTPACTION=0"
#define GSM_HTTP_TERM     "AT+HTTPTERM"

#define GSM_CMD_TIMEOUT   5000
#define GSM_INIT_TIMEOUT  5000
#define GSM_GPRS_TIMEOUT  7000
#define REFRESH_TIMEOUT   200
#define TRANSMIT_TIMEOUT  20000

#define DPIN_SIM800TX     5
#define DPIN_SIM800RX     6
#define DPIN_SIM800RST    12

#define DPIN_POWER_AC     8
#define DPIN_IGN          9
#define DPIN_PR_CLK       4 //CLOCK
#define DPIN_PR_SHLD      7 //LATCH
#define DPIN_PR_QH        10 //MISO 

#define APIN_BATTERY      A6
#define APIN_TEMP_WATER   A3
#define APIN_TEMP_AIR     A0

#define TERM_A            0.001353594791
#define TERM_B            0.000197337725
#define TERM_C            0.000000234795
#define TERM_PULLD_RES    10000.0
#define TERM_K_DELTA      273.15

#define AREF_VOLT         5.04
#define AREF_VOLT_BATT    5.04
#define AREF_STEPS        1024.0

#define DC_VM_BATT_R1     47000.0
#define DC_VM_BATT_R2     10050.0

#define TX_SEQ_NULL       0
#define TX_SEQ_AT_OK      10
#define TX_SEQ_SIGNAL_OK  20
#define TX_SEQ_GPRS_OK    30
#define TX_SEQ_SESS_OK    40
#define TX_SEQ_REQ_OK     50

int8_t GSMSignalLevel     = 0;
char GSMRegLevel          = 0;
bool GSMGPRS              = false;

bool  __flame             = false;
bool  __power_ac          = false;
float __t_water           = 0.0;
float __t_air             = 0.0;
float __u_batt            = 0.0;
float __p_water           = 0.0;
//uint16_t* filter_wt;
//uint16_t* filter_at;
//uint16_t* filter_ub;

String s__t_water         = "0.0";
String s__t_air           = "0.0";
String s__u_batt          = "0.0";
String s__p_water         = "0.0";

int timing                = 0;

String json_data          = "";
auto seq                  = TX_SEQ_NULL;
