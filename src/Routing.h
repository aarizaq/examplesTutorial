//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef _ROUTING_
#define _ROUTING_
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif


#include <map>
#include <deque>
#include <vector>
#include <omnetpp.h>
#include "Packet_m.h"

using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
  private:
    typedef std::deque<Packet *> OutputQueue;

    int myAddress;

    typedef std::map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;

    std::vector<OutputQueue> outputQueues;
    std::vector<cMessage *> queueTimers;

    simsignal_t dropSignal;
    simsignal_t outputIfSignal;
    int frameCapacity = -1;

  protected:
    ~Routing();
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
