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

#include <string>
#include "Generator.h"
#include "simpleMsg_m.h"

Define_Module(Generator); // connect c++ code with the ned definition

Generator::~Generator() {

    cancelAndDelete(timer);
}
void Generator::initialize()
{
    timer = new cMessage("timer");
    scheduleAt(simTime()+par("arrivalTime"), timer);
    WATCH(sent);
    // TODO - Generated method body
}

void Generator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if (msg == timer) {
        // send new packet
        sent++;
        std::string name("pktNum:");
        name += std::to_string(sent);
        auto pkt = new SimpleMsg(name.c_str());
        pkt->setByteLength(std::floor(par("size").doubleValue()));
        pkt->setCreate(simTime());
        pkt->setNum(sent);

        send(pkt,"out");
        scheduleAt(simTime()+par("arrivalTime"), timer);
    }
}
