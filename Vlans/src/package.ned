//
// @titlepage
//
// <center><h1>Vlans for OMNeT++ (4.1rc)</h1></center>
// <center>release 0.5b</center>
//
// <h2>Descritpion</h2>
// <p>
// This is a very basic 802.1Q vlan implementation. Currently it provides
// an ethernet interface with 802.1Q support, a StandardVlanHost with
// this interface (the host was actually used for the testing) and a 
// "NetworkConfigurator" that can handle IP assingment. Also a very
// basic implementation of RelayUnit is included and it handles only the mac
// address table (as in INET Framework)
// </p>
// 
// <p>
// Some things from the INET framework needed modification, so instead of changing 
// the original files I just extended them and overiden the functions that need to 
// be different.
// </p>  
//
// 
// <p>
// NOTE: I need this project in order to do traffic differentiation so note that I do
// not have intensions to extend it too much. Anyone though is welcome to contribute
// any extra code/modules/patches or report bugs.
//
// </p> 
//
// 
// Andreas
//

package Vlans;



