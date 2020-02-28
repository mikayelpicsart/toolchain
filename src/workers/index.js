
import { removeBackground } from './removeBackground';

self.onmessage = async function (e) {
    const { cmd, ...data } = JSON.parse(e.data);
    switch (cmd) {
        case "REMOVE_BACKGROUND":
            postMessage(await removeBackground(data.images));
            break;

        default:
            break;
    }
}
