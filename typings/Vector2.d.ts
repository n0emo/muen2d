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
    
    /** vector length */
    length: number;
    
    /** vector square length */
    lengthSqr: number;
    
    /** vector length */
    min: Vector2;
    
    /** vector length */
    max: Vector2;
    
    /** Add two vectors (vec1 + vec2) */
    add(vec1: Vector2, vec2: Vector2): Vector2;
    
    /** Add vector and float value */
    addNum(vec: Vector2, val: number): Vector2;
    
    /** Subtract two vectors (vec1 - vec2) */
    sub(vec1: Vector2, vec2: Vector2): Vector2;
    
    /** Subtract vector by float value */
    subNum(vec: Vector2, val: number): Vector2;

    /** Calculate two vectors dot product */
    dot(vec1: Vector2, vec2: Vector2): number;

    /** Calculate distance between two vectors */
    distance(vec1: Vector2, vec2: Vector2): number;

    /** Calculate square distance between two vectors */
    distanceSqr(vec1: Vector2, vec2: Vector2): number;

    /**
     * Calculate the signed angle from vec1 to vec2, relative to the origin (0, 0)
     * NOTE: Coordinate system convention: positive X right, positive Y down
     * positive angles appear clockwise, and negative angles appear
     * counterclockwise
     */
    angle(vec1: Vector2, vec2: Vector2): number;

    /**
     * Calculate angle defined by a two vectors line
     * NOTE: Parameters need to be normalized
     * Current implementation should be aligned with glm::angle
     */
    lineAngle(start: Vector2, end: Vector2): number;

    /** Scale vector (multiply by value) */
    scale(vec: Vector2, scale: number): Vector2;

    /** Multiply vector by vector */
    mul(vec1: Vector2, vec2: Vector2): Vector2;
    
    /** Negate vector */
    negate(vec: Vector2): Vector2;
    
    /** Divide vector by vector */
    div(vec1: Vector2, vec2: Vector2): Vector2;
    
    /** Normalize provided vector */
    norm(vec: Vector2): Vector2;
    
    /** Transforms a Vector2 by a given Matrix */
    transform(vec: Vector2, mat: Matrix): Vector2;
    
    /** Calculate linear interpolation between two vectors */
    lerp(vec1: Vector2, vec2: Vector2, amount: number): Vector2;
    
    /** Calculate reflected vector to normal */
    reflect(vec: Vector2, normal: Vector2): Vector2;
    
    /** Rotate vector by angle */
    rotate(vec: Vector2, angle: number): Vector2;
    
    /** Move Vector towards target */
    moveTowards(vec: Vector2, target: Vector2, maxDistance: number): Vector2;
    
    /** Invert the given vector */
    invert(vec: Vector2): Vector2;
    
    /** 
     * Clamp the components of the vector between
     * min and max values specified by the given vectors
     */
    clamp(vec: Vector2, min: Vector2, max: Vector2): Vector2;
    
    /** Clamp the magnitude of the vector between two min and max values */
    clampValue(vec: Vector2, min: number, max: number): Vector2;
    
    /** Check whether two given vectors are almost equal */
    equals(p: Vector2, q: Vector2): number;
    
    /** 
     * Compute the direction of a refracted ray
     * v: normalized direction of the incoming ray
     * n: normalized normal vector of the interface of two optical media
     * r: ratio of the refractive index of the medium from where the ray comes
     * to the refractive index of the medium on the other side of the surface
     */
    refract(vec: Vector2, n: Vector2, r: number): Vector2;

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
