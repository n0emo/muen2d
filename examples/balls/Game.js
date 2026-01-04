import Color from "muen:Color";
import graphics from "muen:graphics";
import screen from "muen:screen";

import { Ball } from "./Ball.js";

class Game {
    constructor() {
        console.log("Creating Balls game!");

        this.bgColor = Color.fromHex("#181818");
        this.balls = [];

        console.log("Game created!");
    }

    load() {
        for (var i = 1; i < 500; i++) {
            var x = Math.random() * (screen.width - 30) + 16;
            var y = Math.random() * (screen.height - 30) + 16;
            var angle = Math.random() * 360;
            var color = new Color(randomByte(), randomByte(), randomByte());
            this.balls.push(new Ball(x, y, angle, color));
        }
    }

    update() {
        for (var i = 0; i < this.balls.length; i++) {
            this.balls[i].update();
        }
    }

    draw() {
        graphics.clear(this.bgColor);
        for (var i = 0; i < this.balls.length; i++) {
            this.balls[i].draw();
        }
    }
}

const randomByte = () => Math.floor(Math.random() * 256);

let game = new Game();

export const config = {
    fps: 165,
    width: 800,
    height: 600,
    title: "Balls",
};

export function load() {
    game.load();
}

export function update() {
    game.update();
}

export function draw() {
    game.draw();
}
