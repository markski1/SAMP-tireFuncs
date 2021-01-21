# SAMP-tireFuncs
tireFuncs es una simple libreria que implementa callbacks para daños a ruedas, y funciones simplificadas para controlar el estado de las ruedas de los vehiculos en SA-MP.

## Instalacion

Para instalar esta libreria, simplemente descarga [tire-funcs.inc](../tire-funcs.inc) e incluyelo en tu gamemode.
Agrega `#define TIRE_FUNCS_SPANISH` arriba del include si deseas que `GetTireName` retorne valores en español.

<br>

## Funciones
Ciertos valores aqui remarcados se explican mas abajo.

Funcion | Comportamiento
--- | ---
GetVehicleIndividualTires(vehicleid, &frontLeft, &frontRight, &rearLeft, &rearRight) | Retorna el estado de cada rueda individualmente. Puede devolver los valores `TIRE_NOT_POPPED (0)` o `TIRE_POPPED (1)`
SetVehicleIndividualTires(vehicleid, frontLeft, frontRight, rearLeft, rearRight) | Permite definir el estado de cada rueda individualmente. Acepta los valores `TIRE_NOT_POPPED (0)`, `TIRE_POPPED (1)` y `TIRE_UNCHANGED (2)`
GetTireName(tireid) | Recibe el id de una rueda y devuelve su nombre (Por ejemplo, si el id de la rueda es `REAR_LEFT_TIRE (2)` esto devolvera un string `"rueda trasera izquierda"`)
GetIndividualTireState(vehicleid, i) | Recibe el id de un vehiculo, el id de una rueda, y retorna su estado. Similar a GetVehicleIndividualTires, pero para una sola rueda. Retorna -1 si el vehiculo no tiene dicha rueda. (Por ejemplo, si se pide el estado de una rueda delantera de moto en un auto)

<br>

## Callbacks

tireFuncs implementa un solo callback:

#### OnPlayerPopTire(playerid, vehicleid, tireid)

Este indica que `playerid` revento la rueda `tireid` del vehiculo `vehicleid`.

`playerid` sera INVALID_PLAYER_ID si la libreria no logra saber que jugador lo hizo por algun motivo.

Notese que este callback solo funciona con lagcomp. Si tu server no tiene lagcomp, tendras que usar [opws-fix](https://github.com/boorzz/samp-opws-fix/).

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
TIRE_FIX          // Lo mismo que TIRE_NOT_POPPED, pero tiene mas sentido en algunos casos.
TIRE_POPPED       // Rueda reventada
TIRE_UNCHANGED    // Rueda no modificada (se utiliza en `SetVehicleIndividualTires` para no modificar una rueda especifica.)
```

Por ultimo, `VALID_TIRE_TYPES` es la cantidad de ruedas que maneja este include (6). Si bien las chances de que este numero cambie tienden a 0, no esta de mas utilizar el macro para estar seguros.

## Ejemplos de implementación

Muchos ejemplos estan disponibles [aqui](../examples/ejemplos-es.pwn)
