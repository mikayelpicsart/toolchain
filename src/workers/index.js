
import { removeBackground } from './removeBackground';

self.onmessage = function (e) {
    console.log(e.data);
    const { cmd, ...data } = JSON.parse(e.data);
    switch (cmd) {
        case "REMOVE_BACKGROUND":
            removeBackground(data.images);
            break;

        default:
            break;
    }
    postMessage('ok');
}
