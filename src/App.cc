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

#include "App.h"


Define_Module(App);

App::App()
{
    generatePacket = nullptr;
}

App::~App()
{
    cancelAndDelete(generatePacket);
}

void App::initialize()
{
    myAddress = par("address");
    packetLengthBytes = &par("packetLength");
    sendIATime = &par("sendIaTime");  // volatile parameter
    pkCounter = 0;

    WATCH(pkCounter);
    WATCH(myAddress);

    const char *destAddressesPar = par("destAddresses");
    cStringTokenizer tokenizer(destAddressesPar);
    const char *token;
    while ((token = tokenizer.nextToken()) != nullptr)
        destAddresses.push_back(atoi(token));

    if (destAddresses.size() == 0)
        throw cRuntimeError("At least one address must be specified in the destAddresses parameter!");

    generatePacket = new cMessage("nextPacket");
    scheduleAt(sendIATime->doubleValue(), generatePacket);

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");
}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacket) {
        // Sending packet
        int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

        char pkname[40];
        sprintf(pkname, "pk-%d-to-%d-#%ld", myAddress, destAddress, pkCounter++);
        EV << "generating packet " << pkname << endl;

        cPacket *pk = new cPacket(pkname);
        PacketInfo *info = new PacketInfo();

        pk->setByteLength(packetLengthBytes->intValue());
        pk->setKind(intuniform(0, 7));

        info->setSrcAddr(myAddress);
        info->setDestAddr(destAddress);
        pk->setControlInfo(info);

        send(pk, "out");

        scheduleAt(simTime() + sendIATime->doubleValue(), generatePacket);
        if (hasGUI())
            getParentModule()->bubble("Generating packet...");
    }
    else {
        // Handle incoming packet
        cPacket *pk = check_and_cast<cPacket *>(msg);
        PacketInfo *info = check_and_cast<PacketInfo*> (pk->removeControlInfo());

        EV << "received packet " << pk->getName() << " after " << info->getHopCount() << "hops" << endl;
        emit(endToEndDelaySignal, simTime() - pk->getCreationTime());
        emit(hopCountSignal, info->getHopCount());
        emit(sourceAddressSignal, info->getSrcAddr());
        delete info;
        delete pk;

        if (hasGUI())
            getParentModule()->bubble("Arrived!");
    }
}
