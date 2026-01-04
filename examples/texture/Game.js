import graphics from "muen:graphics";
import Color from "muen:Color";
import Texture from "muen:Texture";
import Vector2 from "muen:Vector2";

export const config = { title: "Graphics" };

const bgColor = Color.fromHex("#181818");
let t = undefined;

export function load() {
    t = new Texture("cat.jpg");
    console.log(t);
}

export function draw() {
    graphics.clear(bgColor);
    graphics.textureEx(t, new Vector2(0, 0), 0, 0.65, new Color(255, 255, 255));
}
