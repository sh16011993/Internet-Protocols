// Putting all the include statements
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<fstream>
#include<cmath>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-flow-classifier.h"

//#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

//typedef uint32_t int32;
//typedef uint16_t int16;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TCP Analysis");

typedef struct metrics{
	float throughput;
	float afct;
}metr;

metr ex1[3];
metr ex21[3], ex22[3];
metr ex3[3];
metr ex41[3], ex42[3];
metr ex51[3], ex52[3];

void e1(int k){
	RngSeedManager::SetRun(k);
	std::string rate = "1Gbps";	
	Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpBic"));

	//NS_LOG_INFO("Creating Nodes");
	NodeContainer nodes;
	nodes.Create(6); //Creating 6 nodes

	//Creating the P2P Node Containers
	NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
	NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));
	NodeContainer n3n4 = NodeContainer(nodes.Get(3), nodes.Get(4));
	NodeContainer n3n5 = NodeContainer(nodes.Get(3), nodes.Get(5));
	NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));

	//Installing Internet Stack
	InternetStackHelper internet;
	internet.Install(nodes);

	//NS_LOG_INFO("Creating Channels");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue(rate));
	NetDeviceContainer d0d2 = p2p.Install(n0n2);
	NetDeviceContainer d1d2 = p2p.Install(n1n2);
	NetDeviceContainer d3d4 = p2p.Install(n3n4);
	NetDeviceContainer d3d5 = p2p.Install(n3n5);
	NetDeviceContainer d2d3 = p2p.Install(n2n3);

	//Assigning IP Addresses
	Ipv4AddressHelper ipv4;

	ipv4.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);
	ipv4.SetBase("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);
	ipv4.SetBase("192.168.3.0", "255.255.255.0");
	Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);
	ipv4.SetBase("192.168.4.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i4 = ipv4.Assign(d3d4);
	ipv4.SetBase("192.168.5.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i5 = ipv4.Assign(d3d5);
		
	//NS_LOG_INFO("Enabling Static Global Routing");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	//NS_LOG_INFO("Performing Experiment 1");

	//NS_LOG_INFO("Run 1");

 	// Create a BulkSendApplication and install it on node 0
 	uint16_t port = 9; 
	uint64_t maxBytes = 524288000; // 500 MB in bytes
	BulkSendHelper source1("ns3::TcpSocketFactory", InetSocketAddress(i3i4.GetAddress(1), port));
	// Set the amount of data to send in bytes.  Zero is unlimited.
   	source1.SetAttribute("MaxBytes", UintegerValue (maxBytes));
	ApplicationContainer sourceApps1 = source1.Install(nodes.Get(0));

	sourceApps1.Start(Seconds(1.0));
	sourceApps1.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps1 = sink1.Install(nodes.Get(4));
	sinkApps1.Start(Seconds(1.0));
	sinkApps1.Stop(Seconds(45.0));

	// Now, do the actual simulation.
	//NS_LOG_INFO("Run Simulation.");

	// Flow monitor
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor = flowHelper.InstallAll();

	Simulator::Stop(Seconds(60.0));
	Simulator::Run();

	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
	{
	     //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	     //NS_LOG_DEBUG ("Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
             /*std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";           
	     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
	     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";*/
//	     std::cout << "  Time first packet was transmitted:  " << i->second.timeFirstTxPacket << "\n";
//	     std::cout << "  Time last packet was received:  " << i->second.timeLastRxPacket << "\n";
	     /*std::cout << "  Time first packet was transmitted (in seconds):  " << (i->second.timeFirstTxPacket).GetSeconds() << "\n";
	     std::cout << "  Time last packet was received (in seconds):  " << (i->second.timeLastRxPacket).GetSeconds() << "\n";
	     std::cout << "  Average Flow Completion Time:   " << (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds()) << " seconds\n";
             std::cout << "  Throughput: " << (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024)  << " Mbps\n";*/

	     //Write to file for Flow 1 and Flow 2 (Throughput and Average Flow Completion Time) for all the runs
	     if(i->first == 1){
		ex1[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex1[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	}

	//std::cout<<"Before FlowMonitor Run\n";
	flowMonitor->SerializeToXmlFile("tcpanalysis.xml", true, true);
	//NS_LOG_INFO("Done.");
	
	// Reporting Data Transferred	
	Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps1.Get(0));
	//std::cout<<"Total Bytes Received (Exp 1): "<<sink->GetTotalRx()<<std::endl;

	//std::cout<<"Before Simulator Destroy\n";
	Simulator::Destroy();	
}

void e2(int k){
	RngSeedManager::SetRun(k);
	std::string rate = "1Gbps";	
	Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpBic"));

	//NS_LOG_INFO("Creating Nodes");
	NodeContainer nodes;
	nodes.Create(6); //Creating 6 nodes

	//Creating the P2P Node Containers
	NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
	NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));
	NodeContainer n3n4 = NodeContainer(nodes.Get(3), nodes.Get(4));
	NodeContainer n3n5 = NodeContainer(nodes.Get(3), nodes.Get(5));
	NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));

	//Installing Internet Stack
	InternetStackHelper internet;
	internet.Install(nodes);

	//NS_LOG_INFO("Creating Channels");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue(rate));
	NetDeviceContainer d0d2 = p2p.Install(n0n2);
	NetDeviceContainer d1d2 = p2p.Install(n1n2);
	NetDeviceContainer d3d4 = p2p.Install(n3n4);
	NetDeviceContainer d3d5 = p2p.Install(n3n5);
	NetDeviceContainer d2d3 = p2p.Install(n2n3);

	//Assigning IP Addresses
	Ipv4AddressHelper ipv4;

	ipv4.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);
	ipv4.SetBase("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);
	ipv4.SetBase("192.168.3.0", "255.255.255.0");
	Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);
	ipv4.SetBase("192.168.4.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i4 = ipv4.Assign(d3d4);
	ipv4.SetBase("192.168.5.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i5 = ipv4.Assign(d3d5);
		
	//NS_LOG_INFO("Enabling Static Global Routing");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	//NS_LOG_INFO("Performing Experiment 1");

	//NS_LOG_INFO("Run 1");

 	uint16_t port = 9; 
	uint64_t maxBytes = 524288000; // 500 MB in bytes

	// Set 1 of BulkSender and Packet Sink Applications
 	// Create a BulkSendApplication and install it on node 0
	BulkSendHelper source1("ns3::TcpSocketFactory", InetSocketAddress(i3i4.GetAddress(1), port));
   	source1.SetAttribute("MaxBytes", UintegerValue (maxBytes)); 	// Set the amount of data to send in bytes.  Zero is unlimited.
	ApplicationContainer sourceApps1 = source1.Install(nodes.Get(0));
	sourceApps1.Start(Seconds(1.0));
	sourceApps1.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps1 = sink1.Install(nodes.Get(4));
	sinkApps1.Start(Seconds(1.0));
	sinkApps1.Stop(Seconds(45.0));

	// Set 2 of BulkSender and Packet Sink Applications
	// Create a BulkSendApplication and install it on node 0
	BulkSendHelper source2("ns3::TcpSocketFactory", InetSocketAddress(i3i5.GetAddress(1), port));
   	source2.SetAttribute("MaxBytes", UintegerValue (maxBytes));	// Set the amount of data to send in bytes.  Zero is unlimited.
	ApplicationContainer sourceApps2 = source2.Install(nodes.Get(1));
	sourceApps2.Start(Seconds(1.0));
	sourceApps2.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps2 = sink2.Install(nodes.Get(5));
	sinkApps2.Start(Seconds(1.0));
	sinkApps2.Stop(Seconds(45.0));


	// Flow monitor
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor = flowHelper.InstallAll();

	// Now, do the actual simulation.
	//NS_LOG_INFO("Run Simulation.");
	Simulator::Stop(Seconds(60.0));
	Simulator::Run();

	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
	{
	     //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	     //NS_LOG_DEBUG ("Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
             /*std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";           
	     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
	     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";*/
//	     std::cout << "  Time first packet was transmitted:  " << i->second.timeFirstTxPacket << "\n";
//	     std::cout << "  Time last packet was received:  " << i->second.timeLastRxPacket << "\n";
	     /*std::cout << "  Time first packet was transmitted (in seconds):  " << (i->second.timeFirstTxPacket).GetSeconds() << "\n";
	     std::cout << "  Time last packet was received (in seconds):  " << (i->second.timeLastRxPacket).GetSeconds() << "\n";
	     std::cout << "  Average Flow Completion Time:   " << (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds()) << " seconds\n";
             std::cout << "  Throughput: " << (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024)  << " Mbps\n";*/

	     //Write to file for Flow 1 and Flow 2 (Throughput and Average Flow Completion Time) for all the runs
	     if(i->first == 1){
		ex21[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex21[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	     if(i->first == 2){
		ex22[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex22[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	}

	//std::cout<<"Before FlowMonitor Run\n";
	flowMonitor->SerializeToXmlFile("tcpanalysis.xml", true, true);
	//NS_LOG_INFO("Done.");
	
	// Reporting Data Transferred	
	Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps1.Get(0));
	//std::cout<<"Total Bytes Received (Exp 2): "<<sink->GetTotalRx()<<std::endl;

	sink = DynamicCast<PacketSink>(sinkApps2.Get(0));
	//std::cout<<"Total Bytes Received (Exp 2): "<<sink->GetTotalRx()<<std::endl;

	//std::cout<<"Before Simulator Destroy\n";
	Simulator::Destroy();	
}

void e3(int k){
	RngSeedManager::SetRun(k);
	std::string rate = "1Gbps";	
	Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpDctcp"));

	//NS_LOG_INFO("Creating Nodes");
	NodeContainer nodes;
	nodes.Create(6); //Creating 6 nodes

	//Creating the P2P Node Containers
	NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
	NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));
	NodeContainer n3n4 = NodeContainer(nodes.Get(3), nodes.Get(4));
	NodeContainer n3n5 = NodeContainer(nodes.Get(3), nodes.Get(5));
	NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));

	//Installing Internet Stack
	InternetStackHelper internet;
	internet.Install(nodes);

	//NS_LOG_INFO("Creating Channels");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue(rate));
	NetDeviceContainer d0d2 = p2p.Install(n0n2);
	NetDeviceContainer d1d2 = p2p.Install(n1n2);
	NetDeviceContainer d3d4 = p2p.Install(n3n4);
	NetDeviceContainer d3d5 = p2p.Install(n3n5);
	NetDeviceContainer d2d3 = p2p.Install(n2n3);

	//Assigning IP Addresses
	Ipv4AddressHelper ipv4;

	ipv4.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);
	ipv4.SetBase("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);
	ipv4.SetBase("192.168.3.0", "255.255.255.0");
	Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);
	ipv4.SetBase("192.168.4.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i4 = ipv4.Assign(d3d4);
	ipv4.SetBase("192.168.5.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i5 = ipv4.Assign(d3d5);
		
	//NS_LOG_INFO("Enabling Static Global Routing");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	//NS_LOG_INFO("Performing Experiment 1");

	//NS_LOG_INFO("Run 1");

 	// Create a BulkSendApplication and install it on node 0
 	uint16_t port = 9; 
	uint64_t maxBytes = 524288000; // 500 MB in bytes
	BulkSendHelper source1("ns3::TcpSocketFactory", InetSocketAddress(i3i4.GetAddress(1), port));
	// Set the amount of data to send in bytes.  Zero is unlimited.
   	source1.SetAttribute("MaxBytes", UintegerValue (maxBytes));
	ApplicationContainer sourceApps1 = source1.Install(nodes.Get(0));

	sourceApps1.Start(Seconds(1.0));
	sourceApps1.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps1 = sink1.Install(nodes.Get(4));
	sinkApps1.Start(Seconds(1.0));
	sinkApps1.Stop(Seconds(45.0));

	// Now, do the actual simulation.
	//NS_LOG_INFO("Run Simulation.");

	// Flow monitor
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor = flowHelper.InstallAll();

	Simulator::Stop(Seconds(60.0));
	Simulator::Run();

	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
	{
	     //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	     //NS_LOG_DEBUG ("Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
             /*std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";           
	     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
	     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";*/
//	     std::cout << "  Time first packet was transmitted:  " << i->second.timeFirstTxPacket << "\n";
//	     std::cout << "  Time last packet was received:  " << i->second.timeLastRxPacket << "\n";
	     /*std::cout << "  Time first packet was transmitted (in seconds):  " << (i->second.timeFirstTxPacket).GetSeconds() << "\n";
	     std::cout << "  Time last packet was received (in seconds):  " << (i->second.timeLastRxPacket).GetSeconds() << "\n";
	     std::cout << "  Average Flow Completion Time:   " << (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds()) << " seconds\n";
             std::cout << "  Throughput: " << (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024)  << " Mbps\n";*/

	     if(i->first == 1){
		ex3[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex3[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	}

	//std::cout<<"Before FlowMonitor Run\n";
	flowMonitor->SerializeToXmlFile("tcpanalysis.xml", true, true);
	//NS_LOG_INFO("Done.");
	
	// Reporting Data Transferred	
	Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps1.Get(0));
	//std::cout<<"Total Bytes Received (Exp 1): "<<sink->GetTotalRx()<<std::endl;

	//std::cout<<"Before Simulator Destroy\n";
	Simulator::Destroy();	
}

void e4(int k){
	RngSeedManager::SetRun(k);
	std::string rate = "1Gbps";	
	Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpDctcp"));

	//NS_LOG_INFO("Creating Nodes");
	NodeContainer nodes;
	nodes.Create(6); //Creating 6 nodes

	//Creating the P2P Node Containers
	NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
	NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));
	NodeContainer n3n4 = NodeContainer(nodes.Get(3), nodes.Get(4));
	NodeContainer n3n5 = NodeContainer(nodes.Get(3), nodes.Get(5));
	NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));

	//Installing Internet Stack
	InternetStackHelper internet;
	internet.Install(nodes);

	//NS_LOG_INFO("Creating Channels");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue(rate));
	NetDeviceContainer d0d2 = p2p.Install(n0n2);
	NetDeviceContainer d1d2 = p2p.Install(n1n2);
	NetDeviceContainer d3d4 = p2p.Install(n3n4);
	NetDeviceContainer d3d5 = p2p.Install(n3n5);
	NetDeviceContainer d2d3 = p2p.Install(n2n3);

	//Assigning IP Addresses
	Ipv4AddressHelper ipv4;

	ipv4.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);
	ipv4.SetBase("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);
	ipv4.SetBase("192.168.3.0", "255.255.255.0");
	Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);
	ipv4.SetBase("192.168.4.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i4 = ipv4.Assign(d3d4);
	ipv4.SetBase("192.168.5.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i5 = ipv4.Assign(d3d5);
		
	//NS_LOG_INFO("Enabling Static Global Routing");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	//NS_LOG_INFO("Performing Experiment 1");

	//NS_LOG_INFO("Run 1");

 	uint16_t port = 9; 
	uint64_t maxBytes = 524288000; // 500 MB in bytes

	// Set 1 of BulkSender and Packet Sink Applications
 	// Create a BulkSendApplication and install it on node 0
	BulkSendHelper source1("ns3::TcpSocketFactory", InetSocketAddress(i3i4.GetAddress(1), port));
   	source1.SetAttribute("MaxBytes", UintegerValue (maxBytes)); 	// Set the amount of data to send in bytes.  Zero is unlimited.
	ApplicationContainer sourceApps1 = source1.Install(nodes.Get(0));
	sourceApps1.Start(Seconds(1.0));
	sourceApps1.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps1 = sink1.Install(nodes.Get(4));
	sinkApps1.Start(Seconds(1.0));
	sinkApps1.Stop(Seconds(45.0));

	// Set 2 of BulkSender and Packet Sink Applications
	// Create a BulkSendApplication and install it on node 0
	BulkSendHelper source2("ns3::TcpSocketFactory", InetSocketAddress(i3i5.GetAddress(1), port));
   	source2.SetAttribute("MaxBytes", UintegerValue (maxBytes));	// Set the amount of data to send in bytes.  Zero is unlimited.
	ApplicationContainer sourceApps2 = source2.Install(nodes.Get(1));
	sourceApps2.Start(Seconds(1.0));
	sourceApps2.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps2 = sink2.Install(nodes.Get(5));
	sinkApps2.Start(Seconds(1.0));
	sinkApps2.Stop(Seconds(45.0));


	// Flow monitor
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor = flowHelper.InstallAll();

	// Now, do the actual simulation.
	//NS_LOG_INFO("Run Simulation.");
	Simulator::Stop(Seconds(60.0));
	Simulator::Run();

	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
	{
	     //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	     //NS_LOG_DEBUG ("Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
             /*std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";           
	     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
	     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";*/
//	     std::cout << "  Time first packet was transmitted:  " << i->second.timeFirstTxPacket << "\n";
//	     std::cout << "  Time last packet was received:  " << i->second.timeLastRxPacket << "\n";
	     /*std::cout << "  Time first packet was transmitted (in seconds):  " << (i->second.timeFirstTxPacket).GetSeconds() << "\n";
	     std::cout << "  Time last packet was received (in seconds):  " << (i->second.timeLastRxPacket).GetSeconds() << "\n";
	     std::cout << "  Average Flow Completion Time:   " << (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds()) << " seconds\n";
             std::cout << "  Throughput: " << (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024)  << " Mbps\n";*/

	     //Write to file for Flow 1 and Flow 2 (Throughput and Average Flow Completion Time) for all the runs
	     if(i->first == 1){
		ex41[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex41[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	     if(i->first == 2){
		ex42[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex42[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	}

	//std::cout<<"Before FlowMonitor Run\n";
	flowMonitor->SerializeToXmlFile("tcpanalysis.xml", true, true);
	//NS_LOG_INFO("Done.");
	
	// Reporting Data Transferred	
	Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps1.Get(0));
	//std::cout<<"Total Bytes Received (Exp 2): "<<sink->GetTotalRx()<<std::endl;

	sink = DynamicCast<PacketSink>(sinkApps2.Get(0));
	//std::cout<<"Total Bytes Received (Exp 2): "<<sink->GetTotalRx()<<std::endl;

	//std::cout<<"Before Simulator Destroy\n";
	Simulator::Destroy();	
}

void e5(int k){
	RngSeedManager::SetRun(k);
	std::string rate = "1Gbps";	

	//NS_LOG_INFO("Creating Nodes");
	NodeContainer nodes;
	nodes.Create(6); //Creating 6 nodes

	//Creating the P2P Node Containers
	NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
	NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));
	NodeContainer n3n4 = NodeContainer(nodes.Get(3), nodes.Get(4));
	NodeContainer n3n5 = NodeContainer(nodes.Get(3), nodes.Get(5));
	NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));

	//Installing Internet Stack
	InternetStackHelper internet;
	internet.Install(nodes);

	//NS_LOG_INFO("Creating Channels");
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue(rate));
	NetDeviceContainer d0d2 = p2p.Install(n0n2);
	NetDeviceContainer d1d2 = p2p.Install(n1n2);
	NetDeviceContainer d3d4 = p2p.Install(n3n4);
	NetDeviceContainer d3d5 = p2p.Install(n3n5);
	NetDeviceContainer d2d3 = p2p.Install(n2n3);

	//Assigning IP Addresses
	Ipv4AddressHelper ipv4;

	ipv4.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);
	ipv4.SetBase("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);
	ipv4.SetBase("192.168.3.0", "255.255.255.0");
	Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);
	ipv4.SetBase("192.168.4.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i4 = ipv4.Assign(d3d4);
	ipv4.SetBase("192.168.5.0", "255.255.255.0");
	Ipv4InterfaceContainer i3i5 = ipv4.Assign(d3d5);
		
	//NS_LOG_INFO("Enabling Static Global Routing");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	//NS_LOG_INFO("Performing Experiment 1");

	//NS_LOG_INFO("Run 1");

 	uint16_t port = 9; 
	uint64_t maxBytes = 524288000; // 500 MB in bytes

	// Set 1 of BulkSender and Packet Sink Applications
	TypeId tid = TypeId::LookupByName("ns3::TcpBic");
	std::stringstream nodeId;
	nodeId<<n0n2.Get(0)->GetId();
	std::string specificNode = "/NodeList/"+nodeId.str()+"/$ns3::TcpL4Protocol/SocketType";
	Config::Set(specificNode, TypeIdValue (tid));
	Socket::CreateSocket(n0n2.Get(0), TcpSocketFactory::GetTypeId());
	
 	// Create a BulkSendApplication and install it on node 0
	BulkSendHelper source1("ns3::TcpSocketFactory", InetSocketAddress(i3i4.GetAddress(1), port));
   	source1.SetAttribute("MaxBytes", UintegerValue (maxBytes)); 	// Set the amount of data to send in bytes.  Zero is unlimited.
	ApplicationContainer sourceApps1 = source1.Install(nodes.Get(0));
	sourceApps1.Start(Seconds(1.0));
	sourceApps1.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps1 = sink1.Install(nodes.Get(4));
	sinkApps1.Start(Seconds(1.0));
	sinkApps1.Stop(Seconds(45.0));

	// Set 2 of BulkSender and Packet Sink Applications
	tid = TypeId::LookupByName("ns3::TcpDctcp");
	nodeId<<n1n2.Get(0)->GetId();
	specificNode = "/NodeList/"+nodeId.str()+"/$ns3::TcpL4Protocol/SocketType";
	Config::Set(specificNode, TypeIdValue(tid));
	Socket::CreateSocket(n1n2.Get(0), TcpSocketFactory::GetTypeId());

	// Create a BulkSendApplication and install it on node 1
	BulkSendHelper source2("ns3::TcpSocketFactory", InetSocketAddress(i3i5.GetAddress(1), port));
   	source2.SetAttribute("MaxBytes", UintegerValue (maxBytes));	// Set the amount of data to send in bytes.  Zero is unlimited.
	ApplicationContainer sourceApps2 = source2.Install(nodes.Get(1));
	sourceApps2.Start(Seconds(1.0));
	sourceApps2.Stop(Seconds(45.0));
 
	//Create a PacketSinkApplication and install it on node 4
	//Using port 9 on the client as well
	PacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
	ApplicationContainer sinkApps2 = sink2.Install(nodes.Get(5));
	sinkApps2.Start(Seconds(1.0));
	sinkApps2.Stop(Seconds(45.0));

	// Flow monitor
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor = flowHelper.InstallAll();

	// Now, do the actual simulation.
	//NS_LOG_INFO("Run Simulation.");
	Simulator::Stop(Seconds(60.0));
	Simulator::Run();

	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
	std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
	{
	     //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	     //NS_LOG_DEBUG ("Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")");
             /*std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";           
	     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
	     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";*/
//	     std::cout << "  Time first packet was transmitted:  " << i->second.timeFirstTxPacket << "\n";
//	     std::cout << "  Time last packet was received:  " << i->second.timeLastRxPacket << "\n";
	     /*std::cout << "  Time first packet was transmitted (in seconds):  " << (i->second.timeFirstTxPacket).GetSeconds() << "\n";
	     std::cout << "  Time last packet was received (in seconds):  " << (i->second.timeLastRxPacket).GetSeconds() << "\n";
	     std::cout << "  Average Flow Completion Time:   " << (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds()) << " seconds\n";
             std::cout << "  Throughput: " << (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024)  << " Mbps\n";*/

	     //Write to file for Flow 1 and Flow 2 (Throughput and Average Flow Completion Time)
	     if(i->first == 1){
		ex51[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex51[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	     if(i->first == 2){
		ex52[k-1].throughput = (i->second.rxBytes * 8.0) / (((i->second.timeLastRxPacket).GetSeconds()-(i->second.timeFirstRxPacket).GetSeconds()) * 1024 * 1024);
		ex52[k-1].afct = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
	     }
	}

	//std::cout<<"Before FlowMonitor Run\n";
	flowMonitor->SerializeToXmlFile("tcpanalysis.xml", true, true);
	//NS_LOG_INFO("Done.");
	
	// Reporting Data Transferred	
	Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps1.Get(0));
	//std::cout<<"Total Bytes Received (Exp 2): "<<sink->GetTotalRx()<<std::endl;

	sink = DynamicCast<PacketSink>(sinkApps2.Get(0));
	//std::cout<<"Total Bytes Received (Exp 2): "<<sink->GetTotalRx()<<std::endl;

	//std::cout<<"Before Simulator Destroy\n";
	Simulator::Destroy();	
}

int main(int argc, char* argv[]){
	int i;	
	float avg, sd, diff, varsum, variance;		
	//Open file or writing
	FILE *fp = fopen("tcp_sshekha4.csv", "w");	
	fprintf(fp, "exp,r1_s1,r2_s1,r3_s1,avg_s1,std_s1,unit_s1,r1_s2,r2_s2,r3_s2,avg_s2,std_s2,unit_s2,\n");
	
	// Experiment needs to be run 3 times
	//Experiment 1
	//printf("Experiment 1 Results\n");
	for(i=0; i<3; i++){
		//std::cout<<"Run "<<i+1<<": "<<std::endl;
		e1(i+1);
	}
	//Experiment 2
	//printf("Experiment 2 Results\n");
	for(i=0; i<3; i++){
		//std::cout<<"Run "<<i+1<<": "<<std::endl;
		e2(i+1);
	}
	//Experiment 3
	//printf("Experiment 3 Results\n");
	for(i=0; i<3; i++){
		//std::cout<<"Run "<<i+1<<": "<<std::endl;
		e3(i+1);
	}
	//Experiment 4
	//printf("Experiment 4 Results\n");
	for(i=0; i<3; i++){
		//std::cout<<"Run "<<i+1<<": "<<std::endl;
		e4(i+1);
	}
	//Experiment 5
	//printf("Experiment 5 Results\n");
	for(i=0; i<3; i++){
		//std::cout<<"Run "<<i+1<<": "<<std::endl;
		e5(i+1);
	}
	
	//Write all results to file
	//Writing Exp 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "th_1,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex1[i].throughput);
		avg+=ex1[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex1[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs\n");

	//Writing Exp 2
	//Flow 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "th_2,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex21[i].throughput);
		avg+=ex21[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex21[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs,");

	//Flow 2
	avg = sd = 0; varsum = 0;
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex22[i].throughput);
		avg+=ex22[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex22[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs\n");

	//Writing Exp 3
	avg = sd = 0; varsum = 0;
	fprintf(fp, "th_3,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex3[i].throughput);
		avg+=ex3[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex3[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs\n");

	//Writing Exp 4
	//Flow 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "th_4,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex41[i].throughput);
		avg+=ex41[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex41[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs,");

	//Flow 2
	avg = sd = 0; varsum = 0;
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex42[i].throughput);
		avg+=ex42[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex42[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs\n");

	//Writing Exp 5
	//Flow 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "th_5,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex51[i].throughput);
		avg+=ex51[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex51[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs,");

	//Flow 2
	avg = sd = 0; varsum = 0;
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex52[i].throughput);
		avg+=ex52[i].throughput;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex52[i].throughput - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "Mpbs\n");

	//  Now, writing Average Flow Completion Time
	//Writing Exp 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "afct_1,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex1[i].afct);
		avg+=ex1[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex1[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec\n");

	//Writing Exp 2
	//Flow 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "afct_2,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex21[i].afct);
		avg+=ex21[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex21[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec,");

	//Flow 2
	avg = sd = 0; varsum = 0;
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex22[i].afct);
		avg+=ex22[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex22[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec\n");

	//Writing Exp 3
	avg = sd = 0; varsum = 0;
	fprintf(fp, "afct_3,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex3[i].afct);
		avg+=ex3[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex3[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec\n");

	//Writing Exp 4
	//Flow 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "afct_4,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex41[i].afct);
		avg+=ex41[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex41[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec,");

	//Flow 2
	avg = sd = 0; varsum = 0;
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex42[i].afct);
		avg+=ex42[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex42[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec\n");

	//Writing Exp 5
	//Flow 1
	avg = sd = 0; varsum = 0;
	fprintf(fp, "afct_5,");
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex51[i].afct);
		avg+=ex51[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex51[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec,");

	//Flow 2
	avg = sd = 0; varsum = 0;
	for(i=0; i<3; i++){
		fprintf(fp, "%f,", ex52[i].afct);
		avg+=ex52[i].afct;
	}
	avg/=3;
	for(i=0; i<3; i++){
	     diff=ex52[i].afct - avg;
	     varsum+=pow(diff,2);
        }
	variance = varsum/(float)3;
	sd = sqrt(variance);
	//Write avg and sd to file
	fprintf(fp, "%f,", avg);
	fprintf(fp, "%f,", sd);
	fprintf(fp, "sec\n");

	return 0;
}
