#include <genesis.h>
#include <string.h>
#include <resource.h>
#define ANIM_RUN 0
#define ANIM_JUMP 1

fix16 player_vel_y = FIX16(0);
int player_height = 16;
const int floor_height = 128;
fix16 gravity = FIX16(0.2);
bool jumping = FALSE;

// general stuff
const char msg_start[22] = "Press START to begin!";
const char msg_reset[22] = "Press START to reset!";
bool game_on = FALSE;
const int scrollSpeed = 2;

Sprite *player;
const int player_x = 32;
fix16 player_y = FIX16(112);

Sprite *obstacle;
int obstacle_x = 320;
int obstacle_vel_x = 0;

int score = 0;
char label_score[6] = "SCORE\0";
char str_score[3] = "0";
bool score_added = FALSE;

void updateScoreDisplay()
{
    sprintf(str_score, "%d", score);
    VDP_clearText(1, 2, 3);
    VDP_drawText(str_score, 10, 2);
}

void showText(char s[])
{
    VDP_drawText(s, 20 - strlen(s) / 2, 10);
}

void clearText()
{
    VDP_clearText(0, 10, 32);
}

void startGame()
{
    if (game_on == FALSE)
    {
        game_on = TRUE;
        clearText();
        VDP_drawText(label_score, 10, 1);
        score = 0;
        updateScoreDisplay();
        obstacle_x = 320;
    }
}

void endGame()
{
    if (game_on == TRUE)
    {
        showText(msg_reset);
        game_on = FALSE;
    }
}

void myJoyHandler(u16 joy, u16 changed, u16 state)
{
    if (joy == JOY_1)
    {
        if (state & BUTTON_START)
        {
            startGame();
        }
    }
    if (state & BUTTON_C)
    {
        // прыжок
        if (jumping == FALSE)
        {
            jumping = TRUE;
            player_vel_y = FIX16(-4);
            SPR_setAnim(player, ANIM_JUMP);
        }
    }
}

int main()
{
    PAL_setColor(PAL0, RGB24_TO_VDPCOLOR(0x6dc2ca));
    JOY_init();
    JOY_setEventHandler(&myJoyHandler);

    VDP_setPlaneSize(32, 32, FALSE);

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    VDP_loadTileSet(floor.tileset, 1, DMA);
    VDP_loadTileSet(wall.tileset, 2, DMA);
    VDP_loadTileSet(light.tileset, 3, DMA);
    PAL_setPalette(PAL1, light.palette->data, DMA);
    PAL_setPalette(PAL2, runner.palette->data, DMA);
    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), 0, 16, 32, 1);
    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 2), 0, 17, 32, 14);

    VDP_fillTileMapRectInc(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 3), 15, 13, 2, 3);

    SPR_init();
    player = SPR_addSprite(&runner, player_x, player_y, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    obstacle = SPR_addSprite(&rock, obstacle_x, 128, TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setAnim(player, ANIM_RUN);

    showText(msg_start);

    int offset = 0;

    while (1)
    {
        if (game_on == TRUE)
        {
            SPR_update();
            // проекрутка фона
            obstacle_vel_x = -scrollSpeed;
            obstacle_x = obstacle_x + obstacle_vel_x;
            if (obstacle_x < -8)
            {
                obstacle_x = 320;
            }

            VDP_setHorizontalScroll(BG_B, offset -= scrollSpeed);

            if (offset <= -256)
            {
                offset = 0;
            }

            player_y = player_y + player_vel_y;

            // применяем гравитацибюю
            if (jumping == TRUE)
                player_vel_y = player_vel_y + gravity;

            if (jumping == TRUE && F16_toInt(player_y) + player_height >= (floor_height))
            {
                score_added = FALSE;

                jumping = FALSE;
                player_vel_y = FIX16(0);
                player_y = FIX16(floor_height - player_height);
                SPR_setAnim(player, ANIM_RUN);
            }

            if (player_x < obstacle_x + 8 && player_x + 8 > obstacle_x)
            {
                if (jumping == FALSE)
                {
                    endGame();
                }
                else
                {
                    if (score_added == FALSE)
                    {
                        score++;
                        updateScoreDisplay();
                        score_added = TRUE;
                    }
                }
            }

            SPR_setPosition(player, player_x, F16_toInt(player_y));
            SPR_setPosition(obstacle, obstacle_x, 120);
        }

        SYS_doVBlankProcess();
    }
    return (0);
}
