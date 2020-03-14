import { genToken, AiFetch } from '../helpers/AIFetch';
import { loadImage, resizeIfNeededImage, upScaleImage } from '../helpers';
import { openDB } from 'idb';

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

export async function createPngFromMask(maskUrl, originalIMage) {
    const mask = await loadImage(maskUrl);
    const maskImage = await upScaleImage(mask, originalIMage.width, originalIMage.height);
    const canvas = document.createElement('canvas');
    canvas.width = originalIMage.width;
    canvas.height = originalIMage.height;
    const ctx = canvas.getContext('2d');
    ctx.drawImage(originalIMage, 0, 0);
    ctx.globalCompositeOperation = 'destination-in';
    ctx.drawImage(maskImage, 0, 0);
    document.body.append(canvas);
    return new Promise((resolve, reject) => {
        canvas.toBlob((blob) => resolve(blob))
    });
}

async function removeBackgroundInDepend(src) {
    const image = await loadImage(src);
    const blob = await resizeIfNeededImage(image, 1024);
    const { data: { url: maskUrl } } = await removeBackground(new File([blob], 'image.jpeg'));
    return await createPngFromMask(maskUrl, image);
}

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


export async function removeBackgroundBulk(srcArray = [], callback) {
    const db = await openDB('PicsArt Web Action', 1, {
        upgrade(db, oldVersion, newVersion, transaction) {
            console.log('upgrade oldVersion: ' + oldVersion + ' newVersion: ' + newVersion)
        },
        blocked() {
            console.log('blocked')
        },
        blocking() {
            console.log('blocking')
        },
        terminated() {
            console.log('terminated')
        },
    });
    const store = db.transaction('DataStore', 'readwrite').objectStore('DataStore');
    srcArray.forEach(async function (id) {
        const { url: src } = await store.get(id);
        console.log('src :', data);
        const blob = await removeBackgroundInDepend(src); {
            const tx = db.transaction('DataStore', 'readwrite');
            const store = tx.objectStore('DataStore');
            store.put(blob, id);
            await tx.done;
        }
        callback(id);
    });
}