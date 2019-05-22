//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Queue.h"

Define_Module(Queue);

Queue::~Queue()
{
    cancelAndDelete(timerSent);
    while (!queue.empty()) {
        delete queue.back();
        queue.pop_back();
    }
}

void Queue::initialize()
{
    // TODO - Generated method body
    timerSent = new cMessage("Queue Timer");
    WATCH(numRec);
    WATCH(numSend);
}

void Queue::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if (msg != timerSent && timerSent->isScheduled()) {
        numRec++;
        auto pkt = check_and_cast<cPacket *>(msg);
        queue.push_back(pkt);
    }
    else if (msg != timerSent && queue.empty() && !timerSent->isScheduled()) {
        // sent immediately, queue is empty
        // compute time
        int serviceRate = par("serviceRate");// The type of variable must be of the same type that the defined in the ned file
        auto pkt = check_and_cast<cPacket *>(msg);
        double t = (double)pkt->getBitLength()/(double)serviceRate; // Using casting to avoid the integer division (results 0, 1, 2 ...)
        scheduleAt(simTime()+t, timerSent);
        send(pkt,"out");
        numRec++;
        numSend++;
    }
    else if (msg == timerSent && !queue.empty()) {
        auto pkt = queue.front();
        queue.pop_front();
        int serviceRate = par("serviceRate");
        double t = (double)pkt->getBitLength()/(double)serviceRate;
        scheduleAt(simTime()+t, timerSent);
        send(pkt,"out");
        numSend++;
    }
}
