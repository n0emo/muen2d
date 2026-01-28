import Rectangle from "glint:Rectangle";

/**
 * Texture, tex data stored in GPU memory (VRAM)
 *
 * @example
 * ```js
 * import Texture from "glint:Texture";
 * import graphics from "glint:graphics";
 *
 * // Load texture from file
 * let t = new Texture("sprite.png");
 *
 * // Draw texture on the screen
 * graphics.texture(t, 100, 100);
 * ```
 */
export class Texture {
    /**
     * Load texture from file into GPU memory (VRAM)
     * @param path Path to image file to load
     */
    constructor(path: string);

    /**
     * Load texture from file into GPU memory (VRAM) and give it a cache name
     * @param options Texture loading options
     */
    constructor(options: { name: string });

    constructor(options: { path: string; name?: string });

    /**
     * Check if a texture is valid (loaded in GPU)
     */
    get valid(): boolean;

    get source(): Rectangle;

    /**
     * Unload texture from GPU memory (VRAM)
     */
    unload(): void;
}

export default Texture;
