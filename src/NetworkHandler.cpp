#include "NetworkHandler.h"
#include <thread>

NetworkHandler::NetworkHandler() {}

void NetworkHandler::setVerbose(bool enable) {
	verbose = enable;
}

std::unordered_map<enet_uint16, ENetPeer*>& NetworkHandler::getPeerMap() {
	return peerMap;
}

bool NetworkHandler::initialize() {
	if (initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: ENet is already initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return true;
	}

	if (enet_initialize() == 0) {
		initialized = true;

		if (verbose) dConsole.sendMsg("ENet initialized successfully", MESSAGE_TYPE::NETWORK_VERBOSE);
		return true;
	}

	if (verbose) dConsole.sendMsg("Networking WARNING: Failed to initialize ENet", MESSAGE_TYPE::NETWORK_VERBOSE);
	return false;
}

bool NetworkHandler::shutdown() {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: ENet cannot be shutdown; ENet is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return false;
	}

	initialized = false;
	finished = true;
	netServerThread.join();
	netClientThread.join();

	if (server) {
		enet_host_flush(server);
		enet_host_destroy(server);
	}

	if (client) {
		enet_host_flush(client);
		enet_host_destroy(client);
	}

	clientTrulyConnected = false;

	enet_deinitialize();
	return true;
}

void NetworkHandler::handle(IrrHandling* m) {
	irrNetHandler = m;
	netServerThread = std::thread(netBodyServer, this, m);
	netClientThread = std::thread(netBodyClient, this, m);
}

ENetHost* NetworkHandler::getHost() {
	return server;
}

ENetHost* NetworkHandler::getClient() {
	return client;
}

ENetPeer* NetworkHandler::getPeer() {
	return peer;
}

int NetworkHandler::getPeerState(int peerID) {
	if (!server) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Could not fetch peer state; server is not being hosted", MESSAGE_TYPE::NETWORK_VERBOSE);
		return -1;
	}

	ENetPeer* p = peerMap[peerID];
	if (!p) {
		if (verbose) {
			std::string msg = "Networking WARNING: Peer with ID ";
			msg += std::to_string(peerID);
			msg += " could not be fetched";
			dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
		}
		return -1;
	}

	return (int)p->state;
}

int NetworkHandler::getPeerPing(int peerID) {
	if (!server && !client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Could not fetch peer ping; client is not connected to a server", MESSAGE_TYPE::NETWORK_VERBOSE);
		return -1;
	}

	ENetPeer* p = peerMap[peerID];
	if (!p) {
		if (verbose) {
			std::string msg = "Networking WARNING: Peer with ID ";
			msg += std::to_string(peerID);
			msg += "'s ping could not be fetched";
			dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
		}
		return -1;
	}

	return (int)p->roundTripTime;
}

void NetworkHandler::forceDisconnectClient(int peerID, int reason) {
	if (!server) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Could not kick client; server is not being hosted", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	ENetPeer* p = peerMap[peerID];
	if (!p) {
		if (verbose) {
			std::string msg = "Networking WARNING: Peer with ID ";
			msg += std::to_string(peerID);
			msg += " could not be disconnected";
			dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
		}
		return;
	}

	if (verbose) {
		std::string msg = "Peer with ID ";
		msg += std::to_string(peerID);
		msg += " disconnected for reason code ";
		msg += std::to_string(reason);
		dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
	}

	enet_peer_disconnect(p,reason);
}

void netBodyServer(NetworkHandler* n, IrrHandling* m) {
	// Server Network Loop
	std::mutex lock;
	ENetEvent event;

	while (!n->finished) {
		if (!n->initialized || !(n->getHost())) {
			std::this_thread::yield();
			continue;
		}

		if (n->getPeer() && n->getHost()) {
			lock.lock();
			if (enet_host_service(n->getHost(), &event, 1000) > 0)
				m->addEventTask(true, event);
			lock.unlock();
		}
	}
}

using namespace std::chrono_literals;

void netBodyClient(NetworkHandler* n, IrrHandling* m) {
	// Client Network Loop
	std::mutex lock;
	ENetEvent event;

	while (!n->finished) {
		if (!n->initialized || !(n->getClient())) {
			continue;
		}

		if (n->getPeer() && n->clientTrulyConnected) {
			lock.lock();
			if (enet_host_service(n->getClient(), &event, 1000) > 0)
				m->addEventTask(false, event);
			lock.unlock();
		}
		else if (!n->clientTrulyConnected) {
			std::this_thread::sleep_for(50ms);
		}
	}
}

