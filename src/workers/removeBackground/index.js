import { genToken, AiFetch, fetchWithTimeout } from '../../helpers/AIFetch';
import wasmModule from '../../helpers/image';
import 'formdata-polyfill';

const defer = {};
defer.promise = new Promise((resolve, reject) => {
    defer.resolve = resolve;
    defer.reject = reject;
});

wasmModule.onRuntimeInitialized = function () {
    defer.resolve && defer.resolve();
}

function Uint8ToNumber(Uint8) {
    let num = 0;
    for (let index = 0; index < Uint8.length; index++) {
        num = (num + Uint8[index]) << !!(Uint8.length - 1 - index) * 8;
    }
    return num;
}

export async function removeBackground(imagesSrc) {
    const [uintArray] = await Promise.all(imagesSrc.map(src => getUintArrayFromSrc(src)));
    await defer.promise; // await to onRuntimeInitialized
    // wasmModule._print_tests();
    const offset = wasmModule._resize_image(_arrayToHeap(uintArray).byteOffset, uintArray.length);
    const lengthUInt8 = wasmModule.HEAPU8.subarray(offset, offset + 4);
    const length = Uint8ToNumber(lengthUInt8);
    const data = wasmModule.HEAPU8.subarray(offset + 4, offset + 4 + length);
    return new Blob([data], { type: 'image/jpeg' });
}

// async function removeBackground(file) {
//     const formData = new FormData();
//     formData.append('image', file);
//     const [token, sid] = genToken();
//     const options = {
//         body: formData,
//         method: 'POST',
//         headers: { sid, Authorization: `Bearer ${token}` },
//     }
//     const response = await AiFetch(`matting/${sid}`, { ...options });
//     return await response.json();
// }

// async function createPngFromMask (maskUrl, originalIMage) {
//     const mask = await loadImage(maskUrl);
//     const maskImage = await upScaleImage(mask, originalIMage.width, originalIMage.height);
//     const canvas = document.createElement('canvas');
//     canvas.width = originalIMage.width;
//     canvas.height = originalIMage.height;
//     const ctx = canvas.getContext('2d');
//     ctx.drawImage(originalIMage, 0, 0);
//     ctx.globalCompositeOperation = 'destination-in';
//     ctx.drawImage(maskImage, 0, 0);
//     document.body.append(canvas);
//     return canvas.toDataURL();
// }

// async function removeBackgroundMulti(srcArray = []) {
//     const imagesArray = await Promise.all(srcArray.map(src => loadImage(src))); // original
//     const resizedImagesArray = await Promise.all(imagesArray.map(image => resizeIfNeededImage(image, 512)));
//     const maskArray = await Promise.all(resizedImagesArray.map(blob => removeBackground(new File([blob], 'image.jpeg'))));
//     const imageDataUrlArray = await Promise.all(imagesArray
//         .map((image, index) => {
//             const { data: { url: maskUrl } } = maskArray[index];
//             return createPngFromMask(maskUrl, image)
//         })
//     )
//     return imageDataUrlArray;
// }

// async function removeBackgroundBulk (srcArray = [], callback) {
//     const arrayOfArray = [];
//     while(srcArray.length !== 0) {
//         arrayOfArray.push(srcArray.splice(0, 10));
//     }
//     console.log(arrayOfArray);
//     var lastPromise = null
//     for (let i = 0, p = Promise.resolve(); i < arrayOfArray.length; i++) {
//         lastPromise = p = p.then(data => {
//             i && callback(data, false);
//             return removeBackgroundMulti(arrayOfArray[i]);
//         });
//     }
//     lastPromise.then(data => { callback(data, true) });
// }





async function getUintArrayFromSrc(src) {
    const response = await fetchWithTimeout(src);
    const arrayBuffer = await response.arrayBuffer();
    //console.log(JPEG.decode(arrayBuffer));
    return new Uint8Array(arrayBuffer);
}

function _freeArray(heapBytes) {
    wasmModule._free(heapBytes.byteOffset);
}
function _arrayToHeap(typedArray) {
    const numBytes = typedArray.length * typedArray.BYTES_PER_ELEMENT;
    const ptr = wasmModule._malloc(numBytes);
    const heapBytes = wasmModule.HEAPU8.subarray(ptr, ptr + numBytes);
    heapBytes.set(typedArray);
    return heapBytes;
}