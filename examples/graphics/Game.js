function Game() {

    this.config = {
        title: "Graphics",
    };

    this.bgColor = new Color("#181818");

    this.update = function () {
    }

    this.draw = function () {
        graphics.clear(this.bgColor);
        graphics.rectangle(50, 100, 30, 30, new Color("#c49bffff"));
        graphics.rectangle(new Vector2(100, 100), new Vector2(30, 30), new Color("#bf37bfff"));
        graphics.rectangle(new Rectangle(150, 100, 30, 30), new Color("#ff9ba5ff"));
        graphics.rectangle(new Rectangle(210, 100, 30, 30), new Vector2(10, 10), 45, new Color("#ffc787ff"));
        
        // Error running game: TypeError: Invalid arguments count for graphics.rectangle
        // graphics.rectangle(50, 100, 30, 30, 40, new Color("#c49bffff"));
    }
}

