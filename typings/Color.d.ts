export interface BasicColor {
    r: number;
    g: number;
    b: number;
    a: number;
}

/**
 * Represents color as 32-bit RGBA object
 *
 * @example
 * ```js
 * import Color from "muen:Color"
 * graphics from "muen:graphics"
 *
 * let green = new Color(50, 200, 50);
 * graphics.clear(green);
 * ```
 */
export class Color implements BasicColor {
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
     * let color = Color.fromHex("#123456");
     *
     * // Equivalent to new Color(255, 170, 68, 128);
     * let color = Color.fromHex("#ffaa4480");
     * ```
     *
     * @param code hexadecimal string
     */
    static fromHex(code: string): Color;
}

export default Color;
