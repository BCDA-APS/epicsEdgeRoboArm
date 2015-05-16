/*
 *  RoboArm.st
 *
 *  Jeffrey Gebhardt
 *  BCDA APS
 *  September 11, 2012
 *
 */

#define PRINTF errlogPrintf
#define MAX_MOVES   5
#define A_SHORT_WAIT  0.017

program RoboArm 

option +r; 

%% #include "stdlib.h"
%% #include "string.h"
%% #include "errlog.h"

int i, first;
evflag gripFlag;
evflag wristFlag;
evflag elbowFlag;
evflag shoulderFlag;
evflag baseFlag;
evflag ledFlag;
evflag gripDoneFlag;
evflag wristDoneFlag;
evflag elbowDoneFlag;
evflag shoulderDoneFlag;
evflag baseDoneFlag;
evflag ledDoneFlag;

int start_seq;
assign start_seq to "{P}{A}start_moves.VAL";
monitor start_seq;
evflag startFlag;
sync start_seq startFlag;

int stop_seq;
assign stop_seq to "{P}{A}all_stop.PROC";
monitor stop_seq;
evflag stopFlag;
sync stop_seq stopFlag;

int send_cmd = 1;
assign send_cmd to "{P}{A}send_cmd_seq.PROC";

int grip_seq;
assign grip_seq to "{P}{A}grip_move_seq.VAL";
int grip_moves[MAX_MOVES];
assign grip_moves to
                {"{P}{A}grip_move_1.VAL",
                 "{P}{A}grip_move_2.VAL",
                 "{P}{A}grip_move_3.VAL",
                 "{P}{A}grip_move_4.VAL",
                 "{P}{A}grip_move_5.VAL"};
double grip_starts[MAX_MOVES];
assign grip_starts to
                {"{P}{A}grip_start_1.VAL",
                 "{P}{A}grip_start_2.VAL",
                 "{P}{A}grip_start_3.VAL",
                 "{P}{A}grip_start_4.VAL",
                 "{P}{A}grip_start_5.VAL"};
double grip_delays[MAX_MOVES];
assign grip_delays to
                {"{P}{A}grip_delay_1.VAL",
                 "{P}{A}grip_delay_2.VAL",
                 "{P}{A}grip_delay_3.VAL",
                 "{P}{A}grip_delay_4.VAL",
                 "{P}{A}grip_delay_5.VAL"};

int wrist_seq;
assign wrist_seq to "{P}{A}wrist_move_seq.VAL";
int wrist_moves[MAX_MOVES];
assign wrist_moves to
                {"{P}{A}wrist_move_1.VAL",
                 "{P}{A}wrist_move_2.VAL",
                 "{P}{A}wrist_move_3.VAL",
                 "{P}{A}wrist_move_4.VAL",
                 "{P}{A}wrist_move_5.VAL"};
double wrist_starts[MAX_MOVES];
assign wrist_starts to
                {"{P}{A}wrist_start_1.VAL",
                 "{P}{A}wrist_start_2.VAL",
                 "{P}{A}wrist_start_3.VAL",
                 "{P}{A}wrist_start_4.VAL",
                 "{P}{A}wrist_start_5.VAL"};
double wrist_delays[MAX_MOVES];
assign wrist_delays to
                {"{P}{A}wrist_delay_1.VAL",
                 "{P}{A}wrist_delay_2.VAL",
                 "{P}{A}wrist_delay_3.VAL",
                 "{P}{A}wrist_delay_4.VAL",
                 "{P}{A}wrist_delay_5.VAL"};

int elbow_seq;
assign elbow_seq to "{P}{A}elbow_move_seq.VAL";
int elbow_moves[MAX_MOVES];
assign elbow_moves to
                {"{P}{A}elbow_move_1.VAL",
                 "{P}{A}elbow_move_2.VAL",
                 "{P}{A}elbow_move_3.VAL",
                 "{P}{A}elbow_move_4.VAL",
                 "{P}{A}elbow_move_5.VAL"};
double elbow_starts[MAX_MOVES];
assign elbow_starts to
                {"{P}{A}elbow_start_1.VAL",
                 "{P}{A}elbow_start_2.VAL",
                 "{P}{A}elbow_start_3.VAL",
                 "{P}{A}elbow_start_4.VAL",
                 "{P}{A}elbow_start_5.VAL"};
double elbow_delays[MAX_MOVES];
assign elbow_delays to
                {"{P}{A}elbow_delay_1.VAL",
                 "{P}{A}elbow_delay_2.VAL",
                 "{P}{A}elbow_delay_3.VAL",
                 "{P}{A}elbow_delay_4.VAL",
                 "{P}{A}elbow_delay_5.VAL"};

