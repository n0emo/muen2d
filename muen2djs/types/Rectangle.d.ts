/**
 * @module muen
 */

/**
 * Rectangle, 4 components
 */
declare class Rectangle {
    /** Rectangle top-left corner position x */
    x: number;

    /** Rectangle top-left corner position y */
    y: number;

    /** Rectangle width */
    width: number;

    /** Rectangle height */
    height: number;

    /**
     * @param x Rectangle top-left corner position x
     * @param y Rectangle top-left corner position y
     * @param width Rectangle width
     * @param height Rectangle height
     */
    constructor(x: number, y: number, width: number, height: number);

    /**
     * Creates zero initialized rectangle
     */
    static zero(): Rectangle;

    /**
     * Creates vector with top-left corner set to `position` and size set to `size`
     * @param position Rectangle top-left corner
     * @param size Rectangle size
     */
    static fromVectors(size: Vector2, position: Vector2): Rectangle;
}

/**
 * @module
 * @hidden
 */
declare module "muen/Rectangle" {
    export = Rectangle;
}
