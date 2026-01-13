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

    /** vector length */
    get length(): number;

    /** vector length square */
    get lengthSqr(): number;

    /** Add two vectors (this + other) */
    add(otherX: number, otherY: number): Vector2;
    add(other: Vector2): Vector2;

    /** Add vector and float value */
    addNum(val: number): Vector2;

    /** Subtract two vectors (this - other) */
    sub(otherX: number, otherY: number): Vector2;
    sub(other: Vector2): Vector2;

    /** Subtract vector by float value */
    subNum(val: number): Vector2;

    /** Calculate two vectors dot product */
    dot(otherX: number, otherY: number): number;
    dot(other: Vector2): number;

    /** Calculate distance between two vectors */
    distance(otherX: number, otherY: number): number;
    distance(other: Vector2): number;

    /** Calculate square distance between two vectors */
    distanceSqr(otherX: number, otherY: number): number;
    distanceSqr(other: Vector2): number;

    /**
     * Calculate the signed angle from this to other, relative to the origin (0, 0)
     * NOTE: Coordinate system convention: positive X right, positive Y down
     * positive angles appear clockwise, and negative angles appear
     * counterclockwise
     */
    angle(otherX: number, otherY: number): number;
    angle(other: Vector2): number;

    /**
     * Calculate angle defined by a two vectors line
     * NOTE: Parameters need to be normalized
     * Current implementation should be aligned with glm::angle
     */
    lineAngle(endX: number, endY: number): number;
    lineAngle(end: Vector2): number;

    /** Scale vector (multiply by value) */
    scale(scale: number): Vector2;

    /** Multiply vector by vector */
    mul(otherX: number, otherY: number): Vector2;
    mul(other: Vector2): Vector2;

    /** Negate vector */
    negate(): Vector2;

    /** Divide vector by vector */
    div(otherX: number, otherY: number): Vector2;
    div(other: Vector2): Vector2;

    /** Normalize provided vector */
    norm(): Vector2;

    /** Transforms a this Vector2 by a given Matrix */
    // TODO: transform(mat: Matrix): Vector2;

    /** Calculate linear interpolation between two vectors */
    lerp(otherX: number, otherY: number, amount: number): Vector2;
    lerp(other: Vector2, amount: number): Vector2;

    /** Calculate reflected vector to normal */
    reflect(normalX: number, normalY: number): Vector2;
    reflect(normal: Vector2): Vector2;

    /** Get min value for each pair of components */
    min(otherX: number, otherY: number): Vector2;
    min(other: Vector2): Vector2;

    /** Get max value for each pair of components */
    max(otherX: number, otherY: number): Vector2;
    max(other: Vector2): Vector2;

    /** Rotate vector by angle */
    rotate(angle: number): Vector2;

    /** Move Vector towards target */
    moveTowards(targetX: number, targetY: number, maxDistance: number): Vector2;
    moveTowards(target: Vector2, maxDistance: number): Vector2;

    /** Invert the given vector */
    invert(): Vector2;

    /**
     * Clamp the components of the vector between
     * min and max values specified by the given vectors
     */
    clamp(minX: number, minY: number, maxX: number, maxY: number): Vector2;
    clamp(min: Vector2, max: Vector2): Vector2;

    /** Clamp the magnitude of the vector between two min and max values */
    clampValue(min: number, max: number): Vector2;

    /** Check whether two given vectors are almost equal */
    equals(otherX: number, otherY: number): number;
    equals(other: Vector2): number;

    /**
     * Compute the direction of a refracted ray
     * v: normalized direction of the incoming ray
     * n: normalized normal vector of the interface of two optical media
     * r: ratio of the refractive index of the medium from where the ray comes
     * to the refractive index of the medium on the other side of the surface
     */
    refract(nX: number, nY: number, r: number): Vector2;
    refract(n: Vector2, r: number): Vector2;

    /**
     * Clones vector creating new instance
     */
    clone(): Vector2;

    /**
     * Creates zero initialized Vector2
     */
    static zero(): Vector2;

    /** returns Vector2{1, 1} */
    static one(): Vector2;
}

export default Vector2;
