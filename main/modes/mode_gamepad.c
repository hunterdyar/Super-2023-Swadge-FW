//==============================================================================
// Includes
//==============================================================================

#include <stdlib.h>

#include "esp_timer.h"
#include "esp_log.h"
#include "tinyusb.h"
#include "tusb_hid_gamepad.h"

#include "bresenham.h"
#include "swadge_esp32.h"
#include "swadgeMode.h"

#include "mode_gamepad.h"
#include "mode_main_menu.h"

//==============================================================================
// Defines
//==============================================================================

#define Y_OFF 20

#define DPAD_BTN_RADIUS     16
#define DPAD_CLUSTER_RADIUS 45

#define START_BTN_RADIUS 10
#define START_BTN_SEP     2

#define AB_BTN_RADIUS 25
#define AB_BTN_Y_OFF   8
#define AB_BTN_SEP     2

#define ACCEL_BAR_H       8
#define ACCEL_BAR_SEP     1
#define MAX_ACCEL_BAR_W 100

#define EXIT_TIME_US 1000000

//==============================================================================
// Functions Prototypes
//==============================================================================

void gamepadEnterMode(display_t* disp);
void gamepadExitMode(void);
void gamepadMainLoop(int64_t elapsedUs);
void gamepadButtonCb(buttonEvt_t* evt);
void gamepadTouchCb(touch_event_t* evt);
void gamepadAccelCb(accel_t* accel);

//==============================================================================
// Variables
//==============================================================================

typedef struct
{
    int64_t time_exit_pressed;
    display_t* disp;
    hid_gamepad_report_t gpState;
    font_t ibmFont;
    bool drawDisp;
    bool isPluggedIn;
} gamepad_t;

gamepad_t* gamepad;

swadgeMode modeGamepad =
{
    .modeName = "Gamepad",
    .fnEnterMode = gamepadEnterMode,
    .fnExitMode = gamepadExitMode,
    .fnMainLoop = gamepadMainLoop,
    .fnButtonCallback = gamepadButtonCb,
    .fnTouchCallback = gamepadTouchCb,
    .wifiMode = NO_WIFI,
    .fnEspNowRecvCb = NULL,
    .fnEspNowSendCb = NULL,
    .fnAccelerometerCallback = gamepadAccelCb,
    .fnAudioCallback = NULL,
    .fnTemperatureCallback = NULL
};

//==============================================================================
// Functions
//==============================================================================

/**
 * Enter the gamepad mode, allocate memory, intialize USB
 */
void gamepadEnterMode(display_t* disp)
{
    // Allocate memory for this mode
    gamepad = (gamepad_t*)calloc(1, sizeof(gamepad_t));

    // Save a pointer to the display
    gamepad->disp = disp;
    gamepad->drawDisp = true;

    // Load the font
    loadFont("ibm_vga8.font", &(gamepad->ibmFont));
}

/**
 * Exit the gamepad mode and free memory
 */
void gamepadExitMode(void)
{
    freeFont(&(gamepad->ibmFont));
    free(gamepad);
}

/**
 * Draw the gamepad state to the display when it changes
 *
 * @param elapsedUs unused
 */
void gamepadMainLoop(int64_t elapsedUs __attribute__((unused)))
{
    // Check if plugged in or not
    if(tud_ready() != gamepad->isPluggedIn)
    {
        gamepad->drawDisp = true;
        gamepad->isPluggedIn = tud_ready();
    }

    // If there is something to draw
    if(gamepad->drawDisp || (0 != gamepad->time_exit_pressed))
    {
        // Lower the flag to draw for the next loop
        gamepad->drawDisp = false;

        // Clear the display
        fillDisplayArea(gamepad->disp, 0, 0, gamepad->disp->w, gamepad->disp->h, c213);

        // Always Draw some reminder text, centered
        const char reminderText[] = "A + B + Start + Select to Exit";
        int16_t tWidth = textWidth(&gamepad->ibmFont, reminderText);
        drawText(gamepad->disp, &gamepad->ibmFont, c555, reminderText, (gamepad->disp->w - tWidth) / 2, 10);

        // If it's plugged in, draw buttons
        if(gamepad->isPluggedIn)
        {
            // Helper function pointer
            void (*drawFunc)(display_t*, int, int, int, paletteColor_t);

            // A list of all the hat directions, in order
            static const uint8_t hatDirs[] =
            {
                GAMEPAD_HAT_UP,
                GAMEPAD_HAT_UP_RIGHT,
                GAMEPAD_HAT_RIGHT,
                GAMEPAD_HAT_DOWN_RIGHT,
                GAMEPAD_HAT_DOWN,
                GAMEPAD_HAT_DOWN_LEFT,
                GAMEPAD_HAT_LEFT,
                GAMEPAD_HAT_UP_LEFT
            };

            // For each hat direction
            for(uint8_t i = 0; i < ARRAY_SIZE(hatDirs); i++)
            {
                // The degree around the cluster
                int16_t deg = i * 45;
                // The center of the cluster
                int16_t xc = gamepad->disp->w / 4;
                int16_t yc = (gamepad->disp->h / 2) + Y_OFF;
                // Draw the button around the cluster
                xc += (( getSin1024(deg) * DPAD_CLUSTER_RADIUS) / 1024);
                yc += ((-getCos1024(deg) * DPAD_CLUSTER_RADIUS) / 1024);

                // Draw either a filled or outline circle, if this is the direction pressed
                drawFunc = (gamepad->gpState.hat == hatDirs[i]) ? &plotCircleFilled : &plotCircle;
                drawFunc(gamepad->disp, xc, yc, DPAD_BTN_RADIUS, c551 /*hsv2rgb(i * 32, 0xFF, 0xFF)*/);
            }

            // Start button
            drawFunc = (gamepad->gpState.buttons & GAMEPAD_BUTTON_START) ? &plotCircleFilled : &plotCircle;
            drawFunc(gamepad->disp,
                     (gamepad->disp->w / 2) - START_BTN_RADIUS - START_BTN_SEP,
                     (gamepad->disp->h / 4) + Y_OFF,
                     START_BTN_RADIUS, c333);

            // Select
            drawFunc = (gamepad->gpState.buttons & GAMEPAD_BUTTON_SELECT) ? &plotCircleFilled : &plotCircle;
            drawFunc(gamepad->disp,
                     (gamepad->disp->w / 2) + START_BTN_RADIUS + START_BTN_SEP,
                     (gamepad->disp->h / 4) + Y_OFF,
                     START_BTN_RADIUS, c333);

            // Button A
            drawFunc = (gamepad->gpState.buttons & GAMEPAD_BUTTON_A) ? &plotCircleFilled : &plotCircle;
            drawFunc(gamepad->disp,
                     ((3 * gamepad->disp->w) / 4) + AB_BTN_RADIUS + AB_BTN_SEP,
                     (gamepad->disp->h / 2) - AB_BTN_Y_OFF + Y_OFF,
                     AB_BTN_RADIUS, c243);

            // Button B
            drawFunc = (gamepad->gpState.buttons & GAMEPAD_BUTTON_B) ? &plotCircleFilled : &plotCircle;
            drawFunc(gamepad->disp,
                     ((3 * gamepad->disp->w) / 4) - AB_BTN_RADIUS - AB_BTN_SEP,
                     (gamepad->disp->h / 2) + AB_BTN_Y_OFF + Y_OFF,
                     AB_BTN_RADIUS, c401);

            // Set up drawing accel bars
            int16_t barY = (gamepad->disp->h * 3) / 4;

            // Plot X accel
            int16_t barWidth = ((gamepad->gpState.rx + 128) * MAX_ACCEL_BAR_W) / 256;
            fillDisplayArea(gamepad->disp, gamepad->disp->w - barWidth, barY, gamepad->disp->w, barY + ACCEL_BAR_H, c500);
            barY += (ACCEL_BAR_H + ACCEL_BAR_SEP);

            // Plot Y accel
            barWidth = ((gamepad->gpState.ry + 128) * MAX_ACCEL_BAR_W) / 256;
            fillDisplayArea(gamepad->disp, gamepad->disp->w - barWidth, barY, gamepad->disp->w, barY + ACCEL_BAR_H, c050);
            barY += (ACCEL_BAR_H + ACCEL_BAR_SEP);

            // Plot Z accel
            barWidth = ((gamepad->gpState.rz + 128) * MAX_ACCEL_BAR_W) / 256;
            fillDisplayArea(gamepad->disp, gamepad->disp->w - barWidth, barY, gamepad->disp->w, barY + ACCEL_BAR_H, c005);
            barY += (ACCEL_BAR_H + ACCEL_BAR_SEP);

            // If b is being held
            if(0 != gamepad->time_exit_pressed)
            {
                // Figure out for how long
                int64_t tHeldUs = esp_timer_get_time() - gamepad->time_exit_pressed;
                // If it has been held for more than the exit time
                if(tHeldUs > EXIT_TIME_US)
                {
                    // exit
                    switchToSwadgeMode(&modeMainMenu);
                }
                else
                {
                    // Draw 'progress' bar for exiting
                    int16_t numPx = (tHeldUs * gamepad->disp->w) / EXIT_TIME_US;
                    fillDisplayArea(gamepad->disp, 0, gamepad->disp->h - 10, numPx, gamepad->disp->h, c333);
                }
            }
        }
        else
        {
            // If it's not plugged in, give a hint
            const char plugInText[] = "Plug USB-C into computer please!";
            tWidth = textWidth(&gamepad->ibmFont, plugInText);
            drawText(gamepad->disp, &gamepad->ibmFont, c555, plugInText,
                     (gamepad->disp->w - tWidth) / 2,
                     (gamepad->disp->h - gamepad->ibmFont.h) / 2);
        }
    }
}

/**
 * Button callback. Send the button state over USB and save it for drawing
 *
 * @param evt The button event that occurred
 */
void gamepadButtonCb(buttonEvt_t* evt)
{
    gamepad->drawDisp = true;

    // Build a list of all independent buttons held down
    gamepad->gpState.buttons = 0;
    if(evt->state & BTN_A)
    {
        gamepad->gpState.buttons |= GAMEPAD_BUTTON_A;
    }
    if(evt->state & BTN_B)
    {
        gamepad->gpState.buttons |= GAMEPAD_BUTTON_B;
    }
    if(evt->state & START)
    {
        gamepad->gpState.buttons |= GAMEPAD_BUTTON_START;
    }
    if(evt->state & SELECT)
    {
        gamepad->gpState.buttons |= GAMEPAD_BUTTON_SELECT;
    }

    // Check if the buttons are held to exit
    if(gamepad->gpState.buttons == (GAMEPAD_BUTTON_A | GAMEPAD_BUTTON_B | GAMEPAD_BUTTON_START | GAMEPAD_BUTTON_SELECT))
    {
        // Combo pressed, note the time
        gamepad->time_exit_pressed = esp_timer_get_time();
    }
    else
    {
        // Combo released, clear the timer
        gamepad->time_exit_pressed = 0;
    }

    // Figure out which way the D-Pad is pointing
    gamepad->gpState.hat = GAMEPAD_HAT_CENTERED;
    if(evt->state & UP)
    {
        if(evt->state & RIGHT)
        {
            gamepad->gpState.hat |= GAMEPAD_HAT_UP_RIGHT;
        }
        else if(evt->state & LEFT)
        {
            gamepad->gpState.hat |= GAMEPAD_HAT_UP_LEFT;
        }
        else
        {
            gamepad->gpState.hat |= GAMEPAD_HAT_UP;
        }
    }
    else if(evt->state & DOWN)
    {
        if(evt->state & RIGHT)
        {
            gamepad->gpState.hat |= GAMEPAD_HAT_DOWN_RIGHT;
        }
        else if(evt->state & LEFT)
        {
            gamepad->gpState.hat |= GAMEPAD_HAT_DOWN_LEFT;
        }
        else
        {
            gamepad->gpState.hat |= GAMEPAD_HAT_DOWN;
        }
    }
    else if(evt->state & RIGHT)
    {
        gamepad->gpState.hat |= GAMEPAD_HAT_RIGHT;
    }
    else if(evt->state & LEFT)
    {
        gamepad->gpState.hat |= GAMEPAD_HAT_LEFT;
    }

    // Only send data if USB is ready
    if(tud_ready())
    {
        // Send the state over USB
        tud_gamepad_report(&gamepad->gpState);
    }
}

/**
 * @brief TODO
 *
 * @param evt
 */
void gamepadTouchCb(touch_event_t* evt)
{
    ESP_LOGE("GP", "%s (%d %d %d)", __func__, evt->pad_num, evt->pad_status, evt->pad_val);
}

/**
 * Acceleromoeter callback. Save the state and send it over USB
 *
 * @param accel The last read acceleration value
 */
void gamepadAccelCb(accel_t* accel)
{
    // Take 14 bits down to 8 bits, save it
    gamepad->gpState.rx = (accel->x) >> 6;
    gamepad->gpState.ry = (accel->y) >> 6;
    gamepad->gpState.rz = (accel->z) >> 6;

    // Redraw acceleration bars
    gamepad->drawDisp = true;

    // Only send data if USB is ready
    if(tud_ready())
    {
        // Send the state over USB
        tud_gamepad_report(&gamepad->gpState);
    }
}