int shoulder_seq;
assign shoulder_seq to "{P}{A}shoulder_move_seq.VAL";
int shoulder_moves[MAX_MOVES];
assign shoulder_moves to
                {"{P}{A}shoulder_move_1.VAL",
                 "{P}{A}shoulder_move_2.VAL",
                 "{P}{A}shoulder_move_3.VAL",
                 "{P}{A}shoulder_move_4.VAL",
                 "{P}{A}shoulder_move_5.VAL"};
double shoulder_starts[MAX_MOVES];
assign shoulder_starts to
                {"{P}{A}shoulder_start_1.VAL",
                 "{P}{A}shoulder_start_2.VAL",
                 "{P}{A}shoulder_start_3.VAL",
                 "{P}{A}shoulder_start_4.VAL",
                 "{P}{A}shoulder_start_5.VAL"};
double shoulder_delays[MAX_MOVES];
assign shoulder_delays to
                {"{P}{A}shoulder_delay_1.VAL",
                 "{P}{A}shoulder_delay_2.VAL",
                 "{P}{A}shoulder_delay_3.VAL",
                 "{P}{A}shoulder_delay_4.VAL",
                 "{P}{A}shoulder_delay_5.VAL"};

int base_seq;
assign base_seq to "{P}{A}base_move_seq.VAL";
int base_moves[MAX_MOVES];
assign base_moves to
                {"{P}{A}base_move_1.VAL",
                 "{P}{A}base_move_2.VAL",
                 "{P}{A}base_move_3.VAL",
                 "{P}{A}base_move_4.VAL",
                 "{P}{A}base_move_4.VAL"};
double base_starts[MAX_MOVES];
assign base_starts to
                {"{P}{A}base_start_1.VAL",
                 "{P}{A}base_start_2.VAL",
                 "{P}{A}base_start_3.VAL",
                 "{P}{A}base_start_4.VAL",
                 "{P}{A}base_start_5.VAL"};
double base_delays[MAX_MOVES];
assign base_delays to
                {"{P}{A}base_delay_1.VAL",
                 "{P}{A}base_delay_2.VAL",
                 "{P}{A}base_delay_3.VAL",
                 "{P}{A}base_delay_4.VAL",
                 "{P}{A}base_delay_4.VAL"};

int led_seq;
assign led_seq to "{P}{A}led_onoff_seq.VAL";
int led_onoffs[MAX_MOVES];
assign led_onoffs to
                {"{P}{A}led_onoff_1.VAL",
                 "{P}{A}led_onoff_2.VAL",
                 "{P}{A}led_onoff_3.VAL",
                 "{P}{A}led_onoff_4.VAL",
                 "{P}{A}led_onoff_5.VAL"};
double led_starts[MAX_MOVES];
assign led_starts to
                {"{P}{A}led_start_1.VAL",
                 "{P}{A}led_start_2.VAL",
                 "{P}{A}led_start_3.VAL",
                 "{P}{A}led_start_4.VAL",
                 "{P}{A}led_start_5.VAL"};
double led_delays[MAX_MOVES];
assign led_delays to
                {"{P}{A}led_delay_1.VAL",
                 "{P}{A}led_delay_2.VAL",
                 "{P}{A}led_delay_3.VAL",
                 "{P}{A}led_delay_4.VAL",
                 "{P}{A}led_delay_5.VAL"};

