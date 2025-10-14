#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/random-variable-stream.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Lab1Part1");

int
main (int argc, char *argv[])
{
    uint32_t nClients = 1;
    uint32_t nPackets = 1;

    CommandLine cmd (__FILE__);
    cmd.AddValue ("nClients", "Number of client nodes", nClients);
    cmd.AddValue ("nPackets", "Number of packets per client", nPackets);
    cmd.Parse (argc, argv);

    if (nClients > 5 || nPackets > 5)
    {
        NS_LOG_ERROR ("Error: Max clients and packets are 5.");
        return 1;
    }

    Time::SetResolution (Time::NS);
    
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer serverNode;
    serverNode.Create (1);

    NodeContainer clientNodes;
    clientNodes.Create (nClients);

    InternetStackHelper stack;
    stack.Install (serverNode);
    stack.Install (clientNodes);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    Ipv4AddressHelper address;
    Ipv4InterfaceContainer serverInterfaces;

    for (uint32_t i = 0; i < nClients; ++i)
    {
        NodeContainer linkNodes (serverNode.Get (0), clientNodes.Get (i));
        NetDeviceContainer linkDevices = pointToPoint.Install (linkNodes);

        std::ostringstream subnet;
        subnet << "10.1." << i + 1 << ".0";
        address.SetBase (subnet.str ().c_str (), "255.255.255.0");
        
        Ipv4InterfaceContainer interfaces = address.Assign (linkDevices);

        if (i == 0)
        {
            serverInterfaces.Add (interfaces.Get (0));
        }
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t serverPort = 15;
    UdpEchoServerHelper echoServer (serverPort);
    ApplicationContainer serverApps = echoServer.Install (serverNode.Get (0));
    serverApps.Start (Seconds (0.0));
    serverApps.Stop (Seconds (20.0));

    Ptr<UniformRandomVariable> randomStart = CreateObject<UniformRandomVariable> ();
    randomStart->SetAttribute ("Min", DoubleValue (2.0));
    randomStart->SetAttribute ("Max", DoubleValue (7.0));

    Ipv4Address serverIp = serverInterfaces.GetAddress (0);

    for (uint32_t i = 0; i < nClients; ++i)
    {
        UdpEchoClientHelper echoClient (serverIp, serverPort);
        echoClient.SetAttribute ("MaxPackets", UintegerValue (nPackets));
        echoClient.SetAttribute ("Interval", StringValue ("1.0s"));
        echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

        ApplicationContainer clientApp = echoClient.Install (clientNodes.Get (i));
        clientApp.Start (Seconds (randomStart->GetValue ()));
        clientApp.Stop (Seconds (20.0));
    }

    Simulator::Stop (Seconds (20.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
