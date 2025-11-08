#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <limits>
#include <format>
#include <ranges>
#include "lib.hpp"

int load(std::vector<std::vector<MAP_SIZE_t>> &stage, std::vector<TILE_NUM_t> &tiles, std::string stage_name){
    std::ifstream ifs("./stages/stage_" + stage_name + ".txt");
    if(!ifs.is_open()){
        ifs.open("../stages/stage_" + stage_name + ".txt");
        if(!ifs.is_open()){
            std::cerr << "!not exist!" << std::endl;
            return 1;
        }
    }
    
    std::string line;
    std::getline(ifs, line);
    auto iss = std::istringstream(line);
    MAP_SIZE_t width, height;
    iss >> width >> height;

    stage = std::vector<std::vector<MAP_SIZE_t>>(height+2, std::vector<MAP_SIZE_t>(width, -1));

    std::getline(ifs, line);
    iss = std::istringstream(line);
    MAP_SIZE_t goal;
    while(iss >> goal){
        if(!(0 <= goal && goal < width)){
            std::cerr << "!goal out of range!" << std::endl;
            return 1; // Invalid goal position
        }
        stage[0].at(goal) = 30;
    }
    for(MAP_SIZE_t i = 0; i < width; ++i){
        if(stage[0][i] != 30){
            stage[0][i] = 32; // Default tile
        }
    }

    std::getline(ifs, line);
    iss = std::istringstream(line);
    MAP_SIZE_t start;
    while(iss >> start){
        if(!(0 <= start && start < width)){
            std::cerr << "!start out of range!" << std::endl;
            return 1; // Invalid start position
        }
        stage[height+1].at(start) = 31;
    }
    for(MAP_SIZE_t i = 0; i < width; ++i){
        if(stage[height+1][i] != 31){
            stage[height+1][i] = 32; // Default tile
        }
    }

    for(MAP_SIZE_t i = 1; i <= height; ++i){
        for(MAP_SIZE_t j = 0; j < width; ++j){
            ifs >> stage[i][j];
        }
    }

    for(auto i = 0; i < 10; ++i){
        ifs >> tiles[i];
    }

    ifs.close();
    return 0;
}

int set_tiles(std::vector<std::vector<MAP_SIZE_t>> &stage, std::vector<TILE_NUM_t> &tiles){
    const MAP_SIZE_t WIDTH = stage[0].size();
    const MAP_SIZE_t HEIGHT = stage.size() - 2;

    std::cout << "\033[H\033[0J"; //move cursor to home position and Clear screen
    print_stage(stage);
    print_tiles(tiles);

    class Cursor{public: MAP_SIZE_t x=0,y=1;};//top-left's (x, y) = (0, 1)
    Cursor cursor;

    std::cout << "\033[3;2H";//Move cursor to top-left tile
    std::cout << "\033[7m" << tile(stage[1][0]) << "\033[0m";//Highlight
    while(1){
        unsigned char input = getch_wrapper();
        if(input == 0xe0){
            switch(getch_wrapper()){
                case 0x48:
                    input = 'w';
                    break;
                
                case 0x4b:
                    input = 'a';
                    break;
                
                case 0x50:
                    input = 's';
                    break;
                
                case 0x4d:
                    input = 'd';
                    break;
            }
        }
        switch(input){
            case 'w':
                if(cursor.y - 1 > 0){
                    std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                    cursor.y--;
                    std::cout << "\033[2A\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                }
                break;
            
            case 'a':
                if(cursor.x - 1 >= 0){
                    std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                    cursor.x--;
                    std::cout << "\033[4D\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                }
                break;
            
            case 's':
                if(cursor.y + 1 <= HEIGHT){
                    std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                    cursor.y++;
                    std::cout << "\033[2B\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                }
                break;
                
            case 'd':
                if(cursor.x + 1 < WIDTH){
                    std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                    cursor.x++;
                    std::cout << "\033[4C\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                }
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(stage[cursor.y][cursor.x] == -1){
                    std::cerr << "!illigal tile!" << std::endl;
                    return 1;
                }

                if(0 <= stage[cursor.y][cursor.x] && stage[cursor.y][cursor.x] < 10 && tiles[input - '0'] != 0){//change-able tile and useable choiced
                    if(tiles[stage[cursor.y][cursor.x]] != -1){//not inf
                        tiles[stage[cursor.y][cursor.x]]++;//useable++
                        std::cout << "\0337" << std::format("\033[{};{}H", HEIGHT*2 + 6, (stage[cursor.y][cursor.x])*4 + 2) << std::setw(3) << tiles[stage[cursor.y][cursor.x]] << "\0338";
                    }

                    stage[cursor.y][cursor.x] = input - '0';
                    std::cout << "\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";

                    if(tiles[input - '0'] != -1){//not inf
                        tiles[input - '0']--;//useable--
                        std::cout << "\0337" << std::format("\033[{};{}H", HEIGHT*2 + 6, (input - '0')*4 + 2) << std::setw(3) << tiles[input - '0'] << "\0338";
                    }
                }
                break;
            
            case ' ':
                return 0;
            
            case 0x03:// Ctrl+c
                exit(EXIT_FAILURE);
        }
    }
}

int run_stage(std::vector<std::vector<MAP_SIZE_t>> stage){
    const MAP_SIZE_t WIDTH = stage[0].size();
    const MAP_SIZE_t HEIGHT = stage.size() - 2;

    std::vector<Rat> rats;
    for(auto [i, tile]: std::ranges::enumerate_view(stage.back())){
        if(tile == 31){
            rats.emplace_back(i, HEIGHT+1);//set start position
        }
    }

    std::cout << "\033[H\033[0J"; //move cursor to home position and Clear screen
    print_stage(stage);
    std::cout << std::format("\033[{}d", 2*HEIGHT + 3);
    for(const auto& r : rats){//print rats at start
        std::cout << std::format("\033[{}G", (4*r.x + 3));
        std::cout << Color::RAT << "^" << "\033[39m";
    }

    bool is_speed_up{false};
    while(1){
        if(kbhit_wrapper()){
            switch(getch_wrapper()){
                case ' ':
                    return -1;//stop
                    break;

                case 'd':
                    is_speed_up = !is_speed_up;
                    break;
            }
        }

        if(is_speed_up){std::this_thread::sleep_for(std::chrono::milliseconds(250));}
        else{std::this_thread::sleep_for(std::chrono::milliseconds(500));}

        for(auto& r : rats){//toooo long...
            if(r.goaled){
                (r.direction += 1) %= 4;
                print_rat(r);
                continue; // Skip if the rat has already reached the goal
            }
            
            {//dead lock
                std::int8_t dx, dy;
                switch(r.direction){
                    case Direction::UP:    dx =  0; dy = -1; break;
                    case Direction::DOWN:  dx =  0; dy =  1; break;
                    case Direction::LEFT:  dx = -1; dy =  0; break;
                    case Direction::RIGHT: dx =  1; dy =  0; break;
                }
                for(auto opponent = rats.rbegin(); *opponent != r; ++opponent){
                    if(r.x+dx == opponent->x && r.y+dy == opponent->y && r.direction == (opponent->direction + 2) % 4){
                        std::cout << std::format("\033[{};{}H", (2*r.y + 1), (4*r.x + 4)) << "!";
                        std::cout << std::format("\033[{};{}H", (2*opponent->y + 1), (4*opponent->x + 4)) << "!";
                        return 1; // dead lock
                    }
                }
            }

            fix_tile_at_rat(r, stage);

            if(r.move(WIDTH, HEIGHT)){
                print_rat(r);
                std::cout << "!" << std::endl;
                return 1;// Out of bounds
            }

            {// bump
                for(auto opponent = rats.begin(); *opponent != r; ++opponent){
                    if(r.x == opponent->x && r.y == opponent->y){
                        if(stage[r.y][r.x] == 29 && r.direction != (opponent->direction)%2){//in cross, not on same line
                            continue;
                        }
                        std::cout << std::format("\033[{};{}H", (2*r.y + 1), (4*r.x + 2)) << Color::RAT << "***" "\033[39m";
                        return 1; // Bump
                    }
                }
            }

            {// tile
                if(stage[r.y][r.x] == 0){
                    //nop
                }
                else if(std::abs(stage[r.y][r.x]) <= 20){
                    if(stage[r.y][r.x]%10 == 1){
                        r.direction = Direction::UP;
                    }
                    else if(stage[r.y][r.x]%10 == 2){
                        r.direction = Direction::LEFT;
                    }
                    else if(stage[r.y][r.x]%10 == 3){
                        r.direction = Direction::DOWN;
                    }
                    else if(stage[r.y][r.x]%10 == 4){
                        r.direction = Direction::RIGHT;
                    }
                    else if(std::abs(stage[r.y][r.x])%10 < 9){
                        if(stage[r.y][r.x]%10 == 5){
                            r.direction = Direction::UP;
                        }
                        else if(stage[r.y][r.x]%10 == -5){
                            r.direction = Direction::DOWN;
                        }
                        else if(stage[r.y][r.x]%10 == 6){
                            r.direction = Direction::LEFT;
                        }
                        else if(stage[r.y][r.x]%10 == -6){
                            r.direction = Direction::RIGHT;
                        }
                        else if(stage[r.y][r.x]%10 == 7){
                            r.direction = Direction::UP;
                        }
                        else if(stage[r.y][r.x]%10 == -7){
                            r.direction = Direction::RIGHT;
                        }
                        else if(stage[r.y][r.x]%10 == 8){
                            r.direction = Direction::LEFT;
                        }
                        else if(stage[r.y][r.x]%10 == -8){
                            r.direction = Direction::DOWN;
                        }
                        stage[r.y][r.x] *= -1;
                    }
                    else if(stage[r.y][r.x]%10 == 9){
                        r.direction = (r.direction + 2)%4;//turn
                    }
                    else if(stage[r.y][r.x] == 10){
                        for(auto _ = 0; _ < 4; ++_){
                            print_rat(r);
                            (r.direction += 1) %= 4;
                            std::this_thread::sleep_for(std::chrono::milliseconds(125));
                        }
                        std::cout << "\b" "!" << std::endl;
                        return 1; // fall into a hole
                    }
                    else if(stage[r.y][r.x] == 20){
                        stage[r.y][r.x] = 10; // break tile
                    }
                }
                else if(stage[r.y][r.x] == 30){
                    r.goaled = true; // Reached the goal
                    if(std::ranges::all_of(rats, [](const Rat &rat){return rat.goaled;})){
                        print_rat(r);
                        return 0;//clear
                    }
                }
                else if(stage[r.y][r.x] == 32){
                    print_rat(r);
                    std::cout << "!" << std::endl;
                    return 1; // not a goal
                }
                else if(stage[r.y][r.x] == 21){
                    if(r.direction == Direction::DOWN){
                        r.direction = Direction::LEFT;
                    }
                    else if(r.direction == Direction::RIGHT){
                        r.direction = Direction::UP;
                    }
                    else{
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                }
                else if(stage[r.y][r.x] == 22){
                    if(r.direction == Direction::UP){
                        r.direction = Direction::LEFT;
                    }
                    else if(r.direction == Direction::RIGHT){
                        r.direction = Direction::DOWN;
                    }
                    else{
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                }
                else if(stage[r.y][r.x] == 23){
                    if(r.direction == Direction::UP){
                        r.direction = Direction::RIGHT;
                    }
                    else if(r.direction == Direction::LEFT){
                        r.direction = Direction::DOWN;
                    }
                    else{
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                }
                else if(stage[r.y][r.x] == 24){
                    if(r.direction == Direction::DOWN){
                        r.direction = Direction::RIGHT;
                    }
                    else if(r.direction == Direction::LEFT){
                        r.direction = Direction::UP;
                    }
                    else{
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                }
                else if(stage[r.y][r.x] == 25){
                    if(r.direction == Direction::LEFT || r.direction == Direction::RIGHT){
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                }
                else if(stage[r.y][r.x] == 26){
                    if(r.direction == Direction::UP || r.direction == Direction::DOWN){
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                }
                else if(stage[r.y][r.x] == 27){
                    if(r.direction == Direction::UP){
                        r.direction = Direction::LEFT;
                    }
                    else if(r.direction == Direction::DOWN){
                        r.direction = Direction::RIGHT;
                    }
                    else if(r.direction == Direction::LEFT){
                        r.direction = Direction::UP;
                    }
                    else if(r.direction == Direction::RIGHT){
                        r.direction = Direction::DOWN;
                    }
                }
                else if(stage[r.y][r.x] == 28){
                    if(r.direction == Direction::UP){
                        r.direction = Direction::RIGHT;
                    }
                    else if(r.direction == Direction::DOWN){
                        r.direction = Direction::LEFT;
                    }
                    else if(r.direction == Direction::LEFT){
                        r.direction = Direction::DOWN;
                    }
                    else if(r.direction == Direction::RIGHT){
                        r.direction = Direction::UP;
                    }
                }
                else if(stage[r.y][r.x] == 29){
                    //nop
                }
                else{
                    print_rat(r);
                    std::cout << "!invalid tile!" << std::endl;
                    return 2; // Invalid tile
                }
            }
            
            print_rat(r);
        }
    }
}

int print_stage(const std::vector<std::vector<MAP_SIZE_t>> &stage){
    const MAP_SIZE_t WIDTH = stage[0].size();
    const MAP_SIZE_t HEIGHT = stage.size() - 2;

    std::cout << "\x0e";
        std::cout << " "; for(auto &cell : stage.front()){  std::cout << tile_for_set(cell) << " ";} std::cout <<             "\n";
        std::cout << "l"; for(MAP_SIZE_t i=1; i<WIDTH; i++){std::cout << "qqq"                 "w";} std::cout << "qqq""k" << "\n";
    for(MAP_SIZE_t i=1; i<HEIGHT; i++){
        std::cout << "x"; for(auto &cell : stage[i]){       std::cout << tile_for_set(cell) << "x";} std::cout <<             "\n";
        std::cout << "t"; for(MAP_SIZE_t i=1; i<WIDTH; i++){std::cout << "qqq"                 "n";} std::cout << "qqq""u" << "\n";
    }
        std::cout << "x"; for(auto &cell : stage[HEIGHT]){  std::cout << tile_for_set(cell) << "x";} std::cout <<             "\n";
        std::cout << "m"; for(MAP_SIZE_t i=1; i<WIDTH; i++){std::cout << "qqq"                 "v";} std::cout << "qqq""j" << "\n";
        std::cout << " "; for(auto &cell : stage.back()){   std::cout << tile_for_set(cell) << " ";} std::cout << std::endl;
    std::cout << "\x0f";
    return 0;
}

int print_tiles(const std::vector<TILE_NUM_t> &tiles){
    std::cout << "  0   1   2   3   4   5   6   7   8   9  \n";
    std::cout << '|';
    for(auto i = 0; i < 10; ++i){
        std::cout << Color::USER_SET << tiles_i2s[i] << "\033[39m" << '|';
    }
    std::cout << '\n';

    std::cout << '|';
    for(auto i = 0; i < 10; ++i){
        std::cout.width(3);
        std::cout << tiles[i] << '|';
    }
    return 0;
}

int print_rat(const Rat &r){
    std::cout << std::format("\033[{};{}H", (2*r.y + 1), (4*r.x + 3));// Move cursor to rat position
    std::cout << Color::RAT;
    switch (r.direction){
        case Direction::UP:
            std::cout << "^";
            break;

        case Direction::DOWN:
            std::cout << "v";
            break;

        case Direction::LEFT:
            std::cout << "<";
            break;

        case Direction::RIGHT:
            std::cout << ">";
            break;
    }
    std::cout << "\033[39m"; // Reset color
    return 0;
}

int fix_tile_at_rat(const Rat &r, const std::vector<std::vector<MAP_SIZE_t>> &stage){
    std::cout << std::format("\033[{};{}H", (2*r.y + 1), (4*r.x + 2));
    std::cout << tile(stage[r.y][r.x]);
    return 0;
}

int Rat::move(const MAP_SIZE_t WIDTH, const MAP_SIZE_t HEIGHT){
    switch(this->direction){
        case Direction::UP:
            this->y--;
            break;

        case Direction::DOWN:
            if(this->y < HEIGHT){
                this->y++;
            }
            else{
                return 1; // Out of bounds
            }
            break;
        
        case Direction::LEFT:
            if(this->x > 0){
                this->x--;
            }
            else{
                return 1; // Out of bounds
            }
            break;

        case Direction::RIGHT:
            if(this->x < WIDTH - 1){
                this->x++;
            }
            else{
                return 1; // Out of bounds
            }
            break;
    }
    return 0;
}