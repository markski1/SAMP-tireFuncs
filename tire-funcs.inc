/*
	tireFuncs 1.0

	mrks.cf | github.com/markski1

	Thanks to:
		open.mp/docs contributors
		SACNR
*/

#if defined tireFuncs_included
	#endinput
#endif

#if !defined IsValidVehicle
	native IsValidVehicle(vehicleid);
#endif

#define tireFuncs_included

// TIRE TYPES
// These are arbitrary and made only for easily handling individual tires in a recognizable way.

#define FRONT_LEFT_TIRE    0
#define FRONT_RIGHT_TIRE   1
#define REAR_LEFT_TIRE     2
#define REAR_RIGHT_TIRE    3
#define BIKE_FRONT_TIRE    4
#define BIKE_REAR_TIRE     5
#define INVALID_TIRE       6  // Unused

// TIRE DAMAGE
// NOT POPPED and POPPED are reasonable values that go accordingly with how samp manages tire states. UNCHANGED is an arbitrary value used when applying states.

#define TIRE_NOT_POPPED    0
#define TIRE_POPPED        1
#define TIRE_UNCHANGED     2

// TIRE NAMES

#if defined TIRE_FUNCS_SPANISH
	static tf_TireNames[7][32] = {
		"rueda delantera izquierda",
		"rueda delantera derecha",
		"rueda trasera izquierda",
		"rueda trasera derecha",
		"rueda delantera",
		"rueda trasera",
		"rueda invalida"
	};
#else
	static tf_TireNames[7][32] = {
		"front left tire",
		"front right tire",
		"rear left tire",
		"rear right tire",
		"front tire",
		"back tire",
		"invalid tire"
	};
#endif

// INTERNAL FIGURES

#if !defined TIRE_POP_THRESHOLD
	#define TIRE_POP_THRESHOLD     1000 // The maximum amount of milliseconds between a player shooting a vehicle and the vehicle reporting a popped tire for the two to be correlated
#endif

enum tf_pData {
	tf_veh_id,
	tf_shot_tc
}

static tf_Player[MAX_PLAYERS][tf_pData];

static tf_Tires[MAX_VEHICLES];



// Forward main callback

forward OnPlayerPopTire(playerid, vehicleid, tireid);



// Gamemode hooks



public OnPlayerConnect(playerid) {
	tf_Player[playerid][tf_veh_id] = 0;
	tf_Player[playerid][tf_shot_tc] = 0;

	#if defined tf_OnPlayerConnect
		return tf_OnPlayerConnect(playerid);
	#endif
}

public OnPlayerDeath(playerid, killerid, reason) {
	tf_Player[playerid][tf_veh_id] = 0;
	tf_Player[playerid][tf_shot_tc] = 0;

	#if defined tf_OnPlayerDeath
		return tf_OnPlayerDeath(playerid, killerid, reason);
	#endif
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) {
	if (hittype == BULLET_HIT_TYPE_VEHICLE) {
		if (IsValidVehicle(hitid)) {
			tf_Player[playerid][tf_veh_id] = hitid;
			tf_Player[playerid][tf_shot_tc] = GetTickCount();
		}
	}

	#if defined TIREFUNCS_DEBUG
		SendClientMessage(playerid, 0x00AA00FF, "[DEBUG] tireFuncs detects you shooting a vehicle.");
	#endif

	#if defined tf_OnPlayerWeaponShot
		return tf_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
	#endif
}

public OnVehicleDamageStatusUpdate(vehicleid, playerid) {
	if (IsValidVehicle(vehicleid)) {

		new dummy, tires;
		GetVehicleDamageStatus(vehicleid, dummy, dummy, dummy, tires);

		// If the newly reported damage reports changes to the state of the tires
		if (tf_Tires[vehicleid] != tires) {
			new newFrontRight, newFrontLeft, newRearRight, newRearearLeft;
			new oldFrontRight, oldFrontLeft, oldRearRight, oldRearearLeft;
			// Get the tire states for before and after the damage was reported
			tf_GetTireState(tires, newRearRight, newFrontRight, newRearearLeft, newFrontLeft);
			tf_GetTireState(tf_Tires[vehicleid], oldRearRight, oldFrontRight, oldRearearLeft, oldFrontLeft);

			// Report damage for all popped tires, individually.
			if (tf_IsBike(vehicleid)) {
				if (newRearRight == TIRE_POPPED && newRearRight != oldRearRight) {
					tf_ReportTireDamage(vehicleid, BIKE_FRONT_TIRE);
				}
				if (newFrontRight == TIRE_POPPED && newFrontRight != oldFrontRight) {
					tf_ReportTireDamage(vehicleid, BIKE_REAR_TIRE);
				}
			} else {
				if (newRearRight == TIRE_POPPED && newRearRight != oldRearRight) {
					tf_ReportTireDamage(vehicleid, FRONT_RIGHT_TIRE);
				}
				if (newFrontRight == TIRE_POPPED && newFrontRight != oldFrontRight) {
					tf_ReportTireDamage(vehicleid, REAR_RIGHT_TIRE);
				}
				if (newRearearLeft == TIRE_POPPED && newRearearLeft != oldRearearLeft) {
					tf_ReportTireDamage(vehicleid, FRONT_LEFT_TIRE);
				}
				if (newFrontLeft == TIRE_POPPED && newFrontLeft != oldFrontLeft) {
					tf_ReportTireDamage(vehicleid, REAR_LEFT_TIRE);
				}
			}

			// And update the internal tire state
			tf_Tires[vehicleid] = tires;
		}
	}

	#if defined tf_OnVehicleDamageStatusUpdate
		return tf_OnVehicleDamageStatusUpdate(vehicleid, playerid);
	#endif
}

