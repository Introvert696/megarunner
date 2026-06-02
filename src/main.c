#include <genesis.h>
#include <string.h>

// general stuff
const char msg_start[22] = "Press START to begin!";
const char msg_reset[22] = "Press START to reset!";
bool game_on = FALSE;

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
    }
}

int main()
{
    JOY_init();
    JOY_setEventHandler(&myJoyHandler);
    showText(msg_start);

    while (1)
    {
        if (game_on == TRUE)
        {
            // тут игра
        }

        SYS_doVBlankProcess();
    }
    return (0);
}
