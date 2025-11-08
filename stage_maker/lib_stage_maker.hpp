#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <conio.h>//!win


#define WAITTING_FOR_ENTER std::cin.seekg(std::ios_base::end);std::cin.get();std::cin.seekg(std::ios_base::end)
using MAP_SIZE_t = std::int16_t;
using TILE_NUM_t = std::int32_t;//size of MAP_SIZE_t * MAP_SIZE_t

namespace Color{
    constexpr char
    INVALID[] = "\033[31m",//red
    USER_SET[] = "\033[37m",//white
    NO_CHANGE[] = "\033[90m",//gray
    RAT[] = "\033[32m";//green
}

namespace Direction{
    constexpr std::int8_t
    UP = 0,
    LEFT = 1,
    DOWN = 2,
    RIGHT = 3;
}

class Rat{
    public:
        bool goaled = false;
        MAP_SIZE_t x, y;
        std::int8_t direction = Direction::UP;

        Rat() = delete;
        Rat(MAP_SIZE_t x, MAP_SIZE_t y) : x(x), y(y) {}
        int move(const MAP_SIZE_t WIDTH, const MAP_SIZE_t HEIGHT);

        friend bool operator==(const Rat &rat1, const Rat &rat2){
            return &rat1 == &rat2;
        }
        friend bool operator!=(const Rat &rat1, const Rat &rat2){
            return !(rat1 == rat2);
        }
};

inline std::unordered_map<std::int8_t, std::string> tiles_i2s{
    {- 1, "???" },//error
    {  0, "   " },//default
    {  1, "^ ^" },//up user-set
    {  2, "< <" },//left user-set
    {  3, "V V" },//down user-set
    {  4, "> >" },//right user-set
    {  5, "^ V" },//up-down user-set
    {- 5, "V ^" },//down-up user-set
    {  6, "< >" },//left-right user-set
    {- 6, "> <" },//right-left user-set
    {  7, "^ >" },//up-right user-set
    {- 7, "> ^" },//right-up user-set
    {  8, "< V" },//left-down user-set
    {- 8, "V <" },//down-left user-set
    {  9, "@ @" },//turn user-set
    { 10, "XXX" },//hole
    { 11, "^-^" },//up no-change
    { 12, "<-<" },//left no-change
    { 13, "V-V" },//down no-change
    { 14, ">->" },//right no-change
    { 15, "^-V" },//up-down no-change
    {-15, "V-^" },//down-up no-change
    { 16, "<->" },//left-right no-change
    {-16, ">-<" },//right-left no-change
    { 17, "^->" },//up-right no-change
    {-17, ">-^" },//right-up no-change
    { 18, "<-V" },//left-down no-change
    {-18, "V-<" },//down-left no-change
    { 19, "@-@" },//turn no-change
    { 20, "###" },//one time
    { 21, "qj " },//left-up corner
    { 22, "qk " },//left-down corner
    { 23, " lq" },//right-up corner
    { 24, " mq" },//right-down corner
    { 25, " x " },//up-down corner
    { 26, "qqq" },//left-right corner
    { 27, " \\ "},//left-down right-up corner
    { 28, " / " },//left-up right-down corner
    { 29, "qxq" },//cross
    { 30, "sss" },//goal
    { 31, "ooo" },//start
    { 32, "   " },//not goal
};

inline std::string tile_for_set(const short &num){
    if(num == -1){
        return Color::INVALID + tiles_i2s[-1] + "\033[39m";
    }
    else if(num == 0){
        //nop
    }
    else if(0 < num && num < 10){
        return Color::USER_SET + tiles_i2s[num] + "\033[39m";
    }
    else if(10 <= num && num < 30){
        return Color::NO_CHANGE + tiles_i2s[num] + "\033[39m";
    }
    else if(30 <= num && num <= 32){
        //nop
    }
    else{
        return tiles_i2s[-1] + "\033[39m";
    }
    return tiles_i2s[num];
}

inline std::string tile(const short &num){
    if(num == -1){
        return Color::INVALID + tiles_i2s[-1] + "\033[39m";
    }
    else if(num == 0){
        //nop
    }
    else if(0 < std::abs(num) && std::abs(num) < 10){
        return Color::USER_SET + tiles_i2s[num] + "\033[39m";
    }
    else if(10 <= std::abs(num) && std::abs(num) <= 20){
        return Color::NO_CHANGE + tiles_i2s[num] + "\033[39m";
    }
    else if(20 < num && num < 30){
        using namespace std::string_literals;
        return "\x0e"s + Color::NO_CHANGE + tiles_i2s[num] + "\033[39m" + "\x0f";
    }
    else if(30 <= num && num < 32){
        return "\x0e" + tiles_i2s[num] + "\x0f";
    }
    else if(num == 32){
        //nop
    }
    else{
        return tiles_i2s[-1] + "\033[39m";
    }
    return tiles_i2s[num];
}

inline int kbhit_wrapper(){return _kbhit();}//!win
inline int getch_wrapper(){return _getch();}//!win
bool hear_continue();
int load(std::vector<std::vector<MAP_SIZE_t>> &stage, std::vector<TILE_NUM_t> &tiles, std::string stage_name = "0");
int set_tiles(std::vector<std::vector<MAP_SIZE_t>> &stage, std::vector<TILE_NUM_t> &tiles);
int run_stage(std::vector<std::vector<MAP_SIZE_t>> stage);
int save_stage(std::vector<std::vector<MAP_SIZE_t>> &stage, std::vector<TILE_NUM_t> &tiles, std::string &stage_name);
int print_stage(const std::vector<std::vector<MAP_SIZE_t>> &stage);
int print_tiles(const std::vector<TILE_NUM_t> &tiles);
int print_rat(const Rat &r);
int fix_tile_at_rat(const Rat &r, const std::vector<std::vector<MAP_SIZE_t>> &stage);