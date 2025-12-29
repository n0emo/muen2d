/**
 * @module muen
 */

/**
 * Operations on game screen
 * @namespace
 */
declare interface screen {
    /** Time since last frame in seconds */
    get dt(): number;

    /** Time since window initialization */
    get time(): number;

    /** Width of the game screen */
    get width(): number;

    /** Height of the game screen */
    get height(): number;
}

/**
 * @module
 * @hidden
 */

declare module "muen/screen" {
    const screen: screen;
    export = screen;
}
