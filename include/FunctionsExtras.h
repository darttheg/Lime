#include "FunctionsImports.h"
#include "LimeEvents.h"

namespace Steam {
#include <steam/steam_api.h>

	bool isInitialized = false;

	static void SteamRunCallbacks() {
		if (isInitialized) SteamAPI_RunCallbacks();
	}

	bool initSteam(sol::optional<int> opID) {
		int id = opID.value_or(480); // Spacewar
		_putenv_s("SteamAppId", std::to_string(id).c_str());
		_putenv_s("SteamGameId", std::to_string(id).c_str());

		if (SteamAPI_RestartAppIfNecessary(id))
			return false;

		isInitialized = SteamAPI_Init();
		return isInitialized;
	}

	bool isRunning() {
		return SteamAPI_IsSteamRunning();
	}

	std::string getUsername() {
		if (!isInitialized) return "";
		return SteamFriends()->GetPersonaName();
	}

	int getUserID() {
		if (!isInitialized) return -1;
		return SteamUser()->GetSteamID().ConvertToUint64();
	}

	void setRichPresence(std::string key, std::string val) {
		if (!isInitialized) return;
		SteamFriends()->SetRichPresence(key.c_str(), val.c_str());
	}

	void clearRichPresence() {
		if (!isInitialized) return;
		SteamFriends()->ClearRichPresence();
	}

	void bind() {
		if (!std::filesystem::exists("steam_api64.dll"))
			return;

		sol::table steam = lua->create_named_table("Steam");

		steam["Initialize"] = &initSteam;
		steam["IsRunning"] = &isRunning;
		steam["GetUsername"] = &getUsername;
		steam["GetUserID"] = &getUserID;
		steam["SetRichPresence"] = &setRichPresence;
		steam["ClearRichPresence"] = &clearRichPresence;

		irrHandler->apiRegistry["Steam"] = steam;

		SteamRunCallbacks();
		lua->set_function("__steamapi_run_callbacks", []() {
			SteamRunCallbacks();
		});

		Events::Lime::OnUpdate.get()->hook(lua->get<sol::function>("__steamapi_run_callbacks"));
	}
};

void bindExtras() {
	Steam::bind();
}