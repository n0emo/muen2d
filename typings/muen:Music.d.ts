/**
 * Represents music stream
 */
export class Music {
    /**
     * @param path Path to music file
     */
    constructor(path: string);

    /**
     * Is music playing
     */
    get playing(): boolean;

    /**
     * Is music looping
     */
    get looping(): boolean;

    /**
     * Set music to loop or not
     */
    set looping(value: boolean);

    /**
     * Get music volume
     */
    get volume(): number;

    /**
     * Set music volume (from 0.0 to 1.0)
     */
    set volume(value: number);

    /**
     * Get music pan
     */
    get pan(): number;

    /**
     * Set music pan (0.5 is center, 0.0 is left, 1.0 is right)
     */
    set pan(value: number);

    /**
     * Get music pitch
     */
    get pitch(): number;

    /**
     * Set music pitch (base is 1.0)
     */
    set pitch(value: number);

    /**
     * Unload music from memory
     */
    unload(): void;

    /**
     * Start music playback
     */
    play(): void;

    /**
     * Stop music playback
     */
    stop(): void;

    /**
     * Pause music
     */
    pause(): void;

    /**
     * Resume music
     */
    resume(): void;

    /**
     * Seek music to cursor (in seconds)
     */
    seek(cursor: number): void;
}

export default Music;
