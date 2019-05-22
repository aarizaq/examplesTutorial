//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "Routing.h"

Define_Module(Routing);

Routing::~Routing() {
    for(auto &elem : queueTimers) {
        cancelAndDelete(elem);
    }
    queueTimers.clear();
    for(auto &elem : outputQueues) {
        for(auto &elem2 : elem) {
            delete elem2;
        }
    }
    outputQueues.clear();
}

void Routing::initialize()
{
    myAddress = getParentModule()->par("address");
    dropSignal = registerSignal("drop");
    outputIfSignal = registerSignal("outputIf");
    //
    // Brute force approach -- every node does topology discovery on its own,
    // and finds routes to all other nodes independently, at the beginning
    // of the simulation. This could be improved: (1) central routing database,
    // (2) on-demand route calculation
    //
    cTopology *topo = new cTopology("topo");

    std::vector<std::string> nedTypes;
    nedTypes.push_back(getParentModule()->getNedTypeName());
    topo->extractByNedTypeName(nedTypes);
    EV << "cTopology found " << topo->getNumNodes() << " nodes\n";

    cTopology::Node *thisNode = topo->getNodeFor(getParentModule());

    // find and store next hops
    for (int i = 0; i < topo->getNumNodes(); i++) {
        if (topo->getNode(i) == thisNode)
            continue;  // skip ourselves
        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

        if (thisNode->getNumPaths() == 0)
            continue;  // not connected

        cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        int gateIndex = parentModuleGate->getIndex();
        int address = topo->getNode(i)->getModule()->par("address");
        rtable[address] = gateIndex;
        EV << "  towards address " << address << " gateIndex is " << gateIndex << endl;
    }
    delete topo;
    outputQueues.resize(this->gateSize("line"));
    for (int i = 0; i < this->gateSize("line"); i++) {
        std::string name("timer");
        name += std::to_string(i);
        auto msg = new cMessage(name.c_str(),i);
        queueTimers.push_back(msg);
    }
    frameCapacity = par("frameCapacity");
}

void Routing::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        // end transmission message
        int outGate = msg->getKind();
        if (outputQueues[outGate].empty())
            return;
        auto pk = outputQueues[outGate].front();
        outputQueues[outGate].pop_front();
        send(pk, "line$o", outGate);
        simtime_t endTransmission = gate("line$o", outGate)->getTransmissionChannel()->getTransmissionFinishTime();
        scheduleAt(endTransmission, queueTimers[outGate]);
        return;
    }

    Packet *pk = check_and_cast<Packet *>(msg);
    int destAddr = pk->getDestAddr();

    if (destAddr == myAddress) {
        EV << "local delivery of packet " << pk->getName() << endl;
        send(pk, "localOut");
        emit(outputIfSignal, -1);  // -1: local
        return;
    }
    int outGateIndex = -1;

    // application can force the output gate with independence of the routing table
    auto controlInfo = pk->removeControlInfo();
    if (controlInfo != nullptr) {
        auto outPort = dynamic_cast<OutputPort *>(controlInfo);
        if (outPort != nullptr && outPort->getPort() < outputQueues.size())
            outGateIndex = outPort->getPort();
        delete controlInfo;
    }

    // no valid gate, search in the routing table
    if (outGateIndex == -1) {
        RoutingTable::iterator it = rtable.find(destAddr);
        if (it == rtable.end()) {
            EV << "address " << destAddr << " unreachable, discarding packet "
                      << pk->getName() << endl;
            emit(dropSignal, (long) pk->getByteLength());
            delete pk;
            return;
        }
        outGateIndex = (*it).second;
    }

    EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
    pk->setHopCount(pk->getHopCount()+1);
    emit(outputIfSignal, outGateIndex);
    // enqueue or send
    if (queueTimers[outGateIndex]->isScheduled()) {
        if ((frameCapacity < 0) || (frameCapacity > (int) outputQueues[outGateIndex].size())) {
            outputQueues[outGateIndex].push_back(pk);
        }
        else {
            emit(dropSignal, (long)pk->getByteLength());
            delete pk;
        }
    }
    else {
        send(pk, "line$o", outGateIndex);
        simtime_t endTransmission = gate("line$o",outGateIndex)->getTransmissionChannel()->getTransmissionFinishTime();
        scheduleAt(endTransmission, queueTimers[outGateIndex]);

    }
}

