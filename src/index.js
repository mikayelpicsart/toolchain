import BulkWorker from 'web-worker:./workers/build/bulk-iife.js';
const bulkWorker = new BulkWorker();

export function removeBulkBackground(arr = []) {
    bulkWorker.postMessage(JSON.stringify({ cmd: 'REMOVE_BULK_BACKGROUND', payload: arr }));
    return Promise.resolve();
}
bulkWorker.onmessage = function (e) {
    console.log( e);
}