ss coord_move {
    state init {
        when() {
            first     = 1;
            start_seq = 0;
            pvPut(start_seq, SYNC);
            efClear(gripFlag);
	    efClear(wristFlag);
	    efClear(elbowFlag);
	    efClear(shoulderFlag);
	    efClear(baseFlag);
	    efClear(ledFlag);
	    efClear(gripDoneFlag);
	    efClear(wristDoneFlag);
	    efClear(elbowDoneFlag);
	    efClear(shoulderDoneFlag);
	    efClear(baseDoneFlag);
            efClear(ledDoneFlag);
            printf("init complete\n");
        } state init_seq
    }
    state init_seq {
        when(start_seq && first) {
             first = 0;
             for(i=0; i<MAX_MOVES; i++) {
                pvGet(grip_moves[i]);
                pvGet(grip_starts[i]);
	        pvGet(grip_delays[i]);
	        pvGet(wrist_moves[i]);
	        pvGet(wrist_starts[i]);
	        pvGet(wrist_delays[i]);
	        pvGet(elbow_moves[i]);
	        pvGet(elbow_starts[i]);
	        pvGet(elbow_delays[i]);
	        pvGet(shoulder_moves[i]);
	        pvGet(shoulder_starts[i]);
	        pvGet(shoulder_delays[i]);
	        pvGet(base_moves[i]);
	        pvGet(base_starts[i]);
	        pvGet(base_delays[i]);
	        pvGet(led_onoffs[i]);
                pvGet(led_starts[i]);
	        pvGet(led_delays[i]);
            }
            i = -1;
            efClear(stopFlag);
	    printf("init_seq complete\n");
	} state start_seq  
    }
    state start_seq {
        when(start_seq && !first && !efTest(stopFlag)) {
            i++;
            efSet(gripFlag);
            efSet(wristFlag);
            efSet(elbowFlag);
            efSet(shoulderFlag);
            efSet(baseFlag);
            efSet(ledFlag);
            printf("start_seq complete\n");
	} state finish_seq  
	when(efTest(stopFlag)) {
	    i         = -1;
	    first     =  1;
	    start_seq =  0;
	    pvPut(start_seq, SYNC);
	    efClear(gripDoneFlag);
	    efClear(wristDoneFlag);
	    efClear(elbowDoneFlag);
	    efClear(shoulderDoneFlag);
	    efClear(baseDoneFlag);
	    efClear(ledDoneFlag);
	    printf("all_stop called\n");
	} state init_seq
    }
    state finish_seq {
	when(efTest(gripDoneFlag)    &&
	    efTest(wristDoneFlag)    &&
	    efTest(elbowDoneFlag)    &&
	    efTest(shoulderDoneFlag) &&
	    efTest(baseDoneFlag)     &&
            efTest(ledDoneFlag)      &&
            !efTest(stopFlag)        &&
            (i<MAX_MOVES-1)) {
            start_seq = 1;
            pvPut(start_seq, SYNC);
            efClear(gripDoneFlag);
	    efClear(wristDoneFlag);
	    efClear(elbowDoneFlag);
	    efClear(shoulderDoneFlag);
	    efClear(baseDoneFlag);
            efClear(ledDoneFlag);
            printf("finish_seq complete\n");
	} state start_seq
	when(efTest(gripDoneFlag)    &&
	    efTest(wristDoneFlag)    &&
	    efTest(elbowDoneFlag)    &&
	    efTest(shoulderDoneFlag) &&
	    efTest(baseDoneFlag)     &&
	    efTest(ledDoneFlag)      &&
	    !efTest(stopFlag)        &&
	    (i>=MAX_MOVES-1)) {
	    i         = -1;
	    first     =  1;
	    start_seq =  0;
	    pvPut(start_seq, SYNC);
	    efClear(gripDoneFlag);
	    efClear(wristDoneFlag);
	    efClear(elbowDoneFlag);
	    efClear(shoulderDoneFlag);
	    efClear(baseDoneFlag);
	    efClear(ledDoneFlag);
	    printf("finish_seq complete\n");
	} state init_seq
	when(efTest(stopFlag)) {
	    i         = -1;
	    first     =  1;
	    start_seq =  0;
	    pvPut(start_seq, SYNC);
	    efClear(gripDoneFlag);
	    efClear(wristDoneFlag);
	    efClear(elbowDoneFlag);
	    efClear(shoulderDoneFlag);
	    efClear(baseDoneFlag);
	    efClear(ledDoneFlag);
	    printf("all_stop called\n");
	} state init_seq
    }
}

