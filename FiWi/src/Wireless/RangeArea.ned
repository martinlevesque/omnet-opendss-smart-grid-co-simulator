package FiWi.Wireless;

simple RangeArea
{
    parameters:
        @display("i=misc/cloud");
        @labels(node);
        
        double bitrate;
    
    gates:
        input clients[];
    
    
}