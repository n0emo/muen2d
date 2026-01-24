export class Font {
    constructor(path: string);

    constructor(options: { name: string });

    constructor(options: { path: string; name?: string; fontSize?: number; codepoints?: number[] });

    get valid(): boolean;
}

export default Font;