ss grip {
    state grip_init {
        when(efTestAndClear(gripFlag)) {
        } state grip_run
    }
    state grip_run {
        when(delay(grip_starts[i])) {
            if (grip_moves[i]==1) {grip_seq = 1;}
            else {if (grip_moves[i]==2) {grip_seq = 2;}
            else {grip_seq = 0;}}
            if (grip_seq) {
                pvPut(grip_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tgrip = %d\tstart = %f\n", i, grip_moves[i], grip_starts[i]);
            }
            printf("grip_run complete\n");
        } state grip_stop
    } 
    state grip_stop {
        when(delay(grip_delays[i])) {
            if (grip_seq) {
                grip_seq = 0;
                pvPut(grip_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tgrip delay = %f\n", i, grip_delays[i]);
            }
            efSet(gripDoneFlag);
            printf("grip_stop complete\n");
        } state grip_init
    } 
}

ss wrist {
    state wrist_init {
        when(efTestAndClear(wristFlag)) {
        } state wrist_run
    }
    state wrist_run {
        when(delay(wrist_starts[i])) {
            if (wrist_moves[i]==1) {wrist_seq = 1;}
            else {if (wrist_moves[i]==2) {wrist_seq = 2;}
            else {wrist_seq = 0;}}
            if (wrist_seq) {
                pvPut(wrist_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\twrist = %d\tstart = %f\n", i, wrist_moves[i], wrist_starts[i]);
            }
            efClear(wristFlag);
            printf("wrist_run complete\n");
        } state wrist_stop
    } 
    state wrist_stop {
        when(delay(wrist_delays[i])) {
            if (wrist_seq) {
                wrist_seq = 0;
                pvPut(wrist_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\twrist delay = %f\n", i, wrist_delays[i]);
            }
            efSet(wristDoneFlag);
            printf("wrist_stop complete\n");
        } state wrist_init
    } 
}

ss elbow {
    state elbow_init {
        when(efTestAndClear(elbowFlag)) {
        } state elbow_run
    }
    state elbow_run {
        when(delay(elbow_starts[i])) {
            if (elbow_moves[i]==1) {elbow_seq = 1;}
            else {if (elbow_moves[i]==2) {elbow_seq = 2;}
            else {elbow_seq = 0;}}
            if (elbow_seq) {
                pvPut(elbow_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\telbow = %d\tstart = %f\n", i, elbow_moves[i], elbow_starts[i]);
            }
            efClear(elbowFlag);
            printf("elbow_run complete\n");
        } state elbow_stop
    } 
    state elbow_stop {
        when(delay(elbow_delays[i])) {
            if (elbow_seq) {
                elbow_seq = 0;
                pvPut(elbow_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\telbow delay = %f\n", i, elbow_delays[i]);
            }
            efSet(elbowDoneFlag);
            printf("elbow_stop complete\n");
        } state elbow_init
    } 
}

ss shoulder {
    state shoulder_init {
        when(efTestAndClear(shoulderFlag)) {
        } state shoulder_run
    }
    state shoulder_run {
        when(delay(shoulder_starts[i])) {
            if (shoulder_moves[i]==1) {shoulder_seq = 1;}
            else {if (shoulder_moves[i]==2) {shoulder_seq = 2;}
            else {shoulder_seq = 0;}}
            if (shoulder_seq) {
                pvPut(shoulder_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tshoulder = %d\tstart = %f\n", i, shoulder_moves[i], shoulder_starts[i]);
            }
            efClear(shoulderFlag);
            printf("shoulder_run complete\n");
        } state shoulder_stop
    } 
    state shoulder_stop {
        when(delay(shoulder_delays[i])) {
            if (shoulder_seq) {
                shoulder_seq = 0;
                pvPut(shoulder_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tshoulder delay = %f\n", i, shoulder_delays[i]);
            }
            efSet(shoulderDoneFlag);
            printf("shoulder_stop complete\n");
        } state shoulder_init
    } 
}

ss base {
    state base_init {
        when(efTestAndClear(baseFlag)) {
        } state base_run
    }
    state base_run {
        when(delay(base_starts[i])) {
            if (base_moves[i]==1) {base_seq = 1;}
            else {if (base_moves[i]==2) {base_seq = 2;}
            else {base_seq = 0;}}
            if (base_seq) {
                pvPut(base_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tbase = %d\tstart = %f\n", i, base_moves[i], base_starts[i]);
            }
            efClear(baseFlag);
            printf("base_run complete\n");
        } state base_stop
    } 
    state base_stop {
        when(delay(base_delays[i])) {
            if (base_seq) {
                base_seq = 0;
                pvPut(base_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tbase delay = %f\n", i, base_delays[i]);
            }
            efSet(baseDoneFlag);
            printf("base_stop complete\n");
        } state base_init
    } 
}

ss light {
    state led_init {
        when(efTestAndClear(ledFlag)) {
        } state led_run
    }
    state led_run {
        when(delay(led_starts[i])) {
            if (led_onoffs[i]==1) {
                led_seq = 1;
                pvPut(led_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tonoff = %d\tstart = %f\n", i, led_onoffs[i], led_starts[i]);
            }
            efClear(ledFlag);
            printf("led_run complete\n");
        } state led_stop
    } 
    state led_stop {
        when(delay(led_delays[i])) {
            if (led_onoffs[i]==1) {
                led_seq = 0;
                pvPut(led_seq, SYNC);
                sleep(A_SHORT_WAIT);
                pvPut(send_cmd, SYNC);
                printf("i = %d\tled delay = %f\n", i, led_delays[i]);
            }
            efSet(ledDoneFlag);
            printf("led_stop complete\n");
        } state led_init
    }
}


