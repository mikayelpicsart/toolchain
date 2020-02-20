import { genToken, AiFetch } from '../helpers/AIFetch';
import { loadImage, resizeIfNeededImage, upScaleImage } from '../helpers';

export async function removeBackground(file) {
    const formData = new FormData();
    formData.append('image', file);
    const [token, sid] = genToken();
    const options = {
        body: formData,
        method: 'POST',
        headers: { sid, Authorization: `Bearer ${token}` },
    }
    const response = await AiFetch(`matting/${sid}`, { ...options });
    return await response.json();
}

export async function createPngFromMask (maskUrl, originalIMage) {
    const mask = await loadImage(maskUrl);
    const maskImage = await upScaleImage(mask, originalIMage.width, originalIMage.height);
    const canvas = document.createElement('canvas');
    canvas.width = originalIMage.width;
    canvas.height = originalIMage.height;
    const ctx = canvas.getContext('2d');
    ctx.drawImage(originalIMage, 0, 0);
    ctx.globalCompositeOperation = 'destination-in';
    ctx.drawImage(maskImage, 0, 0);
    return await new Promise((resolve, reject) => {
        canvas.toBlob((blob) => resolve(blob), 'image/jpeg');
    });
}

async function removeBackgroundMulti(srcArray = []) {
    const imagesArray = await Promise.all(srcArray.map(src => loadImage(src))); // original
    const resizedImagesArray = await Promise.all(imagesArray.map(image => resizeIfNeededImage(image, 512)));
    const maskArray = await Promise.all(resizedImagesArray.map(blob => removeBackground(new File([blob], 'image.jpeg'))));
    const imageDataUrlArray = await Promise.all(imagesArray
        .map((image, index) => {
            const { data: { url: maskUrl } } = maskArray[index];
            return createPngFromMask(maskUrl, image)
        })
    )
    return imageDataUrlArray;
}

export async function removeBackgroundBulk (srcArray = [], callback) {
    const arrayOfArray = [];
    while(srcArray.length !== 0) {
        arrayOfArray.push(srcArray.splice(0, 10));
    }
    console.log(arrayOfArray);
    var lastPromise = null
    for (let i = 0, p = Promise.resolve(); i < arrayOfArray.length; i++) {
        lastPromise = p = p.then(data => {
            i && callback(data, false);
            return removeBackgroundMulti(arrayOfArray[i]);
        });
    }
    lastPromise.then(data => { callback(data, false); callback(null, true) });
}