import { Console } from "muen:console";
import * as muenMod from "muen";

declare global {
    const console: Console;
    const muen: typeof muenMod;
}
