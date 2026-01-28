import Color from "glint:Color";
import graphics from "glint:graphics";
import screen from "glint:screen";

import { Ball } from "./Ball.js";

class Game {
    constructor() {
        console.log("Creating Balls game!");

        this.bgColor = Color.fromHex("#181818");
        this.balls = [];

        console.log("Game created!");
    }

    load() {
        const count = 500;
        console.log("Adding", count, "balls");
        for (let i = 1; i < count; i++) {
            const x = Math.random() * (screen.width - 30) + 16;
            const y = Math.random() * (screen.height - 30) + 16;
            const angle = Math.random() * 360;
            const color = new Color(randomByte(), randomByte(), randomByte());
            this.balls.push(new Ball(x, y, angle, color));
        }
    }

    update() {
        for (let i = 0; i < this.balls.length; i++) {
            this.balls[i].update();
        }
    }

    draw() {
        graphics.clear(this.bgColor);
        for (let i = 0; i < this.balls.length; i++) {
            this.balls[i].draw();
        }
    }
}

const randomByte = () => Math.floor(Math.random() * 256);

const game = new Game();

export const config = {
    window: {
        title: "Balls",
    },
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
