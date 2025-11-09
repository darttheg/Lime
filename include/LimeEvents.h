#pragma once

#include "Event.h"

namespace Events {
	namespace Lime {
		inline std::shared_ptr<Event> OnStart = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnUpdate = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnEnd = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnResize = std::make_shared<Event>();
	}

	namespace Input {
		inline std::shared_ptr<Event> OnKeyPressed = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnKeyReleased = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnMouseClick = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnMouseMove = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnMouseScroll = std::make_shared<Event>();

		inline std::shared_ptr<Event> OnJoystickConnect = std::make_shared<Event>(); // calls with joystick #
		inline std::shared_ptr<Event> OnJoystickDisconnect = std::make_shared<Event>(); // calls with joystick #
		inline std::shared_ptr<Event> OnJoystickButtonPressed = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnJoystickButtonReleased = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnJoystickDPadDelta = std::make_shared<Event>(); // calls with joystick # and new direction
	}

	namespace Networking {
		inline std::shared_ptr<Event> SonPeerConnect = std::make_shared<Event>();
		inline std::shared_ptr<Event> SonPeerDisconnect = std::make_shared<Event>();
		inline std::shared_ptr<Event> SonPacketReceived = std::make_shared<Event>();
		inline std::shared_ptr<Event> ConConnect = std::make_shared<Event>();
		inline std::shared_ptr<Event> ConDisconnect = std::make_shared<Event>();
		inline std::shared_ptr<Event> ConPacketReceived = std::make_shared<Event>();

		inline std::shared_ptr<Event> OnHTTPGet = std::make_shared<Event>();
		inline std::shared_ptr<Event> OnHTTPDownloadComplete = std::make_shared<Event>();
	}
}

inline void initEvents() {
	(*lua)["Lime"]["OnStart"] = Events::Lime::OnStart;
	(*lua)["Lime"]["OnUpdate"] = Events::Lime::OnUpdate;
	(*lua)["Lime"]["OnEnd"] = Events::Lime::OnEnd;
	(*lua)["Lime"]["OnResize"] = Events::Lime::OnResize;

	(*lua)["Input"]["OnKeyPressed"] = Events::Input::OnKeyPressed;
	(*lua)["Input"]["OnKeyReleased"] = Events::Input::OnKeyReleased;
	(*lua)["Input"]["OnMouseClick"] = Events::Input::OnMouseClick;
	(*lua)["Input"]["OnMouseMove"] = Events::Input::OnMouseMove;
	(*lua)["Input"]["OnMouseScroll"] = Events::Input::OnMouseScroll;

	(*lua)["Input"]["OnJoystickConnect"] = Events::Input::OnJoystickConnect;
	(*lua)["Input"]["OnJoystickDisconnect"] = Events::Input::OnJoystickDisconnect;
	(*lua)["Input"]["OnJoystickButtonPressed"] = Events::Input::OnJoystickButtonPressed;
	(*lua)["Input"]["OnJoystickButtonReleased"] = Events::Input::OnJoystickButtonReleased;
	(*lua)["Input"]["OnJoystickDPadDelta"] = Events::Input::OnJoystickDPadDelta;

	(*lua)["Network"]["Server"]["OnClientConnect"] = Events::Networking::SonPeerConnect;
	(*lua)["Network"]["Server"]["OnClientDisconnect"] = Events::Networking::SonPeerDisconnect;
	(*lua)["Network"]["Server"]["OnPacketReceived"] = Events::Networking::SonPacketReceived;
	(*lua)["Network"]["Client"]["OnConnect"] = Events::Networking::ConConnect;
	(*lua)["Network"]["Client"]["OnDisconnect"] = Events::Networking::ConDisconnect;
	(*lua)["Network"]["Client"]["OnPacketReceived"] = Events::Networking::ConPacketReceived;

	(*lua)["Network"]["OnGetComplete"] = Events::Networking::OnHTTPGet;
	(*lua)["Network"]["OnDownloadComplete"] = Events::Networking::OnHTTPDownloadComplete;
}