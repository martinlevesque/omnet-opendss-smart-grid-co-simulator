//
// RandomWPMobilityTrace - 
// Copyright (C) 2006 Isabel Dietrich
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

package inet.mobility;
simple RandomWPMobilityTrace
{
    parameters:
        
        bool debug = default(false); // debug switch
        double x = default(-1); // start x coordinate (-1 = display string position, or random if it's missing)
        double y = default(-1); // start y coordinate (-1 = display string position, or random if it's missing)
        double updateInterval @unit("s") = default(0.1s);
        volatile double speed @unit("mps") = default(2mps); // use uniform(minSpeed, maxSpeed) or another distribution
        volatile double waitTime @unit("s"); // wait time between reaching a target and choosing a new one
        @display("i=block/cogwheel_s");

}

