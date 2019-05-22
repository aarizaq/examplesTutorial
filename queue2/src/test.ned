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
package queue;
import queue.Generator;
import queue.Queue;
import queue.Sink;
import ned.DatarateChannel;

network Test
{
    types:
        channel C1 extends DatarateChannel
        {
            parameters:
                delay = default(0.1ms);
                datarate = default(1Gbps);
        }
        channel C2 extends DatarateChannel
        {
            parameters:
                delay = default(0.1ms);
                datarate = default(1Gbps);
        }
    submodules:
        generator: Generator {
            @display("p=111,35");
        }
        queue: Queue {
            @display("p=181,35");
        }
        sink: Sink {
            @display("p=349,35");
        }

        generator1: Generator {
            @display("p=198,107");
        }
        queue1: Queue {
            @display("p=280,35");
        }

    connections:
        generator.out --> queue.in++;
        queue.out --> C1 --> queue1.in++;
        generator1.out --> queue1.in++;
        queue1.out --> C1 --> sink.in;
        
}
