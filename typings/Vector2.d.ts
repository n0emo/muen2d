export interface BasicVector2 {
    x: number;
    y: number;
}

/**
 * Represents 2-dimensional vector {x,y}
 */
export class Vector2 implements BasicVector2 {
    /** Vector x component */
    x: number;

    /** Vector y component */
    y: number;

    /**
     * @param x Vector x component
     * @param y Vector y component
     */
    constructor(x: number, y: number);

    /**
     * Clones vector creating new instance
     */
    clone(): Vector2;

    /**
     * Creates zero initialized Vector2
     */
    static zero(): Vector2;
}

export default Vector2;
