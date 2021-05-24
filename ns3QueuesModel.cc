/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* TODO:
 *
 * First part:
 *
 *  - (DONE) Implement  the  scenario  in  ns-3 from part 2  
 *  - (DONE) Use  the  P2P  communication  for  all  the  links.  
 *  - (DONE) For  the  generation  of traffic, you  can  use  the  UDP  Client  and  Server  Application.  
 *      * The  Server  application  will  need  to  be modified so  that  it  reply  only  to  a  fraction of p=0.7 of 
 *        the  messages  it  receives  while  the  rest  is forwarded to the Router.
 *      * You can assume that the Router will simply  drop all the received messages without  taking  any  
 *        action(i.e.,  we  are  not  interested in  simulating the  request messages  after  they leave the Router).
 *
 *  Second part:
 *
 *  Run the simulation and measure:
 *  1. The average number of request packets queued at the link between "g" and the "server" (g -> Server)
 *  2. The average "queueing delay" and the "total average delay" for the request packets traversing the link "f -> g" 
 *
 *  Third part:
 *  - Compare the results you get from the simulation with the one you obtained in Part II.
 *  - Measure the delay between A and the Server and between the Server and A.
 *  - (Create a new file for this one) Replace each P2P link with a bus using CSMA/CD; do not change the datarate or delay
 *  - Run the simulation and compare the results with the one obtained with the P2P. Try to explain the difference you see.
 *  - Run the simulation but this time use the custom PRNG that you implemented in Part I to generate the exponential packet size 
 *    and the exponential time between packets. Which differences do you observe?
 */

 #include <iostream>
 #include <fstream>
 #include <string>
 #include <cassert>
 
 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/flow-monitor-helper.h"
 #include "ns3/ipv4-global-routing-helper.h"
 #include "ns3/netanim-module.h"

 /* Network

             S
  A____E____ |
            \|
	B____      G ___ R ___ Internet 
		   \   / |
			   F   | 
	C____/     D

 */


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NS3QUEUESMODEL");

// TODO: void TcPacketsInQueue(){}

// Forwards a packet received by the Server back to the source of the message with probability p=0.7 or to a third node R with probability 1-p.
static void received_msg (Ptr<Socket> socket1, Ptr<Socket> socket2, Ptr<const Packet> p, const Address &srcAddress , const Address &dstAddress)
{
	std::cout << "::::: A packet received at the Server! Time:   " << Simulator::Now ().GetSeconds () << std::endl;
	
	Ptr<UniformRandomVariable> rand=CreateObject<UniformRandomVariable>();
	
	if(rand->GetValue(0.0,1.0)<=0.7){
		std::cout << "::::: Transmitting from Server to Router   "  << std::endl;
		socket1->Send (Create<Packet> (p->GetSize ()));
	}
	else{
		std::cout << "::::: Transmitting from GW to Controller   "  << std::endl;
		socket2->SendTo(Create<Packet> (p->GetSize ()),0,srcAddress);
	}
}

static void GenerateTraffic (Ptr<Socket> socket, Ptr<ExponentialRandomVariable> randomSize,	Ptr<ExponentialRandomVariable> randomTime)
{
	uint32_t pktSize = randomSize->GetInteger (); //Get random value for packet size
	std::cout << "::::: A packet is generate at Node "<< socket->GetNode ()->GetId () << " with size " << pktSize <<" bytes ! Time:   " << Simulator::Now ().GetSeconds () << std::endl;
	
	// We make sure that the message is at least 12 bytes. The minimum length of the UDP header. We would get error otherwise.
	if(pktSize<12){
		pktSize=12;
	}
	
	socket->Send (Create<Packet> (pktSize));

	Time pktInterval = Seconds(randomTime->GetValue ()); //Get random value for next packet generation time 
	Simulator::Schedule (pktInterval, &GenerateTraffic, socket, randomSize, randomTime); //Schedule next packet generation
}

