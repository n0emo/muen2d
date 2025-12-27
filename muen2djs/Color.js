/**
 * @class
 * Represents color as 32-bit RGBA object
 *
 * @example
 * var Color = require("muen/Color");
 * var graphics = require("muen/graphics");
 *
 * var green = new Color("#22a822");
 * graphics.clear(green);
 *
 * @constructor
 * @param {number} r Color red value
 * @param {number} g Color green value
 * @param {number} b Color blue value
 * @param {number} [a=255] Color alpha value
 */
function Color(r, g, b, a) {
    /**
     * Color red value
     * @type {number}
     */
    this.r = assertByte(r);

    /**
     * Color green value
     * @type {number}
     */
    this.g = assertByte(g);

    /**
     * Color green blue
     * @type {number}
     */
    this.b = assertByte(b);

    /**
     * Color alpha blue
     * @type {number}
     */
    this.a = a ? assertByte(a) : 255;
}

/**
 * Creates new instance of Color from hexadecimal string formatted as
 * `"#rrggbb"` or `"#rrggbbaa"`.
 *
 * @example
 * // Equivalent to new Color(18, 52, 86);
 * var color = Color.fromHex("#123456");
 *
 * // Equivalent to new Color(255, 170, 68, 128);
 * var color = Color.fromHex("#ffaa4480");
 *
 * @param {string} code hexadecimal string
 * @returns {Color}
 */
Color.fromHex = function (code) {
    if (typeof code !== "string" || (code.length !== 7 && code.length !== 9) || code[0] !== "#") {
        throw TypeError("Invalid arguments for Color");
    }

    var r = parseByte(code, 1);
    var g = parseByte(code, 3);
    var b = parseByte(code, 5);
    var a = code.length === 9 ? parseByte(code, 7) : 255;

    return new Color(r, g, b, a);
};

/**
 * @private
 * @param {string} s
 * @param {number} i
 * @return {number}
 */
function parseByte(s, i) {
    return parseInt(s.slice(i, i + 2), 16);
}

/**
 * @private
 * @param {number} n
 * @return {number}
 */
function assertByte(n) {
    if ((n | 0) !== n) {
        throw TypeError("Color byte must be integer");
    }

    if (n < 0 || n >= 256) {
        throw TypeError("Color byte must be in range [0; 255]");
    }

    return n;
}

module.exports = Color;