void NetworkHandler::hostServer(std::string ip, int port, int maxClients, int maxChannels) {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Host cannot be created; ENet is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (server) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Host cannot be created; there is an ongoing host", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	ENetAddress address;
	address.host = ENET_HOST_ANY; // Hosts on localhost by default
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	server = enet_host_create(&address, maxClients, maxChannels, 0, 0);

	char ipString[64];

	if (!server || enet_address_get_host_ip(&server->address, ipString, sizeof(ipString)) != 0) {
		std::string ms = "";
		ms = "Networking WARNING: Server could not be hosted on IP ";
		ms += ipString;
		if (verbose) dConsole.sendMsg(ms.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);

		sol::protected_function onHostFail = (*lua)["NetworkServer"]["OnHostFail"];
		if (onHostFail.valid()) {
			sol::protected_function_result result = onHostFail();
		}
		else if (verbose) {
			dConsole.sendMsg("Networking WARNING: The server failed to host but NetworkServer.OnHostFail is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
		}

		if (verbose) {
			dConsole.sendMsg("Networking WARNING: The server is being hosted but NetworkServer.OnHosted is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
		}

		if (server)
			enet_host_destroy(server);
		server = nullptr;
		return;
	}

	if (verbose) {
		std::string msg = "Hosting server on ";
		msg += ipString;
		msg += " for ";
		msg += std::to_string(maxClients);
		msg += " clients with ";
		msg += std::to_string(maxChannels);
		msg += " channels";
		dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
	}

	sol::protected_function onHosted = (*lua)["NetworkServer"]["OnHosted"];
	if (onHosted.valid()) {
		sol::protected_function_result result = onHosted();
	}
	else if (verbose) {
		dConsole.sendMsg("Networking WARNING: The server is being hosted but NetworkServer.OnHosted is not declared", MESSAGE_TYPE::NETWORK_VERBOSE);
	}
}

bool NetworkHandler::stopHosting() {
	if (server) {
		enet_host_flush(server);
		enet_host_destroy(server);
		if (verbose) dConsole.sendMsg("Server hosting closed", MESSAGE_TYPE::NETWORK_VERBOSE);
		return true;
	}
	else {
		if (verbose) dConsole.sendMsg("Networking WARNING: NetworkServer.StopHosting was called but there is no server to stop hosting", MESSAGE_TYPE::NETWORK_VERBOSE);
		return false;
	}
}

void NetworkHandler::setBandwidthLimit(int incoming, int outgoing) {
	if (!server) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Setting bandwidth limits must be done after hosting a server", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	enet_host_bandwidth_limit(server, incoming, outgoing); // in bytes per second
}

bool NetworkHandler::isHosting() {
	return server;
}

int NetworkHandler::getServerIP() {
	return server ? server->address.host : 0;
}

int NetworkHandler::getPort() {
	return server ? server->address.port : 0;
}

void NetworkHandler::setUseRangeEncoder(bool enable) {
	if (server && enable)
		enet_host_compress_with_range_coder(server);
	else if (server)
		enet_host_compress(server, nullptr);

	if (client && enable)
		enet_host_compress_with_range_coder(client);
	else if (client)
		enet_host_compress(client, nullptr);

	if (!server  && !client && verbose) dConsole.sendMsg("Networking WARNING: Enabling/disabling compression must be done after being connected to a server", MESSAGE_TYPE::NETWORK_VERBOSE);
	else if (verbose) {
		if (enable) dConsole.sendMsg("Range-encoding compressor enabled", MESSAGE_TYPE::NETWORK_VERBOSE);
		else dConsole.sendMsg("Range-encoding compressor disabled", MESSAGE_TYPE::NETWORK_VERBOSE);
	}
}

// Client
bool NetworkHandler::createClient(int outgoing, int channels) {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to create a client was made but networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return false;
	}

	if (client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Client could not be created; current client should be destroyed first", MESSAGE_TYPE::NETWORK_VERBOSE);
		return false;
	}

	client = enet_host_create(NULL, outgoing, channels, 0, 0);
	if (!client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Client could not be created", MESSAGE_TYPE::NETWORK_VERBOSE);
		return false;
	}

	if (verbose) {
		std::string msg = "Created client with ";
		msg += std::to_string(outgoing);
		msg += " outgoing connection(s) and ";
		msg += std::to_string(channels);
		msg += " channel(s) accessible";
		dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
	}
	return true;
}

void NetworkHandler::connectClient(std::string ad, int port, int channels) {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to connect a client was made but networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Client could not connect to address; client is not created", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (peer) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Client could not connect to address; client is already connected to a server", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	ENetAddress address;
	enet_address_set_host(&address, ad.c_str());
	address.port = port;

	if (verbose) {
		std::string msg = "Client attempting to connect to ";
		msg += ad;
		msg += ":";
		msg += std::to_string(port);
		dConsole.sendMsg(msg.c_str(), MESSAGE_TYPE::NETWORK_VERBOSE);
	}
	
	peer = enet_host_connect(client, &address, channels, 0);

	bool doVerbose = verbose;
	std::thread connectThread([this, address, channels, doVerbose]() {
		if (!peer) {
			if (doVerbose) dConsole.sendMsg("Networking WARNING: Failed to create peer connection", MESSAGE_TYPE::NETWORK_VERBOSE);

			sol::protected_function f = (*lua)["NetworkClient"]["OnConnectFail"];
			irrNetHandler->addLuaTask(f, sol::table());
			return;
		}
		else {
			ENetEvent event;
			if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
				if (doVerbose) dConsole.sendMsg("Client connected to server", MESSAGE_TYPE::NETWORK_VERBOSE);

				clientTrulyConnected = true;

				sol::protected_function f = (*lua)["NetworkClient"]["OnConnect"];
				irrNetHandler->addLuaTask(f, sol::table());
			}
			else {
				if (doVerbose) dConsole.sendMsg("Client failed to connect to server", MESSAGE_TYPE::NETWORK_VERBOSE);

				sol::protected_function f = (*lua)["NetworkClient"]["OnConnectFail"];
				irrNetHandler->addLuaTask(f, sol::table());
			}
		}
	});

	connectThread.detach();
}