int main (int argc, char *argv[])
{
   // Users may find it convenient to turn on explicit debugging
   // for selected modules; the below lines suggest how to do this
  #if 0 
    LogComponentEnable ("SimpleGlobalRoutingExample", LOG_LEVEL_INFO);
  #endif
 
  // Set up some default values for the simulation.  Use the 
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (100));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("50kb/s"));

  bool enableFlowMonitor = true;

  // Create nodes for the links
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (9);

  /*
  Node index:
  0: A
  1: E
  2: G
  3: S (Server)
  4: B
  5: F
  6: C
  7: D 
  8: R (Router)
  */

  NodeContainer nAnE = NodeContainer(nodes.Get(0), nodes.Get(1));
  NodeContainer nEnG = NodeContainer(nodes.Get(1), nodes.Get(2));

  NodeContainer nGnS = NodeContainer(nodes.Get(2), nodes.Get(3));
  NodeContainer nGnR = NodeContainer(nodes.Get(2), nodes.Get(8));

  NodeContainer nBnF = NodeContainer(nodes.Get(4), nodes.Get(5));
  NodeContainer nCnF = NodeContainer(nodes.Get(6), nodes.Get(5));

  NodeContainer nFnG = NodeContainer(nodes.Get(5), nodes.Get(2));
  NodeContainer nDnG = NodeContainer(nodes.Get(7), nodes.Get(2));

  InternetStackHelper internet;
  internet.Install (nodes);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  // Constructing a point to point link
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
 
  NetDeviceContainer dAdE = pointToPoint.Install(nAnE);
  NetDeviceContainer dEdG = pointToPoint.Install(nEnG);
  NetDeviceContainer dBdF = pointToPoint.Install(nBnF);

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.5ms"));

  NetDeviceContainer dCdF = pointToPoint.Install(nCnF);

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));

  NetDeviceContainer dDdG = pointToPoint.Install(nDnG);

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));

  NetDeviceContainer dFdG = pointToPoint.Install(nFnG);
  NetDeviceContainer dGdR = pointToPoint.Install(nGnR);

  pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
  pointToPoint.SetQueue("ns3::DropTailQueue");

  NetDeviceContainer dGdS = pointToPoint.Install(nGnS);

  // Later, we add IP addresses.
  // FIXME: Check if IP address are OK 
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer iAiE = ipv4.Assign(dAdE);

  ipv4.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer iEiG = ipv4.Assign(dEdG);

  ipv4.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer iBiF = ipv4.Assign(dBdF);

  ipv4.SetBase("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer iCiF = ipv4.Assign(dCdF);

  ipv4.SetBase("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer iFiG = ipv4.Assign(dFdG);

  ipv4.SetBase("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer iDiG = ipv4.Assign(dDdG);

  ipv4.SetBase("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer iGiR = ipv4.Assign(dGdR);

  ipv4.SetBase("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer iGiS = ipv4.Assign(dGdS);

  // Creates router nodes, initialize routing database and set up the routing tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // TODO: Add AsciiTracerHelper

  NS_LOG_INFO ("Create Applications.");
  //
  // Creates a UdpServer application on node Server (S).
  //
  uint16_t port_number = 9;  
  ApplicationContainer server_apps;
  UdpServerHelper serverS (port_number);
  server_apps.Add(serverS.Install(nodes.Get (3)));

  Ptr<UdpServer> S1 = serverS.GetServer();


  // We Initialize the sockets responsable for transmitting messages

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

  //Transmission Server (S)-> Router (R)
  Ptr<Socket> source1 = Socket::CreateSocket (nodes.Get (3), tid);
  InetSocketAddress remote1 = InetSocketAddress (iGiR.GetAddress (1), port_number); // FIXME: Check index
  source1->Connect (remote1);

  //Transmission Server (S) -> Client (A or B)
  Ptr<Socket> source2 = Socket::CreateSocket (nodes.Get (3), tid);

  S1->TraceConnectWithoutContext ("RxWithAddresses", MakeBoundCallback (&received_msg, source1, source2));

  server_apps.Start (Seconds (1.0));
  server_apps.Stop (Seconds (10.0));

  //
  // Creates a UdpServer application on node A,B to receive the reply from the server.
  //
  UdpServerHelper server (port_number);
  server_apps.Add(server.Install(nodes.Get (0)));  // FIXME: Change index of node
  server_apps.Add(server.Install(nodes.Get (1)));  // FIXME: Change index of node

  // ####Using Sockets to generate traffic at node A and B  (i.e., exponential payload and inter-transmission time)####
  // You can in alternative install two Udp Client applications 
 
  Ptr<Socket> sourceA = Socket::CreateSocket (nodes.Get (0), tid);  // FIXME: Change index of node
  InetSocketAddress remote = InetSocketAddress (iGiS.GetAddress (1), port_number); // FIXME: Change index in GetAddress
  sourceA->Connect (remote);

  Ptr<Socket> sourceB= Socket::CreateSocket (nodes.Get (1), tid);  // FIXME: Change index of node
  sourceB->Connect (remote);
  // TODO: Add sockets 

  //Mean inter-transmission time
  double mean = 0.002; //2 ms
  Ptr<ExponentialRandomVariable> randomTime = CreateObject<ExponentialRandomVariable> ();
  randomTime->SetAttribute ("Mean", DoubleValue (mean));
  // TODO: Add arrival rates

  //Mean packet time
  mean = 100; // 100 Bytes
  Ptr<ExponentialRandomVariable> randomSize = CreateObject<ExponentialRandomVariable> ();
  randomSize->SetAttribute ("Mean", DoubleValue (mean));
  // TODO: Add departure rates

  Simulator::ScheduleWithContext (sourceA->GetNode ()->GetId (), Seconds (2.0), &GenerateTraffic, sourceA, randomSize, randomTime);
  Simulator::ScheduleWithContext (sourceB->GetNode ()->GetId (), Seconds (2.0), &GenerateTraffic, sourceB, randomSize, randomTime);
  // TODO: Add more sources


  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("global-routing.tr"));
  pointToPoint.EnablePcapAll ("global-routing");

  /* 

  AnimationInterface anim ("example.xml"); // The Filename for the trace file used by the Animator
  anim.EnablePacketMetadata (true);	// if true enables writing the packet metadata to the XML trace file

  //Helper function to set Constant Position for a given node.
  // Parameters
  // n	Ptr to the node
  // x	X co-ordinate of the node
  // y	Y co-ordinate of the node
  anim.SetConstantPosition (nodes.Get(0), 100, -100); // FIXME: Change index of node
  anim.SetConstantPosition (nodes.Get(1), 120, -100); // FIXME: Change index of node
  anim.SetConstantPosition (nodes.Get(2), 140, -90);  // FIXME: Change index of node
  anim.SetConstantPosition (nodes.Get(3), 140, -110); // FIXME: Change index of node
  anim.SetConstantPosition (nodes.Get(4), 100, -80);  // FIXME: Change index of node
  // TODO: Add more SetConstantPosition for each node

  */

   // Flow Monitor
  FlowMonitorHelper flowmonHelper;
  if (enableFlowMonitor)
  {
    flowmonHelper.InstallAll ();
  }
 
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (10));
  Simulator::Run ();
  NS_LOG_INFO ("Done.");
 
  if (enableFlowMonitor)
  {
    flowmonHelper.SerializeToXmlFile ("example.flowmon", false, false);
  }
 
   Simulator::Destroy ();
   return 0;
}