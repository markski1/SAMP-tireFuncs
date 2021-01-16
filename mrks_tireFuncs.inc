/*
	mrks_tireFuncs 0.1

	mrks.cf | github.com/markski1

	Thanks to:
		open.mp/docs contributors
		SACNR

	Notes:
		- 	Currently, you NEED y_hooks in order for this include to function. As I personally don't support includes requiring YSI to function,
			I'll add proper non-y_hook compatibility to this, when I have the time.
*/

#if !defined _INC_y_hooks
	#error You must have y_hooks included for mrks_tireFuncs to work. If y_hooks is included, make sure you're including mrks_tireFuncs after it.
#endif

// TIRE TYPES

#define FRONT_LEFT_TIRE    0
#define FRONT_RIGHT_TIRE   1
#define REAR_LEFT_TIRE     2
#define REAR_RIGHT_TIRE    3
#define BIKE_FRONT_TIRE    4
#define BIKE_REAR_TIRE     5
#define INVALID_TIRE       6  // Unused

// TIRE DAMAGE

#define TIRE_NOT_POPPED    0
#define TIRE_POPPED        1

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



hook OnPlayerConnect(playerid) {
	tf_Player[playerid][tf_veh_id] = 0;
	tf_Player[playerid][tf_shot_tc] = 0;
}

hook OnPlayerDeath(playerid) {
	tf_Player[playerid][tf_veh_id] = 0;
	tf_Player[playerid][tf_shot_tc] = 0;
}

hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) {
	if (hittype == BULLET_HIT_TYPE_VEHICLE) {
		if (IsValidVehicle(hitid)) {
			tf_Player[playerid][tf_veh_id] = hitid;
			tf_Player[playerid][tf_shot_tc] = GetTickCount();
		}
	}
	return 1;
}

// Damage is redefined as Dmg in hook replacements to fix a stupid limitation of the pawn compiler.
// Update is already shrinked to Upd by default in y_hook's impl.
DEFINE_HOOK_REPLACEMENT__(Damage, Dmg);
hook OnVehicleDmgStatusUpd(vehicleid, playerid) {
	if (!IsValidVehicle(vehicleid)) return 1;

	new dummy, tires;
	GetVehicleDamageStatus(vehicleid, dummy, dummy, dummy, tires);

	// If the newly reported damage reports changes to the state of the tires
	if (tf_Tires[vehicleid] != tires) {
		new newFrontRight, newFrontLeft, newRearRight, newRearLeft;
		new oldFrontRight, oldFrontLeft, oldRearRight, oldRearLeft;
		// Get the tire states for before and after the damage was reported
		tf_GetTireState(tires, newRearRight, newFrontRight, newRearLeft, newFrontLeft);
		tf_GetTireState(tf_Tires[vehicleid], oldRearRight, oldFrontRight, oldRearLeft, oldFrontLeft);

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
			if (newRearLeft == TIRE_POPPED && newRearLeft != oldRearLeft) {
				tf_ReportTireDamage(vehicleid, FRONT_LEFT_TIRE);
			}
			if (newFrontLeft == TIRE_POPPED && newFrontLeft != oldFrontLeft) {
				tf_ReportTireDamage(vehicleid, REAR_LEFT_TIRE);
			}
		}

		// And update the internal tire state
		tf_Tires[vehicleid] = tires;
	}
	return 1;
}

hook OnVehicleSpawn(vehicleid) {
	new dummy;
	GetVehicleDamageStatus(vehicleid, dummy, dummy, dummy, tf_Tires[vehicleid]);
}


// User functions



stock GetVehicleIndividualTires(vehicleid, &fLeft, &fRight, &rLeft, &rRight) {
	if (!IsValidVehicle(vehicleid)) return 0;
	tf_GetTireState(tf_Tires[vehicleid], rRight, fRight, rLeft, fLeft);
	return 1;
}

stock SetVehicleIndividualTires(vehicleid, fLeft, fRight, rLeft, rRight) {
	if (!IsValidVehicle(vehicleid)) return 0;
	new panels, doors, lights, tires;
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
    tires = tf_EncodeTireState(rRight, fRight, rLeft, fLeft);
    UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
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
			if (tf_Player[i][tf_shot_tc] + TIRE_POP_THRESHOLD > currentMax) {
				currentMax = tf_Player[i][tf_shot_tc] + TIRE_POP_THRESHOLD;
				currentID = i;
			}
		}
	}
	CallLocalFunction("OnPlayerPopTire", "iii", currentID, vehicleid, tireid);
}

static tf_IsBike(vehicleid) {
	switch (GetVehicleModel(vehicleid)) {
		case 448, 461..463, 468, 471, 481, 509, 510, 521..523, 581, 586: return 1;
	}
	return 0;
}

static tf_GetTireState(tires, &rRight, &fRight, &rLeft, &fLeft) {
	rRight = tires & 1;
	fRight = tires >> 1 & 1;
	rLeft = tires >> 2 & 1;
	fLeft = tires >> 3 & 1;
}


// This should be a 'static stock', but that would make this incompatible with the old normie compiler
stock tf_EncodeTireState(rRight, fRight, rLeft, fLeft) {
    return rRight | (fRight << 1) | (rLeft << 2) | (fLeft << 3);
}