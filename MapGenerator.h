#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

#define MAP_SIZE_X 64
#define MAP_SIZE_Y 32

#define ROOM_MAX_SIZE 8
#define ROOM_MIN_SIZE 2

#define MAX_ROOMS 32

#define EXPAND_PROBABILITY_X 100
#define EXPAND_PROBABILITY_Y 33

struct MapTile {
    char value;
    //MapTile
};

struct Vector2D {
    int x, y;
    Vector2D() {
        x = y = 0;
    }

    Vector2D(int _x, int _y) {
        x = _x;
        y = _y;
    }


};

struct MapRoom {
    Vector2D pos;
    Vector2D size;
    char tileType;
    bool hasExpanded;

    MapRoom* leftRoom;
    MapRoom* rightRoom;
    MapRoom* botRoom;
    MapRoom* topRoom;
    
    MapRoom() {
        hasExpanded = false;
        leftRoom = nullptr;
        rightRoom = nullptr;
        botRoom = nullptr;
        topRoom = nullptr;
        tileType = 'A';
    }

    MapRoom(Vector2D _pos, Vector2D _size, char _tile) {
        pos = _pos;
        size = _size;
        tileType = _tile;
        
        hasExpanded = false;
        leftRoom = nullptr;
        rightRoom = nullptr;
        botRoom = nullptr;
        topRoom = nullptr;
    }
};

struct Map {
    int currentStep;
    int currentRoom;
    std::vector<std::vector<char>> mapTiles;
    std::vector<MapRoom> mapRooms;


    Map() {
        currentStep = 0;
        currentRoom = 0;
        mapTiles.resize(MAP_SIZE_Y);
        for(int i = 0; i < MAP_SIZE_Y; ++i) {
            mapTiles[i].resize(MAP_SIZE_X);
            for(int j = 0; j < MAP_SIZE_X; ++j) {
                mapTiles[i][j] = ' ';
            }
        }
    }

    int CustomClampInt(int x, int min, int max) {
        if(x < min) { x = min; }
        else if(x >= max) { x = max; }
        return x;
    }

    Vector2D GenerateRandomRoomSize(int _width = 0, int _height = 0) {
        Vector2D result;
        if(_height <= 0) {
            result.y = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE) + ROOM_MIN_SIZE;
        }
        else {
            result.y = _height;
        }
        if(_width <= 0) {
            result.x = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE) + ROOM_MIN_SIZE;
        } 
        else {
            result.x = _width;
        }
        if(result.y == 1) {
            result.x++;
        }

        return result;
    }


    Vector2D GetValidPosition(Vector2D size, Vector2D beginPos, Vector2D endPos, Vector2D direction) {
        Vector2D tempPos(-1,-1);
        int widthCheck = 0;
        int heightCheck = 0;
        
        for(int i = beginPos.y; i != endPos.y; i += direction.y) {
            widthCheck = 0;
            for(int j = beginPos.x; j != endPos.x; j += direction.x) {
                if(mapTiles[i][j] != ' ') {
                    widthCheck = 0;
                    heightCheck = -1; //como despues se suma uno, se quedara en 0
                    break;
                }
                else {
                    widthCheck++;
                    if(widthCheck >= size.x) {
                        tempPos.x = j;
                        if (direction.x > 0) {
                            tempPos.x -= (size.x - 1);
                        }
                        widthCheck = 0;
                        break;
                    }
                }
            }
            heightCheck++;
            if(heightCheck >= size.y) {
                tempPos.y = i;
                if (direction.y > 0) {
                    tempPos.y -= (size.y - 1);
                }
                break;
            }
        }
        // cout << "size = (" << size.x << ", " << size.y << ")" << endl;
        return tempPos;
    }   

    MapRoom GenerateMapRoom(Vector2D pos, Vector2D size) {
        MapRoom room(pos, size, 'A' + currentRoom);
        int endY = pos.y + size.y;
        for(int i = pos.y; i < endY; ++i) {
            int endX = pos.x + size.x;
            for(int j = pos.x; j < endX; ++j) {
                mapTiles[i][j] = room.tileType;
            }
        }
        currentRoom++;
        return room;
    }

    void ExpandRoom(MapRoom& mapRoom) {
        MapRoom tempMapRoom = mapRoom;


        Vector2D roomSize;
        Vector2D beginSearch;
        Vector2D endSearch;
        Vector2D direction;
        Vector2D roomPosition;

        if(rand() % 100 < EXPAND_PROBABILITY_X) {
        //Expand left
            roomSize = GenerateRandomRoomSize();
            beginSearch = Vector2D(CustomClampInt(tempMapRoom.pos.x-1, 0, MAP_SIZE_X), CustomClampInt(tempMapRoom.pos.y - roomSize.y + 1, 0, MAP_SIZE_Y));
            endSearch = Vector2D(CustomClampInt(beginSearch.x - roomSize.x, 0, MAP_SIZE_X), CustomClampInt(beginSearch.y + roomSize.y + tempMapRoom.size.y - 1, 0, MAP_SIZE_Y));
            
            direction = Vector2D(-1,1);

            roomPosition = GetValidPosition(roomSize, beginSearch, endSearch, direction);
            if(roomPosition.x > -1 && roomPosition.y > -1) {
                mapRooms.push_back(GenerateMapRoom(roomPosition, roomSize));
                tempMapRoom.leftRoom = &(mapRooms[mapRooms.size()-1]);
                mapRoom.hasExpanded = true;

            }
        }

        if(rand() % 100 < EXPAND_PROBABILITY_X) {
            //expand right
            roomSize = GenerateRandomRoomSize();
            beginSearch = Vector2D(CustomClampInt(tempMapRoom.pos.x+tempMapRoom.size.x, 0, MAP_SIZE_X), CustomClampInt(tempMapRoom.pos.y - roomSize.y + 1, 0, MAP_SIZE_Y));
            endSearch = Vector2D(CustomClampInt(beginSearch.x + roomSize.x + 1, 0, MAP_SIZE_X), CustomClampInt(beginSearch.y + roomSize.y + tempMapRoom.size.y - 1, 0, MAP_SIZE_Y));
            
            direction = Vector2D(1,1);

            roomPosition = GetValidPosition(roomSize, beginSearch, endSearch, direction);
            if(roomPosition.x > -1 && roomPosition.y > -1) {
                mapRooms.push_back(GenerateMapRoom(roomPosition, roomSize));
                tempMapRoom.rightRoom = &(mapRooms[mapRooms.size()-1]);
                mapRoom.hasExpanded = true;

            }
        }
        
        if(rand() % 100 < EXPAND_PROBABILITY_Y) {
            //expand down
            roomSize = GenerateRandomRoomSize();
            beginSearch = Vector2D(CustomClampInt(tempMapRoom.pos.x - roomSize.x + 1, 0, MAP_SIZE_X), CustomClampInt(tempMapRoom.pos.y + tempMapRoom.size.y, 0, MAP_SIZE_Y));
            endSearch = Vector2D(CustomClampInt(beginSearch.x + roomSize.x + tempMapRoom.size.x - 1, 0, MAP_SIZE_X), CustomClampInt(beginSearch.y + roomSize.y + 1, 0, MAP_SIZE_Y));
            
            direction = Vector2D(1,1);

            roomPosition = GetValidPosition(roomSize, beginSearch, endSearch, direction);
            if(roomPosition.x > -1 && roomPosition.y > -1) {
                mapRooms.push_back(GenerateMapRoom(roomPosition, roomSize));
                tempMapRoom.botRoom = &(mapRooms[mapRooms.size()-1]);
                mapRoom.hasExpanded = true;
                
            }
        }

        if(rand() % 100 < EXPAND_PROBABILITY_Y) {
            //expand up
            roomSize = GenerateRandomRoomSize();
            beginSearch = Vector2D(CustomClampInt(tempMapRoom.pos.x - roomSize.x + 1, 0, MAP_SIZE_X), CustomClampInt(tempMapRoom.pos.y-1, 0, MAP_SIZE_Y));
            endSearch = Vector2D(CustomClampInt(beginSearch.x + roomSize.x + tempMapRoom.size.x - 1, 0, MAP_SIZE_X), CustomClampInt(beginSearch.y - roomSize.y, 0, MAP_SIZE_Y));
            
            direction = Vector2D(1,-1);

            roomPosition = GetValidPosition(roomSize, beginSearch, endSearch, direction);
            if(roomPosition.x > -1 && roomPosition.y > -1) {
                mapRooms.push_back(GenerateMapRoom(roomPosition, roomSize));
                tempMapRoom.topRoom = &(mapRooms[mapRooms.size()-1]);
                mapRoom.hasExpanded = true;

            }
        }

    }

    bool GenerateMapStep() {
        bool result = false;
        if(currentStep == 0) {
            Vector2D roomSize = GenerateRandomRoomSize();

            Vector2D beginSearch(8,8);
            Vector2D endSearch(beginSearch.x + ROOM_MAX_SIZE, beginSearch.y + ROOM_MAX_SIZE);
            Vector2D direction(1,1);

            Vector2D roomPosition = GetValidPosition(roomSize, beginSearch, endSearch, direction);
            if(roomPosition.x > -1 && roomPosition.y > -1) {
                mapRooms.push_back(GenerateMapRoom(roomPosition, roomSize));
                result = true;
            }
            else { 
                result = false;
            }
        }
        else {
            int size = mapRooms.size();
            for(int i = currentStep-1; i < size; ++i) {
                if(mapRooms[i].hasExpanded) {
                    continue;
                }
                ExpandRoom(mapRooms[i]);
            }
            result = currentRoom < MAX_ROOMS;
        }

        currentStep++;
        return result;
    }

    void GenerateMap() {
        bool finished = false;
        while(!finished) {
            finished = !GenerateMapStep();
        }
    }

    void PrintMap() {
        cout << "PRINT MAP" << endl;
        cout << '\n' << '\n' << '\n' << "------------------------------" << endl;
        for(int i = 0; i < MAP_SIZE_Y; ++i) {
            for(int j = 0; j < MAP_SIZE_X; ++j) {
                cout << mapTiles[i][j];
            }
            cout << endl;
        }
        cout << "------------------------------" << endl;
    }
};


class MapGenerator {
public:
    MapGenerator() = default;
    ~MapGenerator() = default;

protected:
    Map map;

public:
    void Init(unsigned int seed) { srand(seed); }
    void GenerateMap() { map.GenerateMap(); }
    bool GenerateMapStep() { return map.GenerateMapStep(); }
    void PrintMap() { map.PrintMap(); }
};