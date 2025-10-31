#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <format>
#include "lib_stage_maker.hpp"

int main(){
    std::cout << "\033(B\033)0";//!
    std::atexit([]{std::cout << "\033[!p";});
    std::vector<std::vector<MAP_SIZE_t>> stage{{32}, {0}, {32}};
    std::vector<TILE_NUM_t> tiles(10, 0);
    std::string stage_name;

    std::cout << ">";
    std::cin >> stage_name;
    if(std::filesystem::exists("../stages/stage_" + stage_name + ".txt")){
        std::cout << "the file is exist. continue?" << std::endl;
        if(hear_continue() == true){
            load(stage, tiles, stage_name);
        }
        else{
            exit(EXIT_SUCCESS);
        }
    }

    std::cout << "\033[?25l"; // Hide cursor
    while(1){
        if(set_tiles(stage, tiles)){
            std::cerr << "Error: Failed to set tiles." << std::endl;
            exit(EXIT_FAILURE);
        }
        
        auto ret = run_stage(stage);
        std::cout << std::format("\033[{};1H", stage.size()*2); // Move cursor to the bottom
        std::cout << "\033[?25h"; // Show cursor
        if(ret == 0){//clear
            std::cout << "!!!!" << std::endl;
            std::cout << ">";
            std::cin.seekg(std::ios_base::end);
            if(std::cin.get() == 'e'){
                std::cin.seekg(std::ios_base::end);
                break;
            }
            else{
                std::cin.seekg(std::ios_base::end);
                std::cout << "\033[?25l"; // Hide cursor
                continue;
            }
        }
        else if(ret == 1){//miss
            std::cout << "!?!?";
            WAITTING_FOR_ENTER;
            std::cout << "\033[?25l"; // Hide cursor
            continue;
        }
        else if(ret == -1){//stop
            std::cout << "????" << std::endl;
            std::cout << ">";
            std::cin.seekg(std::ios_base::end);
            if(std::cin.get() == 'e'){
                std::cin.seekg(std::ios_base::end);
                exit(EXIT_SUCCESS);
            }
            else{
                std::cin.seekg(std::ios_base::end);
                std::cout << "\033[?25l"; // Hide cursor
                continue;
            }
        }
        else if(ret == 2){
            std::cerr << "Error: Rats in illegal positions." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    if(save_stage(stage, tiles, stage_name)){
        std::cerr << "Error: Failed to save stage data." << std::endl;
        exit(EXIT_FAILURE);

    }

    return 0;
}

bool hear_continue(){
    std::cout << "\033[7m" "no" "\033[0m"  " yes";

    bool answer = false;
    while(1){
        unsigned char input = getch_wrapper();
        if(input == '\r' || input == ' '){//enter or space
            break;
        }
        else if(input == 0xe0){
            switch(getch_wrapper()){
                case 0x4b:
                    input = 'a';
                    break;
                
                case 0x4d:
                    input = 'd';
                    break;
            }
        }
        switch(input){
            case 'a':
                answer = false;
                std::cout << "\033[G";
                std::cout << "\033[7m" "no" "\033[0m"  " yes";
                break;
            
            case 'd':
                answer = true;
                std::cout << "\033[G";
                std::cout << "no "  "\033[7m" "yes" "\033[0m";
                break;
            
            case 0x03:
                exit(EXIT_FAILURE);
        }
    }

    return answer;
}


int load(std::vector<std::vector<MAP_SIZE_t>> &stage, std::vector<TILE_NUM_t> &tiles, std::string stage_name){
    std::ifstream ifs("stages/stage_" + stage_name + ".txt");
    if(!ifs.is_open()){
        ifs.open("../stages/stage_" + stage_name + ".txt");
        if(!ifs.is_open()){
            std::cerr << "!can't open!" << std::endl;
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
    MAP_SIZE_t WIDTH = stage[0].size();
    MAP_SIZE_t HEIGHT = stage.size() - 2;

    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
    print_stage(stage);
    print_tiles(tiles);

    class Cursor{public: MAP_SIZE_t x=0,y=0;};//top-left's (x, y) = (0, 1)
    Cursor cursor;

    std::cout << "\033[;2H";//Move cursor to top-left tile
    std::cout << "\033[7m" << tile(stage[0][0]) << "\033[0m";//Highlight
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
                if(cursor.y > 0){
                    std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                    cursor.y--;
                    std::cout << "\033[2A\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                }
                else if(cursor.y == -1){
                    std::cout << "\033[3D" << std::setw(3) << tiles[cursor.x];
                    cursor.x = 0;
                    cursor.y = HEIGHT + 1;
                    std::cout << std::format("\033[{};2H", HEIGHT*2 + 3);//move start-line left
                    std::cout << "\033[7m" << tile(stage[HEIGHT + 1][0]) << "\033[0m";//Highlight
                }
                break;
            
            case 'a':
                if(cursor.y == -1){//in tiles line
                    if(cursor.x > 0){
                        std::cout << "\033[3D" << std::setw(3) << tiles[cursor.x];
                        cursor.x--;
                        std::cout << "\033[4D\033[3D\033[7m" << std::setw(3) << tiles[cursor.x] << "\033[0m";//Highlight

                    }
                }
                else{
                    if(cursor.x > 0){
                        std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                        cursor.x--;
                        std::cout << "\033[4D\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                    }
                }
                break;
            
            case 's':
                if(cursor.y == -1){}
                else if(cursor.y < HEIGHT + 1){
                    std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                    cursor.y++;
                    std::cout << "\033[2B\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                }
                else if(cursor.y == HEIGHT + 1){//junp to amount of tiles
                    std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                    cursor.x = 0;
                    cursor.y = -1;
                    std::cout << std::format("\033[{};2H", HEIGHT*2 + 6);//move tiles left
                    std::cout << "\033[7m" << std::setw(3) << tiles[0] << "\033[0m";//Highlight
                }
                break;
                
            case 'd':
                if(cursor.y == -1){//in tiles line
                    if(cursor.x < 9){
                        std::cout << "\033[3D" << std::setw(3) << tiles[cursor.x];
                        cursor.x++;
                        std::cout << "\033[4C\033[3D\033[7m" << std::setw(3) << tiles[cursor.x] << "\033[0m";//Highlight
                    }
                }
                else{
                    if(cursor.x < WIDTH - 1){
                        std::cout << "\033[3D" << tile(stage[cursor.y][cursor.x]);
                        cursor.x++;
                        std::cout << "\033[4C\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";//Highlight
                    }
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
                if(0 < cursor.y && cursor.y <= HEIGHT){//in stage
                    input = (input - '0');
                    std::cout << "\033[2D\033[7m" << static_cast<short>(input) << "\033[C\033[0m";
                    input *= 10;
                    input += getch_wrapper() - '0';

                    if(0 <= input && input < 30){
                        stage[cursor.y][cursor.x] = input;
                    }
                    else{
                        stage[cursor.y][cursor.x] = -1;
                    }
                    std::cout << "\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";
                }
                else if(cursor.y == -1){//on tiles line
                    input = (input - '0');
                    std::cout << "\033[3D\033[7m" "  " << static_cast<TILE_NUM_t>(input) << "\033[0m";
                    input *= 10;
                    input += getch_wrapper() - '0';
                    if(!(0 <= input && input < 100)){//not in 2 en
                        std::cout << "\033[3D\033[7m" "???" "\033[0m";
                        break;
                    }
                    tiles[cursor.x] = input;
                    std::cout << "\033[3D\033[7m" << std::setw(3) << tiles[cursor.x] << "\033[0m";
                }
                else if(cursor.y == 0){//on goal line
                    if(input == '0'){
                        stage[0][cursor.x] = 32;
                    }
                    else{
                        stage[0][cursor.x] = 30;
                    }
                    std::cout << "\033[3D\033[7m" << tile(stage[0][cursor.x]) << "\033[0m";
                }
                else if(cursor.y == HEIGHT + 1){//on start line
                    if(input == '0'){
                        stage[HEIGHT + 1][cursor.x] = 32;
                    }
                    else{
                        stage[HEIGHT + 1][cursor.x] = 31;
                    }
                    std::cout << "\033[3D\033[7m" << tile(stage[HEIGHT + 1][cursor.x]) << "\033[0m";
                }
                break;

            case '+':
                if(cursor.y != -1){//in map
                    for(auto &line : stage){
                        line.push_back(0);
                    }
                    stage[0].back() = 32;
                    stage[HEIGHT + 1].back() = 32;
                    WIDTH++;
                    std::cout << "\0337";
                    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
                    print_stage(stage);
                    print_tiles(tiles);

                    std::cout << "\0338";
                    std::cout << "\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";
                }
                else{
                    if(tiles[cursor.x] < 999){//in 3 en
                        tiles[cursor.x]++;
                        std::cout << "\033[3D\033[7m" << std::setw(3) << tiles[cursor.x] << "\033[0m";
                    }
                }
                break;

            case '-':
                if(cursor.y != -1){//in map
                    if(WIDTH <= 1){
                        break;
                    }
                    for(auto &line : stage){
                        line.pop_back();
                    }
                    WIDTH--;
                    std::cout << "\0337";
                    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
                    print_stage(stage);
                    print_tiles(tiles);

                    std::cout << "\0338";
                    if(cursor.x >= WIDTH){
                        cursor.x--;
                        std::cout << "\033[4D";
                    }
                    std::cout << "\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";
                }
                else{
                    if(-1 < tiles[cursor.x]){//in 2 en
                        tiles[cursor.x]--;
                        std::cout << "\033[3D\033[7m" << std::setw(3) << tiles[cursor.x] << "\033[0m";
                    }
                }
                break;
            
            case '*':
                if(cursor.y != -1){//in map
                    stage.insert(stage.end() - 1, std::vector<MAP_SIZE_t>(WIDTH, 0));
                    HEIGHT++;
                    std::cout << "\0337";
                    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
                    print_stage(stage);
                    print_tiles(tiles);

                    std::cout << "\0338";
                    std::cout << "\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";
                }
                break;
            
            case '/':
                if(HEIGHT <= 1){
                    break;
                }
                if(cursor.y != -1){//in map
                    stage.erase(stage.end() - 2);
                    HEIGHT--;
                    std::cout << "\0337";
                    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
                    print_stage(stage);
                    print_tiles(tiles);
                    
                    std::cout << "\0338";
                    if(cursor.y > HEIGHT+1){
                        cursor.y--;
                        std::cout << "\033[2A";
                    }
                    std::cout << "\033[3D\033[7m" << tile(stage[cursor.y][cursor.x]) << "\033[0m";
                }
                break;
            
            case ' ':
                return 0;
            
            case 0x03:
                exit(EXIT_FAILURE);
        }
    }
}

int run_stage(std::vector<std::vector<MAP_SIZE_t>> stage){
    const MAP_SIZE_t WIDTH = stage[0].size();
    const MAP_SIZE_t HEIGHT = stage.size() - 2;

    std::vector<Rat> rats;
    for(int i = 0; i < WIDTH; ++i){
        if(stage[HEIGHT+1][i] == 31){
            rats.emplace_back(i, HEIGHT+1);//set start position
        }
    }

    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
    print_stage(stage);
    for(const auto& r : rats){//print rats at start
        std::cout << std::format("\033[{};{}H", (2*r.y + 1), (4*r.x + 3));
        std::cout << Color::RAT << "^" << "\033[39m";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    while(1){
        if(kbhit_wrapper()){
            if(getch_wrapper() == ' '){
                return -1;//stop
            }
        }

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
                        return 1; // Bump
                    }
                }
            }

            fix_tile_at_rat(rats, r, stage);

            {//move
                if(r.direction == Direction::UP){
                    if(r.y > 0){
                        r.y--;
                    }
                    else{
                        print_rat(r);
                        std::cerr << "?Were are you now?" << std::endl;
                        return 2; // illigal position
                    }
                }
                else if(r.direction == Direction::DOWN){
                    if(r.y < HEIGHT){
                        r.y++;
                    }
                    else{
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // Out of bounds
                    }
                }
                else if(r.direction == Direction::LEFT){
                    if(r.x > 0){
                        r.x--;
                    }
                    else{
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // Out of bounds
                    }
                }
                else if(r.direction == Direction::RIGHT){
                    if(r.x < WIDTH - 1){
                        r.x++;
                    }
                    else{
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // Out of bounds
                    }
                }
            }

            {//bump
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

            {//tile
                if(stage[r.y][r.x] == 0){
                    //nop
                }
                else if(0 < std::abs(stage[r.y][r.x]) && std::abs(stage[r.y][r.x]) <= 20){
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
                    else if(5 <= std::abs(stage[r.y][r.x])%10 && std::abs(stage[r.y][r.x])%10 < 9){
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
                        if(r.direction == Direction::UP){
                            r.direction = Direction::DOWN;
                        }
                        else if(r.direction == Direction::DOWN){
                            r.direction = Direction::UP;
                        }
                        else if(r.direction == Direction::LEFT){
                            r.direction = Direction::RIGHT;
                        }
                        else if(r.direction == Direction::RIGHT){
                            r.direction = Direction::LEFT;
                        }
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
                    for(auto opponent = rats.begin(); opponent != rats.end(); ++opponent){
                        if(!opponent->goaled){continue;}//and skip myself
                        if(r.x == opponent->x && r.y == opponent->y){
                            std::cout << std::format("\033[{};{}H", (2*r.y + 1), (4*r.x + 2)) << Color::RAT << "***" "\033[39m";
                            return 1; // Bump
                        }
                    }
                    r.goaled = true; // Reached the goal
                    if(std::all_of(rats.begin(), rats.end(), [](const Rat &rat){return rat.goaled;})){
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
                    if(r.direction == Direction::UP || r.direction == Direction::LEFT){
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                    else if(r.direction == Direction::DOWN){
                        r.direction = Direction::LEFT;
                    }
                    else if(r.direction == Direction::RIGHT){
                        r.direction = Direction::UP;
                    }
                }
                else if(stage[r.y][r.x] == 22){
                    if(r.direction == Direction::DOWN || r.direction == Direction::LEFT){
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                    else if(r.direction == Direction::UP){
                        r.direction = Direction::LEFT;
                    }
                    else if(r.direction == Direction::RIGHT){
                        r.direction = Direction::DOWN;
                    }
                }
                else if(stage[r.y][r.x] == 23){
                    if(r.direction == Direction::DOWN || r.direction == Direction::RIGHT){
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                    else if(r.direction == Direction::UP){
                        r.direction = Direction::RIGHT;
                    }
                    else if(r.direction == Direction::LEFT){
                        r.direction = Direction::DOWN;
                    }
                }
                else if(stage[r.y][r.x] == 24){
                    if(r.direction == Direction::UP || r.direction == Direction::RIGHT){
                        print_rat(r);
                        std::cout << "!" << std::endl;
                        return 1; // hit a wall
                    }
                    else if(r.direction == Direction::DOWN){
                        r.direction = Direction::RIGHT;
                    }
                    else if(r.direction == Direction::LEFT){
                        r.direction = Direction::UP;
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
                    return 1; // Invalid tile
                }
            }
            
            if(print_rat(r)){return 1;}
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int save_stage(std::vector<std::vector<MAP_SIZE_t>> &stage, std::vector<TILE_NUM_t> &tiles, std::string &stage_name){
    const MAP_SIZE_t WIDTH = stage[0].size();
    const MAP_SIZE_t HEIGHT = stage.size() - 2;

    if(stage_name.ends_with("_MA")){
        stage_name.pop_back();//A
        stage_name.pop_back();//M
        stage_name.pop_back();//_
    }

    std::ofstream ofsMA("../stages/stage_" + stage_name + "_MA.txt");
    std::ofstream ofs("../stages/stage_" + stage_name + ".txt");

    ofsMA << std::format("{:02} {:02}", WIDTH, HEIGHT) << "\n";
    ofs << std::format("{:02} {:02}", WIDTH, HEIGHT) << "\n";

    //goal num write
    for(auto it = stage[0].begin(); it != stage[0].end(); ++it){
        if(*it == 30){
            ofsMA << std::format("{:02} ", it - stage[0].begin());
            ofs << std::format("{:02} ", it - stage[0].begin());
        }
    }
    ofsMA << "\n";
    ofs << "\n";

    //start num write
    for(auto it = stage[HEIGHT + 1].begin(); it != stage[HEIGHT + 1].end(); ++it){
        if(*it == 31){
            ofsMA << std::format("{:02} ", it - stage[HEIGHT + 1].begin());
            ofs << std::format("{:02} ", it - stage[HEIGHT + 1].begin());
        }
    }
    ofsMA << "\n\n";
    ofs << "\n\n";


    //stage write
    for(auto line = stage.begin() + 1; line != stage.end() - 1; ++line){
        for(auto &tile : *line){
            ofsMA << std::format("{:02} ", tile);
            if(0 <= tile && tile < 10){ofs << "00 ";}
            else{ofs << std::format("{:02} ", tile);}
        }
        ofsMA << "\n";
        ofs << "\n";
    }
    ofsMA << "\n\n";
    ofs << "\n\n";


    //amount of tile write
    for(auto &num : tiles){
        ofsMA << std::format("{:02} ", num);
        ofs << std::format("{:02} ", num);
    }

    ofsMA.flush();
    ofs.flush();
    return 0;
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
    std::cout << Color::USER_SET.data() << '|';
    for(auto i = 0; i < 10; ++i){
        std::cout << tiles_i2s[i] << '|';
    }
    std::cout << "\033[39m" << "\n";

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
        
        default:
            std::cout << "!";
            std::cerr << "!invalid direction!" << std::endl;
            return 1; // Invalid direction
    }
    std::cout << "\033[39m"; // Reset color
    return 0;
}

int fix_tile_at_rat(const std::vector<Rat> &rats, const Rat &r, const std::vector<std::vector<MAP_SIZE_t>> &stage){
    for(const auto& other_rat : rats){
        if(other_rat == r){continue;}//skip myself
        if(other_rat.x == r.x && other_rat.y == r.y){
            return 0;
        }
    }
    std::cout << std::format("\033[{};{}H", (2*r.y + 1), (4*r.x + 2));
    std::cout << tile(stage[r.y][r.x]);
    return 0;
}