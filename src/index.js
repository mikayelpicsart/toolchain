import RemoveBackgroundWorker from 'web-worker:./workers/build/removeBackground-iife';

const removeBackgroundWorker = new RemoveBackgroundWorker();

export function removeBackgroundBulk(imagesSrc) {
    const message = { cmd: "REMOVE_BACKGROUND", images: imagesSrc };
    removeBackgroundWorker.postMessage(JSON.stringify(message));
}

removeBackgroundWorker.onmessage = function (e) {
    /**********************************JPEG********************************************* */
    // const imageData = e.data[0];
    // const canvas = document.createElement('canvas');
    // const ctx = canvas.getContext('2d');
    // canvas.width = imageData.width;
    // canvas.height = imageData.height;
    // const imgData = ctx.createImageData(imageData.width, imageData.height);
    // const data = imgData.data;
    // console.log(imageData);
    // for (var i = 0, len = imageData.width * imageData.height; i < len; i++) {
    //     data[i * 4] = imageData.data[i * 3];
    //     data[i * 4 + 1] = imageData.data[i * 3 + 1];
    //     data[i * 4 + 2] = imageData.data[i * 3 + 2];
    //     data[i * 4 + 3] = 255;
    // }
    // console.log(imgData);
    // ctx.putImageData(imgData, 0, 0);
    // document.body.appendChild(canvas);

    //******************************************************************************* */
    /**********************************PNG********************************************* */
    
    e.data.forEach(imageData => {
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        canvas.width = imageData.width;
        canvas.height = imageData.height;
        console.log(imageData);
        const imgData = new ImageData(imageData.data, imageData.width, imageData.height);
        ctx.putImageData(imgData, 0, 0);
        document.body.appendChild(canvas);
    });
    

    //******************************************************************************* */
    // const img = document.createElement('img');
    // img.src = URL.createObjectURL(e.data[0]);
    // document.body.appendChild(img);
    //******************************************************************************* */
    console.log(e.data);
}
