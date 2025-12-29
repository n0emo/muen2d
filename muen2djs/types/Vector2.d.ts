/**
 * @module muen
 */

/**
 * Represents 2-dimensional vector {x,y}
 */
declare class Vector2 {
    /** Vector x component */
    x: number;

    /** Vector y component */
    y: number;

    /**
     * @param x Vector x component
     * @param y Vector y component
     */
    constructor(x: number, y: number);

    /**
     * Clones vector creating new instance
     */
    clone(): Vector2;

    /**
     * Creates zero initialized Vector2
     */
    static zero(): Vector2;
}

/**
 * @module
 * @hidden
 */
declare module "muen/Vector2" {
    export = Vector2;
}
