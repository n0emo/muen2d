export type AlphanumericKey =
    | "apostrophe"
    | "comma"
    | "minus"
    | "period"
    | "slash"
    | "zero"
    | "one"
    | "two"
    | "three"
    | "four"
    | "five"
    | "six"
    | "seven"
    | "eight"
    | "nine"
    | "semicolon"
    | "equal"
    | "a"
    | "b"
    | "c"
    | "d"
    | "e"
    | "f"
    | "g"
    | "h"
    | "i"
    | "j"
    | "k"
    | "l"
    | "m"
    | "n"
    | "o"
    | "p"
    | "q"
    | "r"
    | "s"
    | "t"
    | "u"
    | "v"
    | "w"
    | "x"
    | "y"
    | "z"
    | "leftBracket"
    | "backslash"
    | "rightBracket"
    | "grave";

export type FunctionKey =
    | "space"
    | "escape"
    | "enter"
    | "tab"
    | "backspace"
    | "insert"
    | "delete"
    | "right"
    | "left"
    | "down"
    | "up"
    | "pageUp"
    | "pageDown"
    | "home"
    | "end"
    | "capsLock"
    | "scrollLock"
    | "numLock"
    | "printScreen"
    | "pause"
    | "f1"
    | "f2"
    | "f3"
    | "f4"
    | "f5"
    | "f6"
    | "f7"
    | "f8"
    | "f9"
    | "f10"
    | "f11"
    | "f12"
    | "leftShift"
    | "leftControl"
    | "leftAlt"
    | "leftSuper"
    | "rightShift"
    | "rightControl"
    | "rightAlt"
    | "rightSuper"
    | "kbMenu";

export type KeypadKey =
    | "kp0"
    | "kp1"
    | "kp2"
    | "kp3"
    | "kp4"
    | "kp5"
    | "kp6"
    | "kp7"
    | "kp8"
    | "kp9"
    | "kpDecimal"
    | "kpDivide"
    | "kpMultiply"
    | "kpSubtract"
    | "kpAdd"
    | "kpEnter"
    | "kpEqual";

export type AndroidKey = "back" | "menu" | "volumeUp" | "volumeDown";

/** Keyboard keys (US keyboard layout)
    NOTE: Use GetKeyPressed() to allow redefining
    required keys for alternative layouts */
export type KeyboardKey = AlphanumericKey | FunctionKey | KeypadKey | AndroidKey;

/** Input-related functions: keyboard */
export interface Keyboard {
    /** Check if a key has been pressed once */
    isKeyPressed(key: KeyboardKey): boolean;
    /** Check if a key has been pressed again */
    isKeyPressedRepeat(key: KeyboardKey): boolean;
    /** Check if a key is being pressed */
    isKeyDown(key: KeyboardKey): boolean;
    /** Check if a key has been released once */
    isKeyReleased(key: KeyboardKey): boolean;
    /** Check if a key is NOT being pressed */
    isKeyUp(key: KeyboardKey): boolean;
}

export declare const keyboard: Keyboard;
export default keyboard;
