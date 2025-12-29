/**
 * @module muen
 */

/**
 * Represents sound. use {@link Sound.load} to load Sound from game folder.
 *
 * @example
 * ```js
 * var Sound = require("muen/Sound");
 *
 * var s = Sound.load("sample.ogg");
 * s.play();
 * ```
 */
declare class Sound {
    /**
     * @internal
     */
    constructor(id: number);

    /**
     * Is sound playing
     */
    get playing(): boolean;

    /**
     * Get sound volume
     */
    get volume(): number;

    /**
     * Set sound volume (from 0.0 to 1.0)
     */
    set volume(value: number);

    /**
     * Get sound pan
     */
    get pan(): number;

    /**
     * Set sound pan (0.5 is center, 0.0 is left, 1.0 is right)
     */
    set pan(value: number);

    /**
     * Get sound pitch
     */
    get pitch(): number;

    /**
     * Set sound pitch (base is 1.0);
     */
    set pitch(value: number);

    /**
     * Unload sound from memory
     */
    unload(): void;

    /**
     * Play sound
     */
    play(): void;

    /**
     * Stop sound playback
     */
    stop(): void;

    /**
     * Load sound from memory.
     *
     * Any subsequent calls with the same path will result in returning the same
     * already loaded sound.
     */
    static load(path: string): Sound;
}

/**
 * @module
 * @hidden
 */
declare module "muen/Sound" {
    export = Sound;
}
