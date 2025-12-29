/**
 * @module muen
 */

/**
 * Defines position/orientation in 2D space
 */
declare class Camera {
    /** Camera offset (displacement from target) */
    offset: Vector2;

    /** Camera target (rotation and zoom origin) */
    target: Vector2;

    /** Camera rotation in degrees */
    rotation: number;

    /** Camera zoom (scaling) */
    zoom: number;

    /**
     * @param offset Camera offset (displacement from target)
     * @param target Camera target (rotation and zoom origin)
     * @param rotation Camera rotation in degrees
     * @param zoom Camera zoom (scaling)
     */
    constructor(offset: Vector2, target: Vector2, rotation: number, zoom: number);

    /**
     * Constructs camera with default parameters
     */
    static default(): Camera;
}

/**
 * @module
 * @hidden
 */
declare module "muen/Camera" {
    export = Camera;
}
