----------------
title: mouse't go on //"'" = "\x27"
Author: Inui Yayoi
date: 2025-09-06
----------------
shortcut "main" is entry


# load stage
  ">" mean request input
  input stage name (first, practice stage name "1" and nexts)

>{stage_name}

  loading... (if load failed, stage name is invalid or stages dir is no exist in correct)

  print stage and enter tile setting!


      x0      x1      x2      x3      x4      x5      x6      x7      x8      x9    

    ＿＿＿                                                                           <- goal
  ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
  │      │      │      │      │      │      │      │      │      │      │
0x│■■■│ ^  ^ │ <  < │ V  V │ >  > │ ^  V │ <  > │ ^  > │ <  V │ @  @ │ <- tiles
  │      │      │      │      │      │      │      │      │      │      │
  ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
  │      │      │      │      │      │      │      │      │      │      │
1x│ XXX  │ ^─^ │ <─< │ V─V │ >─> │ ^─V │ <─> │ ^─> │ <─V │ @─@ │
  │      │      │      │      │      │      │      │      │      │      │
  ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
  │      │  │  │      │      │  │  │  │  │      │￥    │    ／│  │  │
2x│ ###  │─┘  │─┐  │  ┌─│  └─│  │  │───│  ￥  │  ／  │─│─│
  │      │      │  │  │  │  │      │  │  │      │    ￥│／    │  │  │
  └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
    ￣￣￣                                                                           <- start


      0       1       2       3       4       5       6       7       8       9

  │      │      │      │      │      │      │      │      │      │      │
  │      │ ^  ^ │ <  < │ V  V │ >  > │ ^  V │ <  > │ ^  > │ <  V │ @  @ │
  │      │      │      │      │      │      │      │      │      │      │

  │      │      │      │      │      │      │      │      │      │      │
  │   -1 │   10 │   10 │   10 │   10 │    0 │    0 │    0 │    0 │    0 │
  │      │      │      │      │      │      │      │      │      │      │

(if lost this image, タイル案等.png help you)
(or set your font monospaced)

# tile setting
  at first, top-left color reverse area is you
  WASD: move
  0-9: set tile
  space: go to run

  at top, under-line"s" mean goal (num 30)
  at botom, over-line"s" mean start (num 31)
  (both, none of tiles have num 32 in program)

  00: none, 01: face up, 02: face left, 03: face down, 04: face right, 05:up-down flip, 06: left-right flip, 07: up-right flip, 08: left-down flip, 09: turn back,
  10: hole (can't cross), 11-19: same 01-09 but can't change,
  20: glass (one time only), 21: left-up cross, 22: left-down cross, 23: down-right cross, 24: right-up cross, 25: up-down cross, 26: left-right cross, 27: up-right left-down cross, 28: up-left down-right cross, 29: over cross
  (negative x5-x8 mean reverse but NOT arrowed)
  00-09 is change-able tiles

  under the stage, number mean useable tiles
  -1 mean inf


# running rats
  "^" mean rats
  go ahead only head
  go out of stage is disable exclusive goal
  So, go no goal position is disable!
  In addition, dump or same cell between rats is disable!! (Specially, 29 is able 'ONLY' crossing)

  in x5-x8, left mark is now direction
  in 21-26, enter not connect is disable

  running quit when all rats goaled or miss or input space (space mean stop running)


# after running
  if miss, wait enter and go back to setting tiles
  for exit, rerun and stop
  else ">" print

>{e|stage name|other}

  "e" mean exit, finish exe
  else other, 
  if stop, go back to setting tiles
  else if clear, input stage name to load stage



have fun!!