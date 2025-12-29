/**
 * @module muen
 */

/**
 * Represents color as 32-bit RGBA object
 *
 * @example
 * ```js
 * var Color = require("muen/Color");
 * var graphics = require("muen/graphics");
 *
 * var green = new Color(50, 200, 50);
 * graphics.clear(green);
 * ```
 */
declare class Color {
    /** Color red value */
    r: number;

    /** Color green value */
    g: number;

    /** Color green blue */
    b: number;

    /** Color alpha blue */
    a: number;

    /**
     * @param r Color red value
     * @param g Color green value
     * @param b Color blue value
     * @param a Color alpha value (default: 255)
     */
    constructor(r: number, g: number, b: number, a?: number);

    /**
     * Creates new instance of Color from hexadecimal string formatted as
     * `"#rrggbb"` or `"#rrggbbaa"`.
     *
     * @example
     * ```js
     * // Equivalent to new Color(18, 52, 86);
     * var color = Color.fromHex("#123456");
     *
     * // Equivalent to new Color(255, 170, 68, 128);
     * var color = Color.fromHex("#ffaa4480");
     * ```
     *
     * @param code hexadecimal string
     */
    static fromHex(code: string): Color;
}

/**
 * @module
 * @hidden
 */
declare module "muen/Color" {
    export = Color;
}
