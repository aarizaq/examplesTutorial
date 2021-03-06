//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


package simplenetwork;

//
// A "Node" consists of a Routing plus an App module which represents the
// aggregate traffic of the subnets connected to that router.
//
module Node
{
    parameters:
        int address;
        string appType;
        @display("i=misc/node_vs,gold");
    gates:
        inout port[];
    submodules:
        app: <appType> like IApp {
            parameters:
                address = address;
                @display("p=140,60");
        }
        routing: Routing {
            parameters:
                @display("p=140,130");
            gates:
                line[sizeof(port)];
        }
    connections:
        routing.localOut --> app.in;
        routing.localIn <-- app.out;
        for i=0..sizeof(port)-1 {
            routing.line[i] <--> port[i];
        }
}


