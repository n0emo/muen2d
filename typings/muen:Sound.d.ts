/**
 * Represents sound. use {@link Sound.constructor} to load Sound from game folder.
 *
 * @example
 * ```js
 * import Sound from "muen:Sound";
 *
 * let s = new Sound("sample.ogg");
 * s.play();
 * ```
 */
export class Sound {
    /**
     * @param path Path to sound file
     */
    constructor(path: string);

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
}

export default Sound;
