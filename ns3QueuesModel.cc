/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* TODO:
 *
 * First part:
 *
 *  - Implement  the  scenario  in  ns-3 from part 2  
 *  - Use  the  P2P  communication  for  all  the  links.  
 *  - For  the  generation  of traffic, you  can  use  the  UDP  Client  and  Server  Application.  
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
 *  - Replace each P2P link with a bus using CSMA/CD; do not change the datarate or delay.
 *  - Run the simulation and compare the results with the one obtained with the P2P. Try to explain the difference you see.
 *  - Run the simulation but this time use the custom PRNG that you implemented in Part I to generate the exponential packet size 
 *    and the exponential time between packets. Which differences do you observe?
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NS3QUEUESMODEL");

//void TcPacketsInQueue(){}

//static void received_msg(){}

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}