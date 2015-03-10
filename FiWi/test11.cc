void FiWiTrafGen::sendChargingDeadlineRequest(bool justArrivedHome, bool wantV2G, const std::string& nodeId, double p_deadline)
{
	// FULL
	if (OpenDSS::Instance().getStateOfChargeOf(nodeId) >= 1 && ! wantV2G)
	{
		return;
	}

	static long chargingDeadlineMsgid = 0;
	++chargingDeadlineMsgid;

	char msgname[300];
	sprintf(msgname, "charging-deadline-request-%d-%ld", getId(), chargingDeadlineMsgid);
	EV << "Generating packet  `" << msgname << "'\n";

	Ieee802Ctrl* etherctrl = new Ieee802Ctrl();
	etherctrl->setSsap(localSAP);
	etherctrl->setDsap(remoteSAP);

	MACAddress dest = aggregatorOf(myAddress);

	etherctrl->setDest(dest);
	etherctrl->setSrc(myAddress);

	ChargingDeadlineRequestMessage* datapacket = new ChargingDeadlineRequestMessage(msgname, IEEE802CTRL_DATA);

	OpenDSS::Instance().setPEVBatteryKwHPerHour(nodeId, this->PEVBatteryKwPerHour);

	datapacket->setNodeId(nodeId.c_str());
	datapacket->setBatteryCapacity(this->PEVBatteryCapacity);
	datapacket->setBatteryKwPerHour(this->PEVBatteryKwPerHour);
	datapacket->setBatteryDepthOfDischarge(0);

	datapacket->setBatteryStateOfCharge(OpenDSS::Instance().getStateOfChargeOf(nodeId)); // 0..1
	datapacket->setCustomerID("customer id");
	datapacket->setVehicleID("vehicule id");
	datapacket->setBaseLoad(OpenDSS::Instance().getCurrentLoadOf(myAddress));
	datapacket->setWantV2G(wantV2G);

	double deadline = p_deadline;

	datapacket->setDeadline(deadline);

	string httpMessage = msgChargingDeadlineRequestPacket(datapacket);

	long lengthMessage = httpMessage.size() + this->TCPHeaderSize() + this->IPv6HeaderSize();
	datapacket->setByteLength(lengthMessage);

	datapacket->setControlInfo(etherctrl);

	EtherFrame *ethframe = new EthernetIIFrame(msgname);
	ethframe->setDest((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getDest());
	ethframe->setSrc((dynamic_cast<Ieee802Ctrl*>(datapacket->getControlInfo()))->getSrc());
	ethframe->setKind(FIWI_NODE_TYPE_ANY);
	ethframe->setPktType(FIWI_TRAF_PKT_TYPE_SMART_GRID_CONTROL);
	ethframe->setHost(ethframe->getSrc().str().c_str());

	ethframe->setTrafficClass(1);

	ethframe->setIsDOS(isDOS);

	ethframe->encapsulate(datapacket);

	ethframe->setControlInfo(datapacket->getControlInfo()->dup());

	send(ethframe, "out");
}