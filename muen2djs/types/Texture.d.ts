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
 *
 * // Load texture from file
 * var t = Texture.load("sprite.png");
 *
 * // Draw texture on the screen
 * graphics.texture(t, 100, 100);
 * ```
 */
declare class Texture implements BasicTexture {
    /** OpenGL texture id */
    id: number;

    /** Texture base width */
    width: number;

    /** Texture base height */
    height: number;

    /** Mipmap levels, 1 by default */
    mipmaps: number;

    /** Data format (PixelFormat type) */
    format: number;

    /**
     * @param id      OpenGL texture id
     * @param width   Texture base width
     * @param height  Texture base height
     * @param mipmaps Mipmap levels, 1 by default
     * @param format  Data format (PixelFormat type)
     */
    constructor(id: number, width: number, height: number, mipmaps: number, format: number);

    /**
     * Check if a texture is valid (loaded in GPU)
     */
    get valid(): boolean;

    get source(): Rectangle;

    /**
     * Load texture from file into GPU memory (VRAM)
     * @param path Path to file
     */
    static load(path: string): Texture;

    /**
     * Unload texture from GPU memory (VRAM)
     */
    unload(): void;
}

/**
 * @module
 * @hidden
 */
declare module "muen/Texture" {
    export = Texture;
}
