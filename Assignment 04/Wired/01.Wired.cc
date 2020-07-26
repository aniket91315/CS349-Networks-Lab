#include <fstream>

#include "ns3/core-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/config-store-module.h"
#include "ns3/gnuplot.h"
#include "ns3/gnuplot-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Assignment 04 01");

// Helper classes and functions for TCP protocol functioning

class MyApp : public Application
{
public:

	MyApp ();
	virtual ~MyApp();

	void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
	virtual void StartApplication (void);
	virtual void StopApplication (void);

	void ScheduleTx (void);
	void SendPacket (void);

	Ptr<Socket>     m_socket;
	Address         m_peer;
	uint32_t        m_packetSize;
	uint32_t        m_nPackets;
	DataRate        m_dataRate;
	EventId         m_sendEvent;
	bool            m_running;
	uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
	m_socket = 0;
}

void MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
	m_socket = socket;
	m_peer = address;
	m_packetSize = packetSize;
	m_nPackets = nPackets;
	m_dataRate = dataRate;
}

void MyApp::StartApplication (void)
{
	m_running = true;
	m_packetsSent = 0;
	m_socket->Bind ();
	m_socket->Connect (m_peer);
	SendPacket ();
}

void MyApp::StopApplication (void)
{
	m_running = false;

	if (m_sendEvent.IsRunning ())
		Simulator::Cancel (m_sendEvent);

	if (m_socket)
		m_socket->Close ();
}

void MyApp::SendPacket (void)
{
	Ptr<Packet> packet = Create<Packet> (m_packetSize);
	m_socket->Send (packet);

	if (++m_packetsSent < m_nPackets)
		ScheduleTx ();
}

void MyApp::ScheduleTx (void)
{
	if (m_running)
	{
		Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
		m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
	}
}

int main()
{
	int k;
	// Configure default TCP agent to Westwood, Veno and Vegas respectively at successive iterations
	for(k = 1; k <= 3; k++)
	{
		std::string s1, s2, fairness_plot_file, throughput_plot_file;
		if(k == 1)
		{
			Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
			std::cout << "Westwood\n\n";
			s1 = "01.FairnessIndex(Westwood)";
			fairness_plot_file = "01.FairnessIndex(Westwood).plt";
			s2 = "01.Throughput(Westwood)";
			throughput_plot_file = "01.Throughput(Westwood).plt";
		}
		else if(k == 2)
		{
			Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVeno::GetTypeId ()));
			std::cout << "Veno\n\n";
			fairness_plot_file = "01.FairnessIndex(Veno).plt";
			s1 = "01.FairnessIndex(Veno)";
			throughput_plot_file = "01.Throughput(Veno).plt";
			s2 = "01.Throughput(Veno)";
		}
		else if(k == 3)
		{
			Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVegas::GetTypeId ()));
			std::cout << "Vegas\n\n";
			fairness_plot_file = "01.FairnessIndex(Vegas).plt";
			s1 = "01.FairnessIndex(Vegas)";
			throughput_plot_file = "01.Throughput(Vegas).plt";
			s2 = "01.Throughput(Vegas)";
		}
		
		// Create and set parameters for graph of Fairness Vs Packet Size
		Gnuplot plot_a (s1);

		plot_a.SetTitle ("Fairness Index Vs Packet Size");
		plot_a.SetTerminal ("png");
		plot_a.SetLegend ("Packet Size", "Fairness Index");
		plot_a.AppendExtra ("set xrange [40:1500]");

		Gnuplot2dDataset fairness_dataset;
		fairness_dataset.SetTitle ("Fairness");
		fairness_dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

		// Create and set parameters for graph of Throughput Vs Packet Size
		Gnuplot plot_b (s2);

		plot_b.SetTitle ("Throughput Vs Packet Size");
		plot_b.SetTerminal ("png");
		plot_b.SetLegend ("Packet Size", "Throughput");
		plot_b.AppendExtra ("set xrange [40:1500]");

		Gnuplot2dDataset throughput_dataset;
		throughput_dataset.SetTitle ("Throughput (Kbps)");
		throughput_dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

		// Array of packet sizes provided in question
		uint32_t packet_size[12] = {40,44,48,52,60,250,300,552,576,628,1420,1500};
		uint32_t num_packets = 200, j;

		for(j = 0; j < 12; j++)
		{
			std::cout << "Packet Size : " << packet_size[j] << "\n";

			// Create nodes for source, destination and routers
			Ptr<Node> n2 = CreateObject<Node> ();
			Ptr<Node> n3 = CreateObject<Node> ();
			Ptr<Node> r1 = CreateObject<Node> ();
			Ptr<Node> r2 = CreateObject<Node> ();

			NodeContainer Container_n2r1 = NodeContainer(n2,r1);
			NodeContainer Container_r1r2 = NodeContainer(r1,r2);
			NodeContainer Container_r2n3 = NodeContainer(r2,n3);
			NodeContainer Cont_n2r1r2n3 = NodeContainer(Container_n2r1,Container_r2n3) ;

			InternetStackHelper stack;
			stack.Install(Cont_n2r1r2n3);

			PointToPointHelper p2pHelper1, p2pHelper2;

			// Create channels and setup channel attributes as per question
			// Setup Droptail queues for routers with queue size equal to bandwidth delay product
			p2pHelper1.SetChannelAttribute("Delay",StringValue("20ms"));
			p2pHelper1.SetDeviceAttribute("DataRate",StringValue("100Mbps"));
			p2pHelper2.SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize ("250000B")));
			NetDeviceContainer Dev_n2r1 = p2pHelper1.Install(Container_n2r1);
			NetDeviceContainer Dev_r2n3 = p2pHelper1.Install(Container_r2n3);

			p2pHelper2.SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize ("62500B")));
			p2pHelper2.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
			p2pHelper2.SetChannelAttribute("Delay",StringValue("50ms"));
			NetDeviceContainer Dev_r1r2 = p2pHelper2.Install(Container_r1r2);

			// Assign IPv4 address to all node devices
			Ipv4AddressHelper address;
			address.SetBase("10.3.1.0","255.255.255.0");
			Ipv4InterfaceContainer interface_n2r1 = address.Assign(Dev_n2r1);

			address.SetBase("10.3.2.0","255.255.255.0");
			Ipv4InterfaceContainer interface_r2n3 = address.Assign(Dev_r2n3);

			address.SetBase("10.3.3.0","255.255.255.0");
			Ipv4InterfaceContainer interface_r1r2 = address.Assign(Dev_r1r2);

			Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

			std::cout <<"n2 : " << interface_n2r1.GetAddress(0) << "\t";
			std::cout <<"r1 : " << interface_n2r1.GetAddress(1) << "\t";
			std::cout <<"r2 : " << interface_r2n3.GetAddress(0) << "\t";
			std::cout <<"n3 : " << interface_r2n3.GetAddress(1) << "\n";
			std::cout << "\n";

			// Setup a TCP sink at destination node3
			uint16_t sinkport = 8080;
			Address sinkAddress(InetSocketAddress(interface_r2n3.GetAddress(1),sinkport));
			PacketSinkHelper packetsinkhelper("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (),sinkport));
			ApplicationContainer sinkApps = packetsinkhelper.Install(n3);
			sinkApps.Start(Seconds(0.));
			sinkApps.Stop(Seconds(20.));

			// Create a TCP socket and setup a source application at node2
			Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(n2,TcpSocketFactory::GetTypeId ());

			Ptr<MyApp> app = CreateObject<MyApp>();
			app->Setup(ns3TcpSocket, sinkAddress, packet_size[j], num_packets, DataRate("100Mbps"));
			n2->AddApplication(app);
			app->SetStartTime(Seconds(1.0));
			app->SetStopTime(Seconds(20.));

			FlowMonitorHelper flowmonitor;
			Ptr<FlowMonitor> monitor = flowmonitor.InstallAll();

			Simulator::Stop (Seconds(20.0));
			Simulator::Run ();

			monitor->CheckForLostPackets();
			Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonitor.GetClassifier ());
			std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
			
			// Using Flow Moniter to calculate Fairness Index and Throughput
			double temp1 = 0, temp2 = 0;
			for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
			{
				Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

				std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ") Source Port :" << t.sourcePort << " Destination Port :" << t.destinationPort << "\n";
				std::cout << "  Tx Bytes:\t\t" << i->second.txBytes << " bytes \n";
				std::cout << "  Rx Bytes:\t\t" << i->second.rxBytes << " bytes\n";
				double time = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
				std::cout << "  Transmission Time:\t" << time << "s\n";
				double throughput = ((i->second.rxBytes * 8.0) / time)/1000;
				std::cout << "  Throughput:\t\t" << throughput  << " Kbps\n\n";

				temp1 += throughput;
				temp2 += throughput * throughput ;
			}
			
			double FairnessIndex = (temp1 * temp1)/ (2 * temp2) ;
			std :: cout << "Average Throughput: " << temp1/2 << " Kbps" << "\n";
			std :: cout << "FairnessIndex:	" << FairnessIndex << std :: endl << "\n";
			fairness_dataset.Add (packet_size[j], FairnessIndex);
			std :: cout << "\n"<< "\n";
			throughput_dataset.Add(packet_size[j],temp1/2);
			Simulator::Destroy ();
		}

		// Provide dataset to gnuplot file for graph plotting
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
