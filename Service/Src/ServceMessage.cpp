#include "ServceMessage.h"
#include "tx_api.h"
#include "DL_F407.h"
#include "om.h"

SRAM_SET_CCM TX_THREAD Service_MSG_Thread;
[[noreturn]] void Service_MSG_Fun(ULONG initial_input) {
    om_init();
    while(true){


    }
}
