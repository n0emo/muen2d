import Rectangle from "glint:Rectangle";

/**
 * NPatch, n-patch layout info
 *
 * @example
 * ```js
 * import Texture from "glint:Texture";
 * import graphics from "glint:graphics";
 * import NPatch from "glint:NPatch";
 * import Rectangle from "glint:Rectangle";
 * import Vector2 from "glint:Vector2";
 * import Color from "glint:Color";
 *
 * let texture = Texture.load("sprite.png");
 * let npatch = new NPatch(t.source, 50, 50, 50, 50);
 * let dest = new Rectangle(0, 0, 200, 200);
 * let origin = new Vector2(50,60);
 * let color = new Color(50,50,50);
 *
 * // Draw texture on the screen
 * graphics.textureNPatch(texture, npatch, dest, origin, 0.0, color);
 * ```
 */
export class NPatch {
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
        layout?: "default" | "vertical" | "horizontal",
    );
}

export default NPatch;
