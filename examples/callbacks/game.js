// The engine uses variuos callbacks to communicate with your game. Note that
// all of them are optional and may be ommited.

// Controls how the engine executes your game
export const config = {
    // Window title
    title: "Hello QuickJS",

    // Window width
    width: 800,

    // Window height
    height: 600,

    // Fixed FPS for the game. `draw` and `update` function will be called `fps`
    // times per second.
    fps: 60,
};

// Engine will call this function once when starting your game. Any
// initialization logic that depends on engine state should go there (loading
// texture or sounds, using screen, etc.)
export function load() {
    console.log("loading");
}

// Engine will call this every frame. Use to update your game logic
export function update() {
    console.log("update");
}

// Engine will call this every frame. Use to render your game on the screen
export function draw() {
    console.log("draw");
}
