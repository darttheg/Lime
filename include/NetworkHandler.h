#pragma once

#include "IrrHandling.h"
#include <thread>

#include "Packet.h"
#include "mutex"

class NetworkHandler
{
public:
	NetworkHandler();

	bool initialize(); // Initialize ENet handling
	bool shutdown(); // Shutdown networking system, disconnect from servers, stop hosting etc.
	void handle(IrrHandling* m); // Handle events if initialized
	void setVerbose(bool enable); // Enable/disable networking warnings

	std::unordered_map<enet_uint16, ENetPeer*>& getPeerMap();

	// Server/Hosting
	void hostServer(std::string ip, int port, int maxClients, int maxChannels); // Calls NetworkServer.OnHosted on completion, NetworkServer.OnHostFail on fail
	bool stopHosting();
	void setBandwidthLimit(int incoming, int outgoing);
	bool isHosting();
	int getServerIP();
	int getPort();
	void setUseRangeEncoder(bool enable);
	ENetHost* getHost();
	ENetHost* getClient();
	ENetPeer* getPeer();
	int getPeerState(int peerID);
	int getPeerPing(int peerID);
	void forceDisconnectClient(int peerID, int reasonCode);

	// Client
	bool createClient(int outgoing, int channels);
	void connectClient(std::string address, int port, int channels);
	void disconnectClient();
	bool destroyClient();
	bool isClientConnected();

	// Packets
	void sendPacketToServer(const Packet& p, int channel, bool tcp);
	void sendPacketToPeer(int peerID, const Packet& p, int channel, bool tcp);
	void sendPacketToAll(const Packet& p, int channel, bool tcp);

	bool initialized = false;
	bool verbose = false;
	bool finished = false;
	bool clientTrulyConnected = false;

	std::unordered_map<enet_uint16, ENetPeer*> getPeers();
private:
	// Server
	ENetHost* server = nullptr;

	// Client
	ENetHost* client = nullptr;
	ENetPeer* peer = nullptr;

	std::mutex netLock;

	std::thread netServerThread;
	std::thread netClientThread;

	IrrHandling* irrNetHandler = nullptr;

	std::unordered_map<enet_uint16, ENetPeer*> peerMap;
};

void netBodyServer(NetworkHandler* n, IrrHandling* m);
void netBodyClient(NetworkHandler* n, IrrHandling* m);