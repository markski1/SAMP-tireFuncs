/*
	Ejemplos de implementación de tireFuncs

	NO INTENTES COMPILAR O EJECUTAR ESTE .pwn - Probablemente no funcionara. Estos son solo  ejemplos sueltos.
*/

#include <a_samp>
#include <tire-funcs>

/*
	EJEMPLO DE USO DEL CALLBACK.

	En este ejemplo, un mensaje es enviado al conductor del vehiculo indicandole cual rueda fue pinchada.
	Si tireFuncs fue capaz de determinar quien destruyo la rueda, se le dice el nombre.
	Tambien se le avisa a quien lo haya hecho de su exito.

	Utilizamos la funcion GetTireName para obtener el nombre de la rueda pinchada.
*/

public OnPlayerPopTire(playerid, vehicleid, tireid) {
	new name[MAX_PLAYER_NAME], string[128];
	new driverid = GetVehicleDriver(vehicleid);
	if (IsPlayerConnected(driverid) && IsPlayerConnected(playerid)) {
		// Notificar al conductor
		GetPlayerName(playerid, name, MAX_PLAYER_NAME);
		format(string, sizeof(string), "%s ha reventado la %s de tu vehiculo.", name, GetTireName(tireid));
		SendClientMessage(driverid, RED, string);
		// Notificar al jugador
		format(string, sizeof(string), "Has reventado la %s de un vehiculo.", GetTireName(tireid));
		SendClientMessage(playerid, GREEN, string);
	} else if (IsPlayerConnected(driverid)) {
		format(string, sizeof(string), "La %s de tu vehiculo ha sido reventada.", GetTireName(tireid));
		SendClientMessage(driverid, RED, string);
	}
}

/*
	EJEMPLOS DE USO DE FUNCION

	En este ejemplo, pinchamos las dos ruedas de atras de un vehiculo, y dejamos las otras dos sin modificar.

	Notese que no nos importa si es una moto o no, ya que la posicion de las ruedas se respeta, y las ruedas que la moto no tenga se ignoran.
*/

stock DestrozarRuedasTraseras(vehicleid) {
	SetVehicleIndividualTires(TIRE_UNCHANGED, TIRE_UNCHANGED, TIRE_POPPED, TIRE_POPPED);
}

/*
	En este ejemplo, le decimos a un jugador que ruedas tiene reventadas.

	En un sentido practico, esto es obviamente inutil, ya que el jugador puede simplemente mirar sus ruedas.
	Pero en este caso funciona como un ejemplo para utilizar el include.

	Ejemplo del mensaje: "Ruedas pinchadas: rueda trasera izquierda, rueda delantera izquierda."
*/

stock NotificarUsuarioDeRuedas(playerid) {
	new vehicleid = IsPlayerInAnyVehicle(playerid);
	if (!IsValidVehicle(vehicleid)) {
		SendClientMessage(playerid, RED, "No estas en un vehiculo.");
		return 1;
	}
	new string[128], tireCount = 0;
	format(string, 128, "Ruedas pinchadas: ");
	// Loopear por las 6 posibles ruedas.
	// La libreria se encarga de preocuparse por si el vehiculo es una moto o no, y devuelve -1 si el vehiculo no debe tener X rueda.
	// De esta forma eso no les debe preocupar a la hora de implementarlo.
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
		format(string, 128, "Ruedas pinchadas: Ninguna");
	}
	SendClientMessage(playerid, -1, string);
	return 1;
}

/*
	En este ejemplo, arreglamos las ruedas de un usuario y les cobramos $500 por cada una.

	Notese que idealmente querras chequear si un usuario tiene suficiente dinero antes de cobrarles.
	Tambien vas a querer utilizar dinero del lado del servidor. Aqui no importa, ya que es solo una demostración.
*/

stock ArreglarRuedasDeJugador(playerid) {
	new vehicleid = IsPlayerInAnyVehicle(playerid);
	if (!IsValidVehicle(vehicleid)) {
		SendClientMessage(playerid, RED, "No estas en un vehiculo.");
		return 1;
	}
	new front_left, front_right, rear_left, rear_right;
	GetVehicleIndividualTires(vehicleid, front_left, front_right, rear_left, rear_right);
	new precio = 0;
	// Si se trata de una moto, no reportara las ruedas faltantes como destruidas, asi que esto no importa.
	if (front_left == TIRE_POPPED) precio += 500;
	if (front_right == TIRE_POPPED) precio += 500;
	if (rear_left == TIRE_POPPED) precio += 500;
	if (rear_right == TIRE_POPPED) precio += 500;
	SetVehicleIndividualTires(vehicleid, TIRE_FIX, TIRE_FIX, TIRE_FIX, TIRE_FIX);
	GivePlayerMoney(playerid, -precio);
	new string[128];
	format(string, 128, "Tus ruedas han sido arregladas. Se te ha cobrado $%i", price);
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