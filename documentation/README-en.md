# SAMP-tireFuncs
tireFuncs is a simple library which implements callbacks for tire damage and simplified functions for handling tires on vehicles.

## Installation

To install this library, simply download [tire-funcs.inc](../tire-funcs.inc) and include it into your gamemode.

<br>

## Functions

Function | Behaviour
--- | ---
GetVehicleIndividualTires(vehicleid, &frontLeft, &frontRight, &rearLeft, &rearRight) | Returns the state of each individual tire of a vehicle by parameter. The values can be `TIRE_NOT_POPPED (0)` or `TIRE_POPPED (1)`
SetVehicleIndividualTires(vehicleid, frontLeft, frontRight, rearLeft, rearRight) | Allows you to set the state of each individual tire of a vehicle. Acceptable values are `TIRE_NOT_POPPED (0)`, `TIRE_POPPED (1)` or `TIRE_UNCHANGED (2)`
GetTireName(tireid) | Receives a tireid and returns a string containing it's name. (I.E. if tireid is `REAR_LEFT_TIRE (2)` this will return `"rear left tire"`)
GetIndividualTireState(vehicleid, i) | Receives a vehicleid and tireid, and returns it's state. Similar to GetVehicleIndividualTires, but for a single tire. Returns -1 if the vehicle has no such tire. (I.E if requesting the state of BIKE_FRONT_TIRE in a 4 wheel car)

<br>

## Callbacks

tireFuncs implements a single callback:

#### OnPlayerPopTire(playerid, vehicleid, tireid)

It indicates that `playerid` popped a tire `tireid` of the vehicle `vehicleid`.

`playerid` will be INVALID_PLAYER_ID if the player cannot be determined for some reason.

Please note that you need lagcomp for this callback to work. Otherwise, you'll need to use [opws-fix](https://github.com/boorzz/samp-opws-fix/).

## Values

To avoid developers from having to deal with "magic numbers", tireFuncs introduces the following macros.

For tires:

```
FRONT_LEFT_TIRE
FRONT_RIGHT_TIRE
REAR_LEFT_TIRE
REAR_RIGHT_TIRE
BIKE_FRONT_TIRE
BIKE_REAR_TIRE
```

For state:
```
TIRE_NOT_POPPED
TIRE_FIX        // Same as TIRE_NOT_POPPED, but makes more sense in some scenarios
TIRE_POPPED
TIRE_UNCHANGED 	// Used in `SetVehicleIndividualTire` to not modify a specified tire
```

## Implementation examples

Many example snippets are available [here](../examples/examples-en.pwn)