#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <format>
#include "lib.hpp"

int main(void){
    std::cout << "\033]0;mouse't go on\a";
    std::cout << "\033(B\033)0";//!
    std::atexit([]{std::cout << "\033[!p";});
    std::vector<std::vector<MAP_SIZE_t>> stage;
    std::vector<TILE_NUM_t> tiles(10);
    std::string stage_name;

    while(1){
        std::cout << ">";
        std::cin >> stage_name;
        if(stage_name == "e"){exit(EXIT_SUCCESS);}
        if(load(stage, tiles, stage_name)){
            std::cerr << "Error: Failed to load stage data." << std::endl;
            WAITTING_FOR_ENTER;
            exit(EXIT_FAILURE);
        }

        std::cout << "\033[?25l"; // Hide cursor
        while(1){
            if(set_tiles(stage, tiles)){
                std::cerr << "Error: Failed to set tiles." << std::endl;
                WAITTING_FOR_ENTER;
                exit(EXIT_FAILURE);
            }
            
            auto ret = run_stage(stage);
            std::cout << std::format("\033[{};1H", stage.size()*2); // Move cursor to the bottom
            std::cout << "\033[?25h"; // Show cursor
            if(ret == 0){//clear
                std::cout << "!!!!" << std::endl;
                break;
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
                WAITTING_FOR_ENTER;
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}