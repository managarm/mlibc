/*
 * oboskrnl/vfs/keycode.h
 *
 * Copyright (c) 2025 Omar Berrow
 *
 * Copied from OBOS.
*/

#pragma once

#include <stdint.h>

#define BIT(n) (1U << (n))

enum modifiers {
    SHIFT = BIT(0),
    CTRL = BIT(1),
    ALT = BIT(2),
    SUPER_KEY = BIT(3),
    FN = BIT(4),
    CAPS_LOCK = BIT(5),
    NUM_LOCK = BIT(6),
    KEY_RELEASED = BIT(7),
    NUMPAD = BIT(8),
};

#undef BIT

/* NOTE: Only 7-bits! */
typedef enum scancode {
    SCANCODE_UNKNOWN = 0,
    SCANCODE_A,
    SCANCODE_B,
    SCANCODE_C,
    SCANCODE_D,
    SCANCODE_E,
    SCANCODE_F,
    SCANCODE_G,
    SCANCODE_H,
    SCANCODE_I,
    SCANCODE_J,
    SCANCODE_K,
    SCANCODE_L,
    SCANCODE_M,
    SCANCODE_N,
    SCANCODE_O,
    SCANCODE_P,
    SCANCODE_Q,
    SCANCODE_R,
    SCANCODE_S,
    SCANCODE_T,
    SCANCODE_U,
    SCANCODE_V,
    SCANCODE_W,
    SCANCODE_X,
    SCANCODE_Y,
    SCANCODE_Z,
    SCANCODE_0,
    SCANCODE_1,
    SCANCODE_2,
    SCANCODE_3,
    SCANCODE_4,
    SCANCODE_5,
    SCANCODE_6,
    SCANCODE_7,
    SCANCODE_8,
    SCANCODE_9,
    SCANCODE_PLUS,
    SCANCODE_FORWARD_SLASH,
    SCANCODE_BACKSLASH,
    SCANCODE_STAR,
    SCANCODE_EQUAL,
    SCANCODE_DASH,
    SCANCODE_UNDERSCORE,
    SCANCODE_BACKTICK,
    SCANCODE_QUOTATION_MARK,
    SCANCODE_APOSTROPHE,
    SCANCODE_SQUARE_BRACKET_LEFT,
    SCANCODE_SQUARE_BRACKET_RIGHT,
    SCANCODE_TAB,
    SCANCODE_ESC,
    SCANCODE_PGUP,
    SCANCODE_PGDOWN,
    SCANCODE_HOME,
    SCANCODE_END,
    SCANCODE_DELETE,
    SCANCODE_BACKSPACE,
    SCANCODE_SPACE,
    SCANCODE_INSERT,
    SCANCODE_F1,
    SCANCODE_F2,
    SCANCODE_F3,
    SCANCODE_F4,
    SCANCODE_F5,
    SCANCODE_F6,
    SCANCODE_F7,
    SCANCODE_F8,
    SCANCODE_F9,
    SCANCODE_F10,
    SCANCODE_F11,
    SCANCODE_F12,
    SCANCODE_DOT,
    SCANCODE_COMMA,
    SCANCODE_SEMICOLON,
    SCANCODE_UP_ARROW,
    SCANCODE_DOWN_ARROW,
    SCANCODE_RIGHT_ARROW,
    SCANCODE_LEFT_ARROW,
    SCANCODE_ENTER,
    /*
     * NOTE(oberrow): These are added even though there are modifiers for these keys
     * This is to allow for programs to use the following keys "as is"
    */
    SCANCODE_SUPER_KEY,
    SCANCODE_CTRL,
    SCANCODE_ALT,
    SCANCODE_FN,
    SCANCODE_SHIFT,
    /*
     * TODO(oberrow): Add power management keys? (some keyboards have stuff like that, 
     * and it is included in keyboard specs as well)
    */
} scancode;

/*
 * Format:
 *  0-6: Scancode (see enum scancode)
 * 7-15: Modifier bits (see enum modifiers)
 * Reading from any keyboard device will yield a "keycode" as
 * defined above.
*/
typedef uint16_t keycode;
#define KEYCODE(scancode, modifiers) (uint16_t)((uint16_t)((scancode) & 0x7f) | ((uint16_t)((modifiers) & 0x1ff) << 7))
#define MODIFIERS_FROM_KEYCODE(code) (enum modifiers)(((uint16_t)(code) >> 7) & 0x1ff)
#define SCANCODE_FROM_KEYCODE(code) (enum scancode)((uint16_t)(code) & 0x7f)
#define KEYCODE_ADD_MODIFIER(code, modifier) ((code) |= ((modifier) << 7))
#define KEYCODE_CLEAR_MODIFIER(code, modifier) ((code) &= ~((modifier) << 7))