public OnVehicleSpawn(vehicleid) {
	new dummy;
	GetVehicleDamageStatus(vehicleid, dummy, dummy, dummy, tf_Tires[vehicleid]);

	#if defined tf_OnVehicleSpawn
		return tf_OnVehicleSpawn(vehicleid);
	#endif
}


// User functions



stock GetVehicleIndividualTires(vehicleid, &frontLeft, &frontRight, &rearLeft, &rearRight) {
	if (!IsValidVehicle(vehicleid)) return 0;
	tf_GetTireState(tf_Tires[vehicleid], rearRight, frontRight, rearLeft, frontLeft);
	return 1;
}

stock SetVehicleIndividualTires(vehicleid, frontLeft, frontRight, rearLeft, rearRight) {
	if (!IsValidVehicle(vehicleid)) return 0;
	new panels, doors, lights, tires;
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
    new frontLeftInternal, frontRightInternal, rearLeftInternal, rearRightInternal;
    tf_GetTireState(tires, rearRight, frontRight, rearLeft, frontLeft);
    if (frontLeft != TIRE_UNCHANGED) {
    	frontLeftInternal = frontLeft;
    }
    if (frontRight != TIRE_UNCHANGED) {
    	frontRightInternal = frontRight;
    }
    if (rearLeft != TIRE_UNCHANGED) {
    	rearLeftInternal = rearLeft;
    }
    if (rearRight != TIRE_UNCHANGED) {
    	rearRightInternal = rearRight;
    }
    tires = tf_EncodeTireState(rearRightInternal, frontRightInternal, rearLeftInternal, frontLeftInternal);
    UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
    tf_Tires[vehicleid] = tires;
    return 1;
}

stock GetTireName(tireid) {
	if (0 >= tireid >= 5) {
		return tf_TireNames[tireid];
	} else {
		return tf_TireNames[6];
	}
}


// Internal functions



static tf_ReportTireDamage(vehicleid, tireid) {
	new currentTc = GetTickCount();
	new currentMax = 0, currentID = INVALID_PLAYER_ID;
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (!IsPlayerConnected(i)) continue;
		if (tf_Player[i][tf_veh_id] != vehicleid) continue;
		if (tf_Player[i][tf_shot_tc] + TIRE_POP_THRESHOLD > currentTc) {
			if (tf_Player[i][tf_shot_tc] > currentMax) {
				currentMax = tf_Player[i][tf_shot_tc];
				currentID = i;
			}
		}
	}
	CallLocalFunction("OnPlayerPopTire", "iii", currentID, vehicleid, tireid);
	#if defined TIREFUNCS_DEBUG
		new debugStr[128];
		format(debugStr, 128, "[DEBUG] tireFuncs detects popped tire %i on vehicle id %i.", tireid, vehicleid);
		SendClientMessageToAll(0x00FAFAFF, debugStr);
	#endif
}

static tf_IsBike(vehicleid) {
	switch (GetVehicleModel(vehicleid)) {
		case 448, 461..463, 468, 471, 481, 509, 510, 521..523, 581, 586: return 1;
	}
	return 0;
}

static tf_GetTireState(tires, &rearRight, &frontRight, &rearLeft, &frontLeft) {
	rearRight = tires & 1;
	frontRight = tires >> 1 & 1;
	rearLeft = tires >> 2 & 1;
	frontLeft = tires >> 3 & 1;
}


// This should be a 'static stock', but that would make this incompatible with the old normie compiler
stock tf_EncodeTireState(rearRight, frontRight, rearLeft, frontLeft) {
    return rearRight | (frontRight << 1) | (rearLeft << 2) | (frontLeft << 3);
}


// ALS4 (7?) HOOKING
// https://github.com/compuphase/pawn/issues/28

#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect tf_OnPlayerConnect

#if defined tf_OnPlayerConnect
	forward tf_OnPlayerConnect();
#endif

#if defined _ALS_OnPlayerDeath
	#undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif

#define OnPlayerDeath tf_OnPlayerDeath

#if defined tf_OnPlayerDeath
	forward tf_OnPlayerDeath();
#endif

#if defined _ALS_OnVehicleDamageStatusUpd
	#undef OnVehicleDamageStatusUpdate
#else
	#define _ALS_OnVehicleDamageStatusUpd
#endif

#define OnVehicleDamageStatusUpdate tf_OnVehicleDamageStatusUpdate

#if defined tf_OnVehicleDamageStatusUpdate
	forward tf_OnVehicleDamageStatusUpdate();
#endif

#if defined _ALS_OnVehicleSpawn
	#undef OnVehicleSpawn
#else
	#define _ALS_OnVehicleSpawn
#endif

#define OnVehicleSpawn tf_OnVehicleSpawn

#if defined tf_OnVehicleSpawn
	forward tf_OnVehicleSpawn();
#endif

#if defined _ALS_OnPlayerWeaponShot
	#undef OnPlayerWeaponShot
#else
	#define _ALS_OnPlayerWeaponShot
#endif

#define OnPlayerWeaponShot tf_OnPlayerWeaponShot

#if defined tf_OnPlayerWeaponShot
	forward tf_OnPlayerWeaponShot();
#endif