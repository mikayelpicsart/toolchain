export async function getFileFromUrl(imageSrc, resize = false, maxSize = 1024) {
    if (typeof imageSrc !== 'string') throw new Error('imageSrc must be string but got' + typeof imageSrc);
    let blob = null;
    if (resize) {
        blob = await resize(imageSrc, maxSize);
    } else {
        const response = await fetch(imageSrc);
        blob = await response.blob();
    }
    return new File([blob], 'image.jpeg');
}

export async function resize(imageSrc, maxSize) {
    const image = await new Promise((resolve, reject) => {
        const image = new Image();
        image.addEventListener('load', () => {
            resolve(image);
        }, { once: true });
        image.src = imageSrc;
    });
    let ratio = Math.min(maxSize / image.width, maxSize / image.height);
    ratio = ratio >= 1 ? 1 : ratio;
    const canvas = document.createElement('canvas');
    canvas.width = image.width * ratio;
    canvas.height = image.height * ratio
    const ctx = canvas.getContext('2d');
    ctx.drawImage(image, 0, 0, image.width, image.height, 0, 0, image.width * ratio, image.height * ratio);
    return await new Promise((resolve, reject) => {
        canvas.toBlob((blob) => resolve(blob))
    });
}