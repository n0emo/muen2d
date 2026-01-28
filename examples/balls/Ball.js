import screen from "glint:screen";
import graphics from "glint:graphics";
import Vector2 from "glint:Vector2";

export class Ball {
    constructor(x, y, angle, color) {
        this.pos = new Vector2(x, y);
        this.vel = new Vector2(1, 0).scale(400).rotate(angle);
        this.radius = 10;
        this.color = color;
    }

    update() {
        const pos = this.pos.clone().add(this.vel.x * screen.dt, this.vel.y * screen.dt);
        let reflect = false;

        if (pos.x < this.radius || pos.x >= screen.width - this.radius) {
            reflect = true;
            this.vel.x *= -1;
        }
        if (pos.y < this.radius || pos.y >= screen.height - this.radius) {
            reflect = true;
            this.vel.y *= -1;
        }

        if (!reflect) {
            this.pos = pos;
        }
    }

    draw() {
        graphics.circle(this.pos.x, this.pos.y, this.radius, this.color);
    }
}
