#include "FunctionsImports.h"

namespace Bind {
	void setVerbose(bool v) {
		networkHandler->setVerbose(v);
	}

	bool initializeNetworking() {
		return networkHandler ? networkHandler->initialize() : false;
	}

	void hostServer(std::string ip, int port, int maxClients, int maxChannels) {
		if (!networkHandler || !networkHandler->initialized) {
			if ((networkHandler->verbose)) dConsole.sendMsg("Networking WARNING: Failed to host server: Networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
			return;
		}

		networkHandler->hostServer(ip, port, maxClients, maxChannels);
	}

	bool isHostingServer() {
		return networkHandler ? networkHandler->isHosting() : false;
	}

	int getIP() {
		return networkHandler ? networkHandler->getServerIP() : 0;
	}

	int getPort() {
		return networkHandler ? networkHandler->getPort() : 0;
	}

	void setBandwidthLimits(int incoming, int outgoing) {
		if (networkHandler) networkHandler->setBandwidthLimit(incoming, outgoing); else if (!(networkHandler->initialized)) dConsole.sendMsg("Networking WARNING: Failed to set bandwidth limits: Networking is not initialized", MESSAGE_TYPE::NETWORK_VERBOSE);
	}

	bool shutdownNetworking() {
		return networkHandler ? networkHandler->shutdown() : false;
	}

	bool stopHosting() {
		return networkHandler ? networkHandler->stopHosting() : false;
	}

	bool createClient(int outgoing, int channels) {
		return networkHandler ? networkHandler->createClient(outgoing, channels) : false;
	}

	bool destroyClient() {
		return networkHandler ? networkHandler->destroyClient() : false;
	}

	void connectClient(std::string ad, int port, int channels) {
		if (networkHandler) networkHandler->connectClient(ad, port, channels);
	}

	void disconnectClient() {
		if (networkHandler) networkHandler->disconnectClient();
	}

	bool isClientConnected() {
		return networkHandler ? networkHandler->isClientConnected() : false;
	}

	int getPeerState(int peerID) {
		return networkHandler ? networkHandler->getPeerState(peerID) : -1;
	}

	int getPeerPing(int peerID) {
		return networkHandler ? networkHandler->getPeerState(peerID) : -1;
	}

	void forceDisconnectClient(int peerID, int reason) {
		if (networkHandler) networkHandler->forceDisconnectClient(peerID, reason);
	}

	void sendPacketToServer(int channel, const Packet& p, bool tcp) {
		if (networkHandler) networkHandler->sendPacketToServer(p, channel, tcp);
	}

	void sendPacketToPeer(int peerID, int channel, const Packet& p, bool tcp) {
		if (networkHandler) networkHandler->sendPacketToPeer(peerID, p, channel, tcp);
	}

	void sendPacketToAll(int channel, const Packet& p, bool tcp) {
		if (networkHandler) networkHandler->sendPacketToAll(p, channel, tcp);
	}

	std::string getPeerIP(int peerID) {
		return networkHandler ? networkHandler->getPeerIP(peerID) : "";
	}
}

void bindNetworking() {
	sol::table networkClient = lua->create_named_table("NetworkClient");
	sol::table networkServer = lua->create_named_table("NetworkServer");

	// Client
	networkClient["Initialize"] = &Bind::initializeNetworking;
	networkClient["SetVerbose"] = &Bind::setVerbose;
	networkClient["Create"] = &Bind::createClient;
	networkClient["Destroy"] = &Bind::destroyClient;
	networkClient["Connect"] = &Bind::connectClient;
	networkClient["Disconnect"] = &Bind::disconnectClient;
	networkClient["IsConnected"] = &Bind::isClientConnected;

	networkClient["SendPacketToServer"] = &Bind::sendPacketToServer;

	// Server
	networkServer["Initialize"] = &Bind::initializeNetworking;
	networkServer["SetVerbose"] = &Bind::setVerbose;
	networkServer["Host"] = &Bind::hostServer;
	networkServer["IsHosting"] = &Bind::isHostingServer;
	networkServer["StopHosting"] = &Bind::stopHosting;
	networkServer["GetHostingIP"] = &Bind::getIP;
	networkServer["GetPort"] = &Bind::getPort;
	networkServer["SetBandwidthLimits"] = &Bind::setBandwidthLimits;
	networkServer["Shutdown"] = &Bind::shutdownNetworking;
	networkServer["GetPeerState"] = &Bind::getPeerState;
	networkServer["GetPeerPing"] = &Bind::getPeerPing;
	networkServer["DisconnectPeer"] = &Bind::forceDisconnectClient;
	networkServer["GetPeerIP"] = &Bind::getPeerIP;
	networkServer["SendPacketToPeer"] = &Bind::sendPacketToPeer;
	networkServer["SendPacketToAll"] = &Bind::sendPacketToAll;
}