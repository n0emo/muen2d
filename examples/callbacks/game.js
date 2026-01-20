/**
 * The engine uses variuos callbacks to communicate with your game. Note that
 */

/** Controls how the engine executes your game */
export const config = {
    /** Window title */
    title: "Hello QuickJS",

    /** Window width */
    width: 800,

    /** Window height */
    height: 600,

    /**
     * Fixed FPS for the game. `draw` and `update` function will be called `fps`
     * times per second.
     */
    fps: 60,
};

/**
 * Engine will call this function once when starting your game. Any
 * initialization logic that depends on engine state should go there (loading
 * texture or sounds, using screen, etc.)
 */
export function load() {
    console.log("My game is loading");
}

/**
 * Engine will call this every frame. Use to update your game logic
 */
export function update() {
    console.log("My game is updating");
}

/**
 * Engine will call this every frame. Use to render your game on the screen
 */
export function draw() {
    console.log("My game is drawing");
}

/**
 * Engine will call this function if you press F5.
 *
 * After that, engine will try to load your game again. If it's successful,
 * engine will call postReload on new game. If it's successful too, then old
 * game will be replaced with the new game.
 *
 * You can return arbitary value from this function and it will be passed to
 * postReload function if present.
 *
 * @returns {string}
 */
export function preReload() {
    console.log("My game is preReloading");
    return "example data";
}

/**
 * Data returned from preReload will be passed here. You could reinitialize
 * your game using it.
 *
 * @param {string} data
 */
export function postReload(data) {
    console.log(`My game is postReloading with '${data}'`);
}
