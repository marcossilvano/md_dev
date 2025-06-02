SPRITE  spr_ship            "sprites/ship.png"      3 3 FAST 3
SPRITE  spr_ball            "sprites/ball.png"      2 2 FAST 3

IMAGE   img_hud           "hud/hud.png" BEST
IMAGE   img_background    "backgrounds/level1_bg.png" BEST

TILESET level1_tiles      "levels/level1/level1_tiles.png" BEST ALL
PALETTE level1_pal        "levels/level1/level1_pal.pal"
MAP     level1_map        "levels/level1/level1_map.tmx" map_layer BEST 0

OBJECTS level1_objects    "levels/level1/level1_map.tmx" objects_layer "room:u16;x:f32;y:f32;speed_x:f16;speed_y:f16" "sortby:room"