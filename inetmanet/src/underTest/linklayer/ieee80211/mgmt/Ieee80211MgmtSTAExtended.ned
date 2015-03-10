//
// Station (managed mode) 802.11 managmenet implementation
//
// @author Juan-Carlos Maureira
//
package inet.underTest.linklayer.ieee80211.mgmt;
simple Ieee80211MgmtSTAExtended like Ieee80211MgmtInterface
{
    parameters:
        int frameCapacity = default(100); // maximum queue length
        double max_beacons_missed = default(5); // maximum number of beacon missed to notify agent about beacon lost event
        @display("i=block/cogwheel");
    gates:
        input uppergateIn;
        output uppergateOut;
        input macIn;
        output macOut;
        input agentIn;
        output agentOut;
}
