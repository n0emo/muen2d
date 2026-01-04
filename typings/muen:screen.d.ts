/**
 * @inline
 */
export interface Screen {
    /** Time since last frame in seconds */
    get dt(): number;

    /** Time since window initialization */
    get time(): number;

    /** Width of the game screen */
    get width(): number;

    /** Height of the game screen */
    get height(): number;
}

export declare const screen: Screen;
export default screen;
