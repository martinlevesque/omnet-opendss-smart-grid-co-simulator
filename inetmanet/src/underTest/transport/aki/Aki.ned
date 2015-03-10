package inet.underTest.transport.aki;


simple Aki
{
	parameters:
        int local_port; // if -1, app doesn't bind in UDP
        int dest_port;
        int aki_timer;
        double mess_timer;
        int anfangs_paket;
        int message_length; // (bits)
        double message_freq; // should usually be a random value, e.g. exponential(1)
        double timer_freq;
        string dest_addresses; // list of \IP addresses, separated by spaces
        bool is_Sender;
        int receiversAdressId;
        int num_Clients;
    gates:
        input udpIn;
        output udpOut;
}