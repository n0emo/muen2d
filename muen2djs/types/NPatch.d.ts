/**
 * @module muen
 */

/**
 * Texture, tex data stored in GPU memory (VRAM)
 *
 * @example
 * ```js
 * var Texture = require("muen/Texture");
 * var graphics = require("muen/graphics");
 * var NPatch = require("muen/NPatch");
 * var Rectangle = require("muen/Rectangle");
 * var Vector2 = require("muen/Vector2");
 * var Color = require("muen/Color");
 *
 * // Load texture from file
 * var t = Texture.load("sprite.png");
 * var n = new NPatch(t.source, 50, 50, 50, 50);
 * var d = new Rectangle(10,10,10,10);
 * var o = new Vector2(50,60);
 * var c = new Color(50,50,50);
 *
 * // Draw texture on the screen
 * graphics.textureNPatch(t, n, d, o, 1, c);
 * ```
 */
/**
 * NPatch, n-patch layout info
 */
declare class NPatch {
    /** Source top-left corner position x */
    x: number;

    /** Source top-left corner position y */
    y: number;

    /** Source width */
    width: number;

    /** Source height */
    height: number;

    /** Left border offset */
    left: number;

    /** Top border offset */
    top: number;

    /** Right border offset */
    right: number;

    /** Bottom border offset */
    bottom: number;

    /**
     * "default":    Npatch layout: 3x3 tiles
     * "vertical":   Npatch layout: 1x3 tiles
     * "horizontal": Npatch layout: 3x1 tiles
     */
    layout?: "default" | "vertical" | "horizontal";
    /**
     * @param {Rectangle} source Texture source rectangle
     * @param {number} left      Left border offset
     * @param {number} top       Top border offset
     * @param {number} right     Right border offset
     * @param {number} bottom    Bottom border offset
     * @param {"default" | "vertical" | "horizontal" | undefined} layout Layout of the n-patch: 3x3, 1x3 or 3x1
     */
    constructor(
        source: Rectangle,
        left: number,
        top: number,
        right: number,
        bottom: number,
        layout?: "default" | "vertical" | "horizontal"
    );
}

/**
 * @module
 * @hidden
 */
declare module "muen/NPatch" {
    export = NPatch;
}