void NetworkHandler::disconnectClient() {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to disconnect the client was made but networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to disconnect the client was made but there is no available client", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!peer) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to disconnect the client was made but the client is not connected to a server", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}
	
	if (verbose) dConsole.sendMsg("Disconnecting client from server", MESSAGE_TYPE::NETWORK_VERBOSE);
	enet_peer_disconnect(peer, 0);
	clientTrulyConnected = false;
}

bool NetworkHandler::destroyClient() {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to destroy the client was made but networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return false;
	}

	if (!client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to destroy the client was made but there is no available client", MESSAGE_TYPE::NETWORK_VERBOSE);
		return false;
	}

	enet_host_flush(client);
	enet_host_destroy(client);
	return true;
}

bool NetworkHandler::isClientConnected() {
	return peer && client && peer->state == ENET_PEER_STATE_CONNECTED;
}

void NetworkHandler::sendPacketToServer(const Packet& p, int channel, bool tcp) {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to send a packet to the server was made but networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Packet could not be sent to the server; not connected to a server", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (server && !client) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Cannot send packet to self; server is this application instance", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!p.p) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Packet could not be sent to the server; packet is invalid", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	irrNetHandler->addPacketToSend(PacketToSend(p.p, channel, -1, tcp));
}

void NetworkHandler::sendPacketToPeer(int peerID, const Packet& p, int channel, bool tcp) {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to send a packet to a peer was made but networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!client && !server) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Packet could not be sent to peer; not connected to a server", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!p.p) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Packet could not be sent to peer; packet is invalid", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	irrNetHandler->addPacketToSend(PacketToSend(p.p, channel, peerID, tcp));
}

void NetworkHandler::sendPacketToAll(const Packet& p, int channel, bool tcp) {
	if (!initialized) {
		if (verbose) dConsole.sendMsg("Networking WARNING: A call to send a packet to all was made but networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!client && !server) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Packet could not be sent to all; not connected to a server", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	if (!p.p) {
		if (verbose) dConsole.sendMsg("Networking WARNING: Packet could not be sent to all; packet is invalid", MESSAGE_TYPE::NETWORK_VERBOSE);
		return;
	}

	irrNetHandler->addPacketToSend(PacketToSend(p.p, -1, -1, tcp));
}

std::unordered_map<enet_uint16, ENetPeer*> NetworkHandler::getPeers() {
	return peerMap;
}
