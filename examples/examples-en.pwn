/*
	tireFuncs implementation examples.

	DO NOT ATTEMPT TO BUILD AND/OR RUN THIS .pwn - It probably won't work as a gamemode. These are just loose examples.
*/

#include <a_samp>
#include <tire-funcs>

/*
	EXAMPLE OF CALLBACK USAGE.

	In this example, a wheel is popped, a message will be sent to the driver of the vehicle, indicating which
	tire was popped. If the library was capable of determining who destroyed it, it'll tell the driver, and
	it'll also inform the shooter of their success.

	We utilize a tireFuncs function called GetTireName to get the name of a tire as a string from it's id.
*/

public OnPlayerPopTire(playerid, vehicleid, tireid) {
	new name[MAX_PLAYER_NAME], string[128];
	new driverid = GetVehicleDriver(vehicleid);
	if (IsPlayerConnected(driverid) && IsPlayerConnected(playerid)) {
		// Notify driver
		GetPlayerName(playerid, name, MAX_PLAYER_NAME);
		format(string, sizeof(string), "%s has popped your vehicle's %s.", name, GetTireName(tireid));
		SendClientMessage(driverid, RED, string);
		// Notify shooter
		format(string, sizeof(string), "You have popped a vehicle's %s.", GetTireName(tireid));
		SendClientMessage(playerid, GREEN, string);
	} else if (IsPlayerConnected(driverid)) {
		format(string, sizeof(string), "Your vehicle's %s has been popped.", GetTireName(tireid));
		SendClientMessage(driverid, RED, string);
	}
}

/*

	EXAMPLE OF FUNCTION USAGE

	In this example, we pop the two back tires of a vehicle, while leaving the other two unchanged.

	Note that we don't care about the vehicle being a bike or not, since the position of front and back tires are respected across them.
*/

stock DestroyBackTires(vehicleid) {
	SetVehicleIndividualTires(TIRE_UNCHANGED, TIRE_UNCHANGED, TIRE_POPPED, TIRE_POPPED);
}

/*
	In this example, we notify the user of their popped tires in a chat message.

	In a practical sense, this is obviously useless, but it should give you an idea of how the function works.

	Example output: "Popped tires: front left tire, rear left tire."
*/

stock NotifyUserTires(playerid) {
	new vehicleid = IsPlayerInAnyVehicle(playerid);
	if (!IsValidVehicle(vehicleid)) {
		SendClientMessage(playerid, RED, "You're not in any vehicle.");
		return 1;
	}
	new string[128], tireCount = 0;
	format(string, 128, "Popped tires: ");
	// Loop through all 6 possible tires.
	// This automatically takes care of wether the vehicle is a bike or not at the library level.
	// It'll return -1 if the vehicle isn't suppoused to be a tire, so we don't have to worry about that at implementation level.
	for (new i = 0; i < VALID_TIRE_TYPES; i++) {
		if (GetIndividualTireState(vehicleid, i) == TIRE_POPPED) {
			if (!tireCount) {
				format(string, 128, "%s %s", string, GetTireName(i));
			} else {
				format(string, 128, "%s, %s", string, GetTireName(i));
			}
			++tireCount;
		}
	}
	if (!tireCount) {
		format(string, 128, "Popped tires: None");
	}
	SendClientMessage(playerid, -1, string);
	return 1;
}

/*
	In this example, we fix a user's tires and change $500 per each tire.

	Note that ideally you want to check if a user has enough money before charging them,
	and you also probably want to use a server sided money system. But for this is just for demonstration.
*/

stock FixPlayerTires(playerid) {
	new vehicleid = IsPlayerInAnyVehicle(playerid);
	if (!IsValidVehicle(vehicleid)) {
		SendClientMessage(playerid, RED, "You're not in any vehicle.");
		return 1;
	}
	new front_left, front_right, rear_left, rear_right;
	GetVehicleIndividualTires(vehicleid, front_left, front_right, rear_left, rear_right);
	new price = 0;
	// If this is a bike, the "invalid" tires can't be popped, so this is fine.
	if (front_left == TIRE_POPPED) price += 500;
	if (front_right == TIRE_POPPED) price += 500;
	if (rear_left == TIRE_POPPED) price += 500;
	if (rear_right == TIRE_POPPED) price += 500;
	SetVehicleIndividualTires(vehicleid, TIRE_FIX, TIRE_FIX, TIRE_FIX, TIRE_FIX);
	GivePlayerMoney(playerid, -price);
	new string[128];
	format(string, 128, "Your tires have been fixed. Cost: $%i", price);
	SendClientMessage(playerid, -1, string);
}





/*
	Function used above, not related to tireFuncs
*/

stock GetVehicleDriver(vehicleid) {
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i)) continue;
		if (GetPlayerState(i) != PLAYER_STATE_DRIVER) continue;
		if (IsPlayerInVehicle(i, vehicleid)) return i;
	}
	return INVALID_PLAYER_ID;
}