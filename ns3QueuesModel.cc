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
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/traffic-control-module.h"

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

uint32_t totalQueuedPackets;
uint32_t totalAmountQueues;

NS_LOG_COMPONENT_DEFINE ("NS3QUEUESMODEL");

void TcPacketsInQueue(QueueDiscContainer qdiscs, Ptr<OutputStreamWrapper> streamTrFile, Ptr<OutputStreamWrapper> streamTxt) {

  uint32_t nQueueDiscs = qdiscs.GetN();
  for (uint32_t i = 0; i < nQueueDiscs; ++i) {
    Ptr<QueueDisc> p = qdiscs.Get(i);
    uint32_t size = p->GetNPackets();
    *streamTrFile->GetStream() << Simulator::Now().GetSeconds() << "\t" << i << " Packets in queue: " << size << std::endl;  //Writes to trace file .tr
    *streamTxt->GetStream() << Simulator::Now().GetSeconds() << "\t" << i << " Packets in queue: " << size << std::endl;     //Writes to text file .txt
    std::cout << Simulator::Now().GetSeconds() << "\t" << i << " Packets in queue: " << size << std::endl;                   //Writes to terminal

    //If queue is from g to Server
    if(i == 0){
      totalQueuedPackets += size;
      totalAmountQueues++;
    } 
  }
}

// Forwards a packet received by the Server back to the source of the message with probability p=0.7 or to a third node R with probability 1-p.
static void received_msg (Ptr<Socket> socket1, Ptr<Socket> socket2, Ptr<const Packet> p, const Address &srcAddress , const Address &dstAddress)
{
	//std::cout << "::::: A packet received at the Server! Time:   " << Simulator::Now ().GetSeconds () << std::endl;
	
	Ptr<UniformRandomVariable> rand=CreateObject<UniformRandomVariable>();
	
	if(rand->GetValue(0.0,1.0)<=0.3){
		//std::cout << "::::: Server Forwards the Message to Router   "  << std::endl;
		socket1->Send (Create<Packet> (p->GetSize ()));
	}
	else{
		//std::cout << "::::: Server Replies to Sender   "  << std::endl;
		socket2->SendTo(Create<Packet> (p->GetSize ()),0,srcAddress);
	}
}

static void GenerateTraffic (Ptr<Socket> socket, Ptr<ExponentialRandomVariable> randomSize,	Ptr<ExponentialRandomVariable> randomTime)
{
	uint32_t pktSize = randomSize->GetInteger (); //Get random value for packet size
	//std::cout << "::::: A packet is generated at Node "<< socket->GetNode ()->GetId () << " with size " << pktSize <<" bytes ! Time:   " << Simulator::Now ().GetSeconds () << std::endl;
	
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

  std::string model = "P2P";

  CommandLine cmd;
  cmd.AddValue ("Model", "Choose \"P2P\" or \"CSMA\"", model);
  
  cmd.Parse(argc, argv);

  //Safety for making sure Model used is correct. P2P used otherwise.
  if(model == "P2P" || model == "CSMA"){}
  else{model = "P2P";}
  std::cout << model << " model is running" << std::endl;

  // Create nodes for the links
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (9);

  double simulationTime = 10; // Seconds
  std::string queueSize = "1000";
  
  int deviceA = 0;
  int deviceB = 1;
  int deviceC = 2;
  int deviceD = 3;
  int nodeE = 4;
  int nodeF = 5;
  int nodeG = 6;
  int Server = 7;
  int Router = 8;

  NodeContainer nAnE = NodeContainer(nodes.Get(deviceA), nodes.Get(nodeE));
  NodeContainer nEnG = NodeContainer(nodes.Get(nodeE), nodes.Get(nodeG));

  NodeContainer nGnS = NodeContainer(nodes.Get(nodeG), nodes.Get(Server));
  NodeContainer nGnR = NodeContainer(nodes.Get(nodeG), nodes.Get(Router));

  NodeContainer nBnF = NodeContainer(nodes.Get(deviceB), nodes.Get(nodeF));
  NodeContainer nCnF = NodeContainer(nodes.Get(deviceC), nodes.Get(nodeF));

  NodeContainer nFnG = NodeContainer(nodes.Get(nodeF), nodes.Get(nodeG));
  NodeContainer nDnG = NodeContainer(nodes.Get(deviceC), nodes.Get(nodeG));

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  // Constructing a point to point link

  PointToPointHelper pointToPoint;
  CsmaHelper csma;
  
  NetDeviceContainer dAdE;
  NetDeviceContainer dEdG;
  NetDeviceContainer dBdF;
  NetDeviceContainer dCdF;
  NetDeviceContainer dDdG;
  NetDeviceContainer dFdG;
  NetDeviceContainer dGdR;
  NetDeviceContainer dGdS;

  if(model == "P2P")
  {
    //PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
    pointToPoint.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("1p"));

    dAdE = pointToPoint.Install(nAnE);
    dEdG = pointToPoint.Install(nEnG);
    dBdF = pointToPoint.Install(nBnF);

    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.5ms"));

    dCdF = pointToPoint.Install(nCnF);

    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));

    dDdG = pointToPoint.Install(nDnG);

    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));

    dFdG = pointToPoint.Install(nFnG);
    dGdR = pointToPoint.Install(nGnR);

    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    
    dGdS = pointToPoint.Install(nGnS);

  }
  else if(model == "CSMA")
  {
    // CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    csma.SetChannelAttribute("Delay", StringValue ("2ms"));
    csma.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("10p"));

    dAdE = csma.Install(nAnE);
    dEdG = csma.Install(nEnG);
    dBdF = csma.Install(nBnF);

    csma.SetChannelAttribute("Delay", StringValue ("0.5ms"));

    dCdF = csma.Install(nCnF);

    csma.SetChannelAttribute("Delay", StringValue ("1ms"));

    dDdG = csma.Install(nDnG);

    csma.SetChannelAttribute("DataRate", StringValue("8Mbps"));
    csma.SetChannelAttribute("Delay", StringValue ("2ms"));

    dFdG = csma.Install(nFnG);
    dGdR = csma.Install(nGnR);

    csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay", StringValue ("2ms"));

    dGdS = csma.Install(nGnS);
  }
  
  InternetStackHelper internet;
  internet.Install (nodes);

  //We have now constructed all the point-to-point links
  //Here we set qdiscs in order to track the traffic between NodeG and the Server
  TrafficControlHelper tch;
  tch.SetRootQueueDisc("ns3::FifoQueueDisc", "MaxSize", StringValue(queueSize + "p"));
  QueueDiscContainer qdiscs_G_to_Server = tch.Install(dGdS);

  // IP addresses
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

  Ptr<OutputStreamWrapper> streamTrFile;
  Ptr<OutputStreamWrapper> streamTxt;
  AsciiTraceHelper asciiTraceHelper;

  if(model == "P2P"){
    streamTrFile = asciiTraceHelper.CreateFileStream("queue_P2P.tr");
    streamTxt = asciiTraceHelper.CreateFileStream("queue_P2P.txt");
  }
  else if (model == "CSMA"){
    streamTrFile = asciiTraceHelper.CreateFileStream("queue_CSMA.tr");
    streamTxt = asciiTraceHelper.CreateFileStream("queue_CSMA.txt");
  }

  for (float t = 1.0; t < simulationTime; t += 0.001) {
    Simulator::Schedule(Seconds(t), &TcPacketsInQueue, qdiscs_G_to_Server, streamTrFile, streamTxt);
  }

  NS_LOG_INFO ("Create Applications.");
  //
  // Creates a UdpServer application on node Server (S).
  //
  uint16_t port_number = 9;  
  ApplicationContainer server_apps;
  UdpServerHelper serverS (port_number);
  server_apps.Add(serverS.Install(nodes.Get (Server)));

  Ptr<UdpServer> S1 = serverS.GetServer();


  // We Initialize the sockets responsable for transmitting messages

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

  //Transmission Server (S)-> Router (R)
  Ptr<Socket> source1 = Socket::CreateSocket (nodes.Get (Server), tid);
  InetSocketAddress remote1 = InetSocketAddress (iGiR.GetAddress (1), port_number);
  source1->Connect (remote1);

  //Transmission Server (S) -> Client (A or B)
  Ptr<Socket> source2 = Socket::CreateSocket (nodes.Get (Server), tid);

  S1->TraceConnectWithoutContext ("RxWithAddresses", MakeBoundCallback (&received_msg, source1, source2));

  server_apps.Start (Seconds (1.0));
  server_apps.Stop (Seconds (10.0));

  //
  // Creates a UdpServer application on node A,B to receive the reply from the server.
  //
  UdpServerHelper server (port_number);
  server_apps.Add(server.Install(nodes.Get (deviceA)));  
  server_apps.Add(server.Install(nodes.Get (deviceB))); 
  //-------------------------------Might Not Be Used-------------------------------------------

  // ####Using Sockets to generate traffic at node A and B  (i.e., exponential payload and inter-transmission time)####
  // You can in alternative install two Udp Client applications 
 
  Ptr<Socket> sourceA = Socket::CreateSocket (nodes.Get (deviceA), tid);  
  InetSocketAddress remote = InetSocketAddress (iGiS.GetAddress (1), port_number);
  sourceA->Connect (remote);

  Ptr<Socket> sourceB= Socket::CreateSocket (nodes.Get (deviceB), tid);  
  sourceB->Connect (remote);

  Ptr<Socket> sourceC= Socket::CreateSocket (nodes.Get (deviceC), tid);  
  sourceC->Connect (remote);

  Ptr<Socket> sourceD= Socket::CreateSocket (nodes.Get (deviceD), tid);  
  sourceD->Connect (remote);

  //Mean inter-transmission time
  double mean_node_AB = 0.002; //2 ms
  Ptr<ExponentialRandomVariable> randomTime_AB = CreateObject<ExponentialRandomVariable> ();
  randomTime_AB->SetAttribute ("Mean", DoubleValue (mean_node_AB));

  double mean_node_C = 0.0005; //0.5 ms
  Ptr<ExponentialRandomVariable> randomTime_C = CreateObject<ExponentialRandomVariable> ();
  randomTime_C->SetAttribute ("Mean", DoubleValue (mean_node_C));
  
  double mean_node_D = 0.001; //1 ms
  Ptr<ExponentialRandomVariable> randomTime_D = CreateObject<ExponentialRandomVariable> ();
  randomTime_D->SetAttribute ("Mean", DoubleValue (mean_node_D));

  //Mean packet time
  double mean = 100; // 100 Bytes
  Ptr<ExponentialRandomVariable> randomSize = CreateObject<ExponentialRandomVariable> ();
  randomSize->SetAttribute ("Mean", DoubleValue (mean));

  Simulator::ScheduleWithContext (sourceA->GetNode ()->GetId (), Seconds (2.0), &GenerateTraffic, sourceA, randomSize, randomTime_AB);
  Simulator::ScheduleWithContext (sourceB->GetNode ()->GetId (), Seconds (2.0), &GenerateTraffic, sourceB, randomSize, randomTime_AB);
  Simulator::ScheduleWithContext (sourceC->GetNode ()->GetId (), Seconds (2.0), &GenerateTraffic, sourceC, randomSize, randomTime_C);
  Simulator::ScheduleWithContext (sourceD->GetNode ()->GetId (), Seconds (2.0), &GenerateTraffic, sourceD, randomSize, randomTime_D);
  
  AsciiTraceHelper ascii;
  if(model == "P2P"){
    pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("global-routing.tr"));
    pointToPoint.EnablePcapAll ("global-routing");
  }
  else if(model == "CSMA"){
    csma.EnableAsciiAll (ascii.CreateFileStream ("global-routing.tr"));
    csma.EnablePcapAll ("global-routing");
  }

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
    flowmonHelper.SerializeToXmlFile ("global-routing.flowmon", false, false);
  }
 
  Simulator::Destroy ();
  std::cout << std::endl << "*** Simulation statistics: ***" << std::endl;
  std::cout << "  Total buffer size: " << totalQueuedPackets << std::endl;
	std::cout << "  Total amount of recordings: " << totalAmountQueues << std::endl;
	std::cout << "  Average buffer size: " << ((double)totalQueuedPackets/(double)totalAmountQueues) << std::endl;
  return 0;
}