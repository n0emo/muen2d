function Game() {
    console.log("Creating new game");

    this.config = {
        fps: 165,
        width: 800,
        height: 600,
        title: "Balls",
    };

    this.bgColor = new Color("#181818");
    this.balls = [];
    for (var i = 1; i < 100; i++) {
        var x = Math.random() * (this.config.width - 30) + 15;
        var y = Math.random() * (this.config.height - 30) + 15;
        var angle = Math.random() * 360;
        var color = new Color(randomByte(), randomByte(), randomByte())
        this.balls.push(new Ball(x, y, angle, color));
    }


    this.update = function() {
        for (var i = 0; i < this.balls.length; i++) {
            this.balls[i].update();
        }
    }

    this.draw = function() {
        graphics.clear(this.bgColor);
        for (var i = 0; i < this.balls.length; i++) {
            this.balls[i].draw();
        }
    }
}

function Ball(x, y, angle, color) {
    this.x = x;
    this.y = y;
    this.xVel = 400 * Math.cos(angle / 360 * Math.PI);
    this.yVel = 400 * Math.sin(angle / 360 * Math.PI);
    this.radius = 10;
    this.color = color;

    this.update = function() {
        this.x += this.xVel * screen.dt;
        this.y += this.yVel * screen.dt;

        if (this.x < this.radius || this.x >= screen.width - this.radius) {
            this.xVel *= -1;
        }
        if (this.y < this.radius || this.y >= screen.height - this.radius) {
            this.yVel *= -1;
        }
    }

    this.draw = function() {
        graphics.circle(this.x, this.y, this.radius, this.color);
    }
}

function randomByte() {
    return Math.floor(Math.random() * 256);
}
