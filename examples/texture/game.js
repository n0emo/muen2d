import graphics from "glint:graphics";
import Color from "glint:Color";
import Texture from "glint:Texture";
import Vector2 from "glint:Vector2";

export const config = {
    window: {
        title: "Texture",
    },
};

const bgColor = Color.fromHex("#181818");
let t;

export function load() {
    const t1 = new Texture("cat.jpg");
    const t2 = new Texture({ path: "cat.jpg", name: "cat.jpg" });
    const t3 = new Texture({ name: "cat.jpg" });
    console.log(" First:", t1);
    console.log("Second:", t2);
    console.log(" Third:", t3);
    t = t1;
}

export function draw() {
    graphics.clear(bgColor);
    graphics.textureEx(t, new Vector2(0, 0), 0, 0.65, new Color(255, 255, 255));
}
