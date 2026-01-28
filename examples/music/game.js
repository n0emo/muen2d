import Music from "glint:Music";
import screen from "glint:screen";

let music;

export const config = {
    window: {
        title: "Music",
    },
};

export function load() {
    music = new Music("GloriousMorning.mp3");
    music.looping = true;
    music.volume = 0.5;
    music.pitch = 1.2;
    music.play();
}

export function update() {
    music.pan = Math.cos(3.0 * screen.time) * 0.5 + 0.5;
}
