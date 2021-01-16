# SAMP-tireFuncs
tireFuncs es una simple libreria que implementa callbacks para daños a ruedas, y funciones simplificadas para controlar el estado de las ruedas de los vehiculos en SA-MP.

## Instalacion

Para instalar esta libreria, simplemente descarga [tire-funcs.inc](tire-funcs.inc) e incluyelo en tu gamemode.
Agrega `#define TIRE_FUNCS_SPANISH` arriba del include si deseas que `GetTireName` retorne valores en español.

<br>

## Funciones
Ciertos valores aqui remarcados se explican mas abajo.

Funcion | Comportamiento
--- | ---
GetVehicleIndividualTires(vehicleid, &frontLeft, &frontRight, &rearLeft, &rearRight) | Retorna el estado de cada rueda individualmente. Puede devolver los valores `TIRE_NOT_POPPED (0)` o `TIRE_POPPED (1)`
SetVehicleIndividualTires(vehicleid, frontLeft, frontRight, rearLeft, rearRight) | Permite definir el estado de cada rueda individualmente. Acepta los valores `TIRE_NOT_POPPED (0)`, `TIRE_POPPED (1)` y `TIRE_UNCHANGED (2)`
GetTireName(tireid) | Recibe el id de una rueda y devuelve su nombre (Por ejemplo, si el id de la rueda es `REAR_LEFT_TIRE (2)` esto devolvera un string `"rueda trasera izquierda"`)
<br>

## Callbacks

vModData implementa un solo callback:

#### OnPlayerPopTire(playerid, vehicleid, tireid)

Este indica que `playerid` revento la rueda `tireid` del vehiculo `vehicleid`.

`playerid` sera INVALID_PLAYER_ID si la libreria no logra saber que jugador lo hizo por algun motivo.

Ejemplo de uso:

```cpp
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
```

En este ejemplo, se envia un mensaje al conductor de un vehiculo si le revientan su rueda. Y en el caso de que se determine quien fue, se le informa su nombre. Al mismo tiempo, tambien notifica a quien haya disparado que ha logrado reventar una rueda.

*Nota: `GetVehicleDriver` no se incluye en esta libreria.


## Valores

Para no tener que andar liandose con "numeros magicos", tireFuncs introduce los siguientes macros.

Para las ruedas:

```
FRONT_LEFT_TIRE   // Rueda frontal izquierda
FRONT_RIGHT_TIRE  // Rueda frontal derecha
REAR_LEFT_TIRE    // Rueda trasera izquierda
REAR_RIGHT_TIRE   // Rueda trasera derecha
BIKE_FRONT_TIRE   // Rueda frontal (en caso de ser moto)
BIKE_REAR_TIRE    // Rueda trasera (en caso de ser moto)
```

Para los estados:
```
TIRE_NOT_POPPED   // Rueda no reventada
TIRE_POPPED       // Rueda reventada
TIRE_UNCHANGED    // Rueda no modificada (se utiliza en `SetVehicleIndividualTires` para no modificar una rueda especifica.)
```
