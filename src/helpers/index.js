export async function loadImage(imageSrc) {
    return await new Promise((resolve, reject) => {
        const image = new Image();
        image.addEventListener('load', () => {
            resolve(image);
        }, { once: true });
        image.addEventListener('error', (error) => {
            reject(error);
        })
        requestCORSIfNotSameOrigin(image, imageSrc);
        image.src = imageSrc;
    });
}
const requestCORSIfNotSameOrigin = (img, url) => {
    if (new URL(url).origin !== window.location.origin) {
        img.crossOrigin = '';
    }
};
export async function resizeIfNeededImage(image, maxSize = 512) {
    let ratio = Math.min(maxSize / image.width, maxSize / image.height);
    ratio = ratio >= 1 ? 1 : ratio;
    const canvas = document.createElement('canvas');
    canvas.width = image.width * ratio;
    canvas.height = image.height * ratio
    const ctx = canvas.getContext('2d');
    ctx.drawImage(image, 0, 0, image.width, image.height, 0, 0, image.width * ratio, image.height * ratio);
    return await new Promise((resolve, reject) => {
        try {
            canvas.toBlob((blob) => resolve(blob))
        } catch (error) {
            reject(error);
        }
        
    });
}

export async function upScaleImage(image, width, height) {
    const canvas = document.createElement('canvas');
    canvas.width = width;
    canvas.height = height
    const ctx = canvas.getContext('2d');
    ctx.drawImage(image, 0, 0, image.width, image.height, 0, 0, width, height);
    return await loadImage(canvas.toDataURL());
}