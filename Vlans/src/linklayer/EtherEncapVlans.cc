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

#include "EtherEncapVlans.h"

using std::vector;
using std::string;

Define_Module(EtherEncapVlans);

void EtherEncapVlans::initialize(){
	EtherEncap::initialize();
	registerVlanInterfaces(par("vlan_txrate").doubleValue());
}

void EtherEncapVlans::processPacketFromHigherLayer(cPacket *msg)
{
    if (msg->getByteLength() > MAX_ETHERNET_DATA)
        error("packet from higher layer (%d bytes) exceeds maximum Ethernet payload length (%d)", msg->getByteLength(), MAX_ETHERNET_DATA);

    totalFromHigherLayer++;

    // Creates MAC header information and encapsulates received higher layer data
    // with this information and transmits resultant frame to lower layer

    // create Ethernet frame, fill it in from Ieee802Ctrl and encapsulate msg in it
    EV << "Encapsulating higher layer packet `" << msg->getName() <<"' for MAC\n";


    Ieee802Ctrl *etherctrl = dynamic_cast<Ieee802Ctrl*>(msg->removeControlInfo());
    Ieee802VlanCtrl *etherctrl_1q = dynamic_cast<Ieee802VlanCtrl*>(etherctrl);


    int vlan_id=0;
    // Get the vlan id (is any)
    if (etherctrl_1q != NULL){
    	std::string ifname = etherctrl_1q->getIfName();
		int subif = ifname.find('.');
		if (subif!=std::string::npos){
			vlan_id = atoi(ifname.substr(subif+1).c_str());
		}
    }

    /**
     * NOTE: For the following block I tried to have
     * a single pointer to EtherFrame and in case we
     * want d1q initialize as a EtherDot1Frame... BUT
     * some frame members where not initializing correctly...
     * (debuger didn't helped much also == more code but
     * working).
     */
    if (etherctrl_1q == NULL || vlan_id==0) {
    	EtherFrame *frame;
    	EV << "EtherEncapVlans: Encapsulating as normal EtherFrame" <<endl;
    	frame = new EthernetIIFrame();
    	frame->setSrc(etherctrl->getSrc());  // set whatever requested
		frame->setByteLength(ETHER_MAC_FRAME_BYTES);
		frame->setName(msg->getName());
		frame->setDest(etherctrl->getDest());
		((EthernetIIFrame*)frame)->setEtherType(etherctrl->getEtherType());
		EV << "Sending for Transmission: Src: "<<frame->getSrc()<<" --> Dest: "<<frame->getDest()<<endl;

		frame->encapsulate(msg);
		if (frame->getByteLength() < MIN_ETHERNET_FRAME)
			frame->setByteLength(MIN_ETHERNET_FRAME);  // "padding"

		send(frame, "lowerLayerOut");

    }else {
			EthernetDot1QFrame *frame;
			EV << "EtherEncapVlans: Encapsulating as 802.1Q Frame" <<endl;
			frame = new EthernetDot1QFrame();
			frame->setVlanType(VLAN_TYPE_D1Q);
			EV << "Setting vlan type to: "<<VLAN_TYPE_D1Q<<" and etherType to: "<<etherctrl_1q->getEtherType()<<endl;
			frame->setVlanID(vlan_id);
			frame->setByteLength(ETHERNET_HEADER_LEN_D1Q);

			// Set proper MAC for this vlan
			if (etherctrl_1q->getSrc() == MACAddress::UNSPECIFIED_ADDRESS){
				frame->setSrc(vmacs[vlan_id]);
			}

			frame->setName(msg->getName());
			frame->setDest(etherctrl->getDest());
			frame->setEtherType(etherctrl->getEtherType());
			EV << "Sending for Transmission: Src: "<<frame->getSrc()<<" --> Dest: "<<frame->getDest()<<endl;

			frame->encapsulate(msg);
			if (frame->getByteLength() < MIN_ETHERNET_FRAME)
				frame->setByteLength(MIN_ETHERNET_FRAME);  // "padding"

			send(frame, "lowerLayerOut");
    }

    delete etherctrl;


}

void EtherEncapVlans::processFrameFromMAC(EtherFrame *frame)
{
	// TODO: Check MAC since MAC layer is in promisc.
    totalFromMAC++;

    // decapsulate and attach control info
    cPacket *higherlayermsg = frame->decapsulate();

    // add Ieee802Ctrl to packet
    Ieee802VlanCtrl *etherctrl = new Ieee802VlanCtrl();
    etherctrl->setSrc(frame->getSrc());
    etherctrl->setDest(frame->getDest());


    EthernetDot1QFrame * d1qframe = dynamic_cast<EthernetDot1QFrame *>(frame);
    EthernetIIFrame * e2frame = dynamic_cast<EthernetIIFrame *>(frame);

    if (e2frame)
    	etherctrl->setEtherType(e2frame->getEtherType());

    char *interfaceName = new char[strlen(getParentModule()->getFullName())+1+5];
    if (d1qframe){
    	EV <<"Dot1Q Frame arrived: "<<d1qframe->getVlanType()<<endl;
    	sprintf(interfaceName,"%s.%d",getParentModule()->getFullName(),d1qframe->getVlanID());
        etherctrl->setIfName(interfaceName);
        etherctrl->setEtherType(d1qframe->getEtherType());
    }
    else{
    	sprintf(interfaceName,"%s",getParentModule()->getFullName());
    	etherctrl->setIfName(interfaceName);
    }

    delete []interfaceName;

    higherlayermsg->setControlInfo(etherctrl);



    EV << "Decapsulating frame `" << frame->getName() <<"', passing up contained "
          "packet `" << higherlayermsg->getName() << "' to higher layer\n";

    // pass up to higher layers.
    send(higherlayermsg, "upperLayerOut");
    delete frame;
}


void EtherEncapVlans::registerVlanInterfaces(double txrate){

	IInterfaceTable *ift = InterfaceTableAccess().getIfExists();
	if (!ift)
		return;


	cStringTokenizer tok(par("vlans").stringValue());
	vector<string> vlans = tok.asVector();

	for (unsigned int i=0; i<vlans.size(); i++){


		InterfaceEntry *interfaceEntry = new InterfaceEntry();

		// interface name: our module name without special characters ([])
		// +5 for the vlan .[0-4096]
		char *interfaceName = new char[strlen(getParentModule()->getFullName())+1+5];
		char *d=interfaceName;
		for (const char *s=getParentModule()->getFullName(); *s; s++)
			if (isalnum(*s))
				*d++ = *s;
		*d++ = '.';
		for (unsigned int c=0; c<vlans[i].length(); c++)
			*d++ = vlans[i][c];

		*d = '\0';

		interfaceEntry->setName(interfaceName);
		delete [] interfaceName;

		// data rate
		interfaceEntry->setDatarate(txrate);

		// generate a link-layer address to be used as interface token for IPv6
		MACAddress addr = MACAddress::generateAutoAddress();
		interfaceEntry->setMACAddress(addr);
		interfaceEntry->setInterfaceToken(addr.formInterfaceIdentifier());

		// Store mac per vlan interface to do encap later
		vmacs[atoi(vlans[i].c_str())] = addr;


		// MTU: typical values are 576 (Internet de facto), 1500 (Ethernet-friendly),
		// 4000 (on some point-to-point links), 4470 (Cisco routers default, FDDI compatible)
		interfaceEntry->setMtu(1500);

		// capabilities
		interfaceEntry->setMulticast(true);
		interfaceEntry->setBroadcast(true);

		// add
		ift->addInterface(interfaceEntry, this);
	}
}


