var Ball = require("Ball");

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
    for (var i = 1; i < 500; i++) {
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

function randomByte() {
    return Math.floor(Math.random() * 256);
}
