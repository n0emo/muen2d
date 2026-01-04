import Camera from "muen:Camera";
import graphics from "muen:graphics";
import Color from "muen:Color";
import screen from "muen:screen";
import Vector2 from "muen:Vector2";

const camera = Camera.default();
const bg = Color.fromHex("#181818");
const fg = Color.fromHex("#e17cf0ff");

export function update() {
    camera.rotation += screen.dt * 150;

    // TODO: does not work: camera.offset.x = screen.width * 0.5;
    // Maybe we should return proxy object which responds to modifications
    // of offset and target by modifying internal state of Camera
    camera.offset = new Vector2(screen.width * 0.5, screen.height * 0.5);
    camera.target = new Vector2(screen.width * 0.5, screen.height * 0.5);
    camera.zoom = Math.sin(screen.time) * 0.5 + 1.2;
}

export function draw() {
    graphics.clear(bg);
    graphics.beginCameraMode(camera);
    graphics.rectangle(screen.width * 0.3, screen.height * 0.3, 50, 100, fg);
    graphics.endCameraMode();
}
