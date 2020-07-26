#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/gnuplot.h"
#include "ns3/gnuplot-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Assignment 4 Wireless 02");

Ptr<PacketSink> sink;

int main ()
{
	int k;
	//Configure default TCP agent to Westwood, Veno and Vegas respectively at successive iterations
	for(k = 1; k <= 3; k++)
	{
		std::string s1, s2, fairness_plot_file, throughput_plot_file;
		if(k == 1)
		{
			Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
			std::cout << "Westwood\n\n";
			s1 = "02.FairnessIndex(Westwood)";
			fairness_plot_file = "02.FairnessIndex(Westwood).plt";
			s2 = "02.Throughput(Westwood)";
			throughput_plot_file = "02.Throughput(Westwood).plt";
		}
		else if(k == 2)
		{
			Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVeno::GetTypeId ()));
			std::cout << "Veno\n\n";
			fairness_plot_file = "02.FairnessIndex(Veno).plt";
			s1 = "02.FairnessIndex(Veno)";
			throughput_plot_file = "02.Throughput(Veno).plt";
			s2 = "02.Throughput(Veno)";
		}
		else if(k == 3)
		{
			Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVegas::GetTypeId ()));
			std::cout << "Vegas\n\n";
			fairness_plot_file = "02.FairnessIndex(Vegas).plt";
			s1 = "02.FairnessIndex(Vegas)";
			throughput_plot_file = "02.Throughput(Vegas).plt";
			s2 = "02.Throughput(Vegas)";
		}
		
		//Create and set parameters for graph of Fairness Vs Packet Size
		Gnuplot plot_a (s1);

		plot_a.SetTitle ("Fairness Index Vs Packet Size");
		plot_a.SetTerminal ("png");
		plot_a.SetLegend ("Packet Size", "Fairness Index");
		plot_a.AppendExtra ("set xrange [40:1500]");

		Gnuplot2dDataset fairness_dataset;
		fairness_dataset.SetTitle ("Fairness");
		fairness_dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

		//Create and set parameters for graph of Throughput Vs Packet Size
		Gnuplot plot_b (s2);

		plot_b.SetTitle ("Throughput Vs Packet Size");
		plot_b.SetTerminal ("png");
		plot_b.SetLegend ("Packet Size", "Throughput");
		plot_b.AppendExtra ("set xrange [40:1500]");

		Gnuplot2dDataset throughput_dataset;
		throughput_dataset.SetTitle ("Throughput (Kbps)");
		throughput_dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

		//Array of packet sizes provided in question
		uint32_t packet_size[12] = {40,44,48,52,60,250,300,552,576,628,1420,1500};
		uint32_t j;

		for(j = 0; j < 12 ; j++)
		{
			std::cout << "Packet Size : " << packet_size[j] << "\n";

			Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (packet_size[j]));

			WifiMacHelper wifiMac_AP,wifiMac_ST;
			WifiHelper wifiHelper;
			wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);

			//Setup Wifi channel
			YansWifiChannelHelper wifiChannel1, wifiChannel2;
			wifiChannel1.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
			wifiChannel1.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));
			wifiChannel2.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
			wifiChannel2.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

			YansWifiPhyHelper wifiPhy1 = YansWifiPhyHelper::Default ();
			wifiPhy1.SetChannel (wifiChannel1.Create ());
			wifiPhy1.SetErrorRateModel ("ns3::YansErrorRateModel");
			wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue ("HtMcs7"),"ControlMode", StringValue ("HtMcs0"));

			YansWifiPhyHelper wifiPhy2 = YansWifiPhyHelper::Default ();
			wifiPhy2.SetChannel (wifiChannel2.Create ());
			wifiPhy2.SetErrorRateModel ("ns3::YansErrorRateModel");
			wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue ("HtMcs7"),"ControlMode", StringValue ("HtMcs0"));

			//Create nodes for source, destination and base stations
			Ptr<Node> n0 = CreateObject<Node> ();
			Ptr<Node> bs1 = CreateObject<Node> ();
			Ptr<Node> bs2 = CreateObject<Node> ();
			Ptr<Node> n1 = CreateObject<Node> ();

			
			NodeContainer Container_bs1bs2 = NodeContainer(bs1,bs2);
			NodeContainer Cont_n0bs1bs2n1 = NodeContainer(n0,n1,bs1,bs2);

			//Create channels and setup channel attributes as per question
			//Setup Droptail queues with queue size equal to bandwidth delay product
			PointToPointHelper p2pHelper;
			p2pHelper.SetChannelAttribute ("Delay", StringValue ("100ms"));
			p2pHelper.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
			p2pHelper.SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize ("125000B")));
			NetDeviceContainer Dev_bs1bs2 = p2pHelper.Install (Container_bs1bs2);

			NetDeviceContainer Dev_bs1, Dev_bs2, Dev_n0, Dev_n1;
			
			Ssid ssid = Ssid ("wireless-network");
			wifiMac_AP.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));
			Dev_bs1 = wifiHelper.Install (wifiPhy1, wifiMac_AP, bs1);
			Dev_bs2 = wifiHelper.Install (wifiPhy2, wifiMac_AP, bs2);

			wifiMac_ST.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid));
			Dev_n0 = wifiHelper.Install (wifiPhy1, wifiMac_ST, n0);
			Dev_n1 = wifiHelper.Install (wifiPhy2, wifiMac_ST, n1);

			//Assign positioning schemes and mobility models
			MobilityHelper mobility;
			
			Ptr<ListPositionAllocator> positionAlloc1 = CreateObject<ListPositionAllocator> ();
			positionAlloc1->Add (Vector (0.0, 0.0, 0.0));
			positionAlloc1->Add (Vector (5.0, 0.0, 0.0));
			mobility.SetPositionAllocator (positionAlloc1);
			mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
			mobility.Install (n0);
			mobility.Install (bs1);

			Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator> ();
			positionAlloc2->Add (Vector (10.0, 0.0, 0.0));
			positionAlloc2->Add (Vector (15.0, 0.0, 0.0));
			mobility.SetPositionAllocator (positionAlloc2);
			mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
			mobility.Install (bs2);
			mobility.Install (n1);

			InternetStackHelper stack;
			stack.Install (Cont_n0bs1bs2n1);

			//Assign IPv4 address to all node devices
			Ipv4AddressHelper address;
			Ipv4InterfaceContainer interface_bs1,interface_bs2,interface_n0,interface_n1;

			address.SetBase ("10.3.1.0", "255.255.255.0");
			interface_n0 = address.Assign (Dev_n0);
			interface_bs1 = address.Assign (Dev_bs1);

			address.SetBase ("10.3.2.0", "255.255.255.0");
			interface_bs2 = address.Assign (Dev_bs2);
			interface_n1 = address.Assign (Dev_n1);

			address.SetBase ("10.3.3.0", "255.255.255.0");
			Ipv4InterfaceContainer baseinterface = address.Assign (Dev_bs1bs2);

			std::cout <<"n0 : " << interface_n0.GetAddress(0) << "\t";
			std::cout <<"bs1 : " << interface_bs1.GetAddress(0) << "\t";
			std::cout <<"bs2 : " << interface_bs2.GetAddress(0) << "\t";
			std::cout <<"n1 : " << interface_n1.GetAddress(0) << "\n\n";

			Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

			//Setup a TCP sink at destination node1 
			PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
			ApplicationContainer sinkApp = sinkHelper.Install (n1);
			sink = StaticCast<PacketSink> (sinkApp.Get(0));

			//Create a TCP socket and setup a source application at node0
			OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (interface_n1.GetAddress (0), 9)));
			server.SetAttribute ("PacketSize", UintegerValue (packet_size[j]));
			server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
			server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
			server.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mbps")));
			ApplicationContainer serverApp = server.Install (n0);

			sinkApp.Start (Seconds (0.0));
			serverApp.Start (Seconds (10.0));

			FlowMonitorHelper flowmonitor;
			Ptr<FlowMonitor> monitor = flowmonitor.InstallAll();
			
			Simulator::Stop (Seconds(11.0));
			Simulator::Run ();

			double temp1 = 0, temp2 = 0;
			monitor->CheckForLostPackets ();
			Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonitor.GetClassifier ());
			std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
			
			//Using Flow Moniter to calculate Fairness Index and Throughput
			for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
			{
				Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

				std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ") Source Port :" << t.sourcePort << " Destination Port :" << t.destinationPort << "\n";
				std::cout << "  Tx Bytes\t\t:" << i->second.txBytes << " bytes \n";
				std::cout << "  Rx Bytes\t\t:" << i->second.rxBytes << " bytes\n";
				double time = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
				std::cout << "  Transmission Time\t:" << time << "s\n";
				double throughput = ((i->second.rxBytes * 8.0) / time)/1000;
				std::cout << "  Throughput\t\t:" << throughput  << " Kbps\n\n";

				temp1 += throughput;
				temp2 += throughput * throughput ;
			}

			double FairnessIndex = (temp1 * temp1)/ (2 * temp2) ;
			std :: cout << "Average Throughput: " << temp1/2 << " Kbps" << "\n";
			std :: cout << " FairnessIndex:	" << FairnessIndex << "\n\n";
			std :: cout << "\n\n";
			fairness_dataset.Add (packet_size[j], FairnessIndex);
			throughput_dataset.Add(packet_size[j],temp1/2);
			Simulator::Destroy ();
		}

		//Provide dataset to gnuplot file for graph plotting
		plot_a.AddDataset (fairness_dataset);
		std :: ofstream plot_file1 (fairness_plot_file.c_str());
		plot_a.GenerateOutput (plot_file1);
		plot_file1.close ();

		plot_b.AddDataset (throughput_dataset);
		std :: ofstream plot_file2 (throughput_plot_file.c_str());
		plot_b.GenerateOutput (plot_file2);
		plot_file2.close ();
	}
	return(0);
}
