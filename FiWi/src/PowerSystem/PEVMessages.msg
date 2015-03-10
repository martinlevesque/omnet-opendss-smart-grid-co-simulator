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

packet PowerSysACKMessage
{
    string type;
}

packet NodeNotificationMessage
{
    string nodeId;
    
    double voltage;
    double load;
}

packet PEVAuthenticationMessage
{
	string nodeId;
	string vehicleID;
	string customerID;
	string pinNumber;
}

packet PEVAuthenticationResponseMessage
{
	string nodeId;
	string vehicleID;
	string customerID;
}

packet PEVControlMessage
{
	string nodeId;
	string vehicleID;
	string customerID;
	string operation;
}

packet SubstationNotificationMessage
{
    string nodeId;
	double totalPowerLoad;
	double totalPowerLosses;
}

packet ChargingDeadlineRequestMessage
{
	string nodeId;
	string vehicleID;
	string customerID;
	
	double deadline;
	double batteryCapacity;
	double batteryKwPerHour;
	double batteryDepthOfDischarge;
	double batteryStateOfCharge;
	double baseLoad;
	bool wantV2G;
}

packet StateOfChargeMessage
{
	string nodeId;
	string vehicleID;
	string customerID;
	
	double batteryCapacity;
	double batteryKwPerHour;
	double batteryDepthOfDischarge;
	double batteryStateOfCharge;
}

packet ChargingDeadlineResponseRow
{
    int status;
	double startTime;
	double endTime;
	string type;
}

packet ChargingDeadlineResponseMessage
{
	string nodeId;
	string vehicleID;
	string customerID;

	ChargingDeadlineResponseRow results[];
}

