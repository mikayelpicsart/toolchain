import { removeBackground } from '../helpers/AIFetch';

// const cc = new OffscreenCanvas(100, 1);
// console.log(cc);

self.addEventListener('message', function (e) {
    const { cmd, payload } = JSON.parse(e.data); 
    console.log(cmd, payload);
    switch (cmd) {
        case 'REMOVE_BULK_BACKGROUND':
            removeBackground(payload).then(data=> self.postMessage(data));
            break;
        default:
            break;
    }
    
}, false);