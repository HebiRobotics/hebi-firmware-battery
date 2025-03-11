/**
 * battery_CAN.cpp
 * 
 * 
*/

#include "battery_CAN.h"
#include "board.h"
#include "all_msg.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}

namespace hebi::firmware::hardware {

/*
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 42.7.7 on the STM32 reference manual.
 */
static const CANConfig cancfg = {
    .mcr =  CAN_MCR_ABOM | //Auto bus-off management
            CAN_MCR_AWUM | //Auto wake-up mode
            CAN_MCR_TXFP,  //Chronological TX FIFO priority
    .btr =  //CAN_BTR_LBKM | //Loopback mode
            CAN_BTR_SJW(3) | //Resync jump width
            CAN_BTR_TS2(1) |  //Time in TS2
            CAN_BTR_TS1(12) | //Time in TS1
            CAN_BTR_BRP(10)    //Baud rate = APB1 / BRP
};

/*
* Receiver thread.
*/
static THD_WORKING_AREA(can_rx1_wa, 256);
static THD_FUNCTION(can_rx, p) {
    CANDriver *canp = (CANDriver *)p;
    event_listener_t el;
    CANRxFrame rxmsg;

    (void)p;
    chRegSetThreadName("receiver");
    chEvtRegister(&canp->rxfull_event, &el, 0);
    while(!chThdShouldTerminateX()) {
        if (chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100)) == 0)
            continue;
        while (canReceive(canp, CAN_ANY_MAILBOX,
                            &rxmsg, TIME_IMMEDIATE) == MSG_OK) {
            /* Process message.*/
            palToggleLine(LINE_LED_R);
        }
    }
    chEvtUnregister(&CAND1.rxfull_event, &el);
}

/*
* Transmitter thread.
*/
static protocol::base_msg tx_msg(0, protocol::MessageType::MSG_INVALID);
thread_reference_t can_tx_thread_ref = NULL;

static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p) {
    CANTxFrame txmsg;
    // protocol::battery_state_msg test_msg(0x01, 1, 2, 3, 4); //TEST

    (void)p;
    chRegSetThreadName("transmitter");
    chThdSuspendS(&can_tx_thread_ref);

    while (!chThdShouldTerminateX()) {

        chSysLock();
        protocol::base_msg msg = tx_msg;
        chSysUnlock();

        txmsg.IDE = CAN_IDE_EXT;
        txmsg.EID = msg.EID.raw;
        txmsg.RTR = CAN_RTR_DATA;
        txmsg.DLC = msg.len;
        txmsg.data32[0] = msg.data32[0];
        txmsg.data32[1] = msg.data32[1];

        canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
        chThdSuspendS(&can_tx_thread_ref);
    }
}


Battery_CAN::Battery_CAN(){


    palWriteLine(LINE_CAN1_STB, PAL_LOW);
    palWriteLine(LINE_CAN1_SHDN, PAL_LOW);
  /*
   * Activates the CAN drivers 1 and 2.
   */
  canStart(&CAND1, &cancfg);

  /*
   * Starting the transmitter and receiver threads.
   */
  chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7,
                    can_rx, &CAND1);
  chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7,
                    can_tx, NULL);
}
   
void Battery_CAN::sendMessage(protocol::base_msg msg){
    if(can_tx_thread_ref != NULL){
        chSysLock();
        tx_msg = msg;
        chSysUnlock();
    
        chThdResume(&can_tx_thread_ref, MSG_OK);
    }
}
    
} //namespace hebi::firmware::hardware