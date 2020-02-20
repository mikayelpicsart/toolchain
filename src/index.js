import RemoveBackgroundWorker from 'web-worker:./workers/build/removeBackground-iife';

const removeBackgroundWorker = new RemoveBackgroundWorker();



export function removeBackgroundBulk(imagesSrc) {
    const message = { cmd: "REMOVE_BACKGROUND", images: imagesSrc };
    removeBackgroundWorker.postMessage(JSON.stringify(message));
}

removeBackgroundWorker.onmessage = function (e) {
    //console.log(e, e.data);
}
