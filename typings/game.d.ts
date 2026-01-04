/**
 * @inline
 */
export interface Config {
    /**
     * Window title
     */
    title?: string;

    /**
     * Window width
     */
    width?: number;

    /**
     * Window height
     */
    height?: number;

    /**
     * Target fps. Engine will try to call your `update` and `draw` callbacks
     * exactly `fps` times per second
     */
    fps: number;
}

/**
 * Configures how the engine executes the game
 * @optional
 */
export declare const config: Config | undefined;

/**
 * Called once when engine initializes game
 *
 * @event
 * @optional
 */
export function load(): void;

/**
 * Called every frame when engine updates game
 *
 * @event
 * @optional
 */
export function update(): void;

/**
 * Called every frame when engine draws game
 *
 * @event
 * @optional
 */
export function draw(): void;
