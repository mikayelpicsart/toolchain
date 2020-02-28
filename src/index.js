import RemoveBackgroundWorker from 'web-worker:./workers/build/removeBackground-iife';

const removeBackgroundWorker = new RemoveBackgroundWorker();



export function removeBackgroundBulk(imagesSrc) {
    const message = { cmd: "REMOVE_BACKGROUND", images: imagesSrc };
    removeBackgroundWorker.postMessage(JSON.stringify(message));
}

removeBackgroundWorker.onmessage = function (e) {
    const img = document.createElement('img');
    img.src = URL.createObjectURL(e.data);
    document.body.appendChild(img);
}
