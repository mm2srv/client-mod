# Mario Maker 2 Server Client
Used to add extra features, notably level-specific mods, to the server

## How to build
```
./exlaunch.sh build
```
Output will be in `deploy`

## Info file format
The player must place a file in the following format in `ocw-info.txt` at the root of their SD card.
```
server.opencourse.world
d4LtgY4xDecIBBUUPVw8Vw
443
6001
```
The first line is the domain hosting the server. The second line is the players auth token. The third line is the port of the main server. The fourth line is the port of the extra server (serves thumbnails, level data, etc).

## wasm3
m2_api_libc.c and header were removed