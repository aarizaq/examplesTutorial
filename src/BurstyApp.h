//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
#ifndef _BURSTYAPP_
#define _BURSTYAPP_

#include <omnetpp.h>
#include "Packet_m.h"

using namespace omnetpp;

/**
 * A bursty packet generator; see NED file for more info.
 */
class BurstyApp : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    std::vector<int> destAddresses;
    cPar *sleepTime;
    cPar *burstTime;
    cPar *sendIATime;
    cPar *packetLengthBytes;

    // state
    cFSM fsm;
    enum {
        INIT = 0,
        SLEEP = FSM_Steady(1),
        ACTIVE = FSM_Steady(2),
        SEND = FSM_Transient(1),
    };

    int pkCounter;
    cMessage *startStopBurst;
    cMessage *sendMessage;
    int numSent;
    int numReceived;

    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;

  public:
    BurstyApp();
    virtual ~BurstyApp();

  protected:
    // redefined cSimpleModule methods
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

    // new methods
    virtual void processTimer(cMessage *msg);
    virtual void processPacket(Packet *pk);
    virtual void generatePacket();
};
#endif
