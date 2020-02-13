import { removeBackground } from '../helpers/AIFetch';
import { getFileFromUrl } from '../helpers';

export async function removeBackgroundBulk (array = []) {
    const fileArray = await Promise.all(array.map(item => getFileFromUrl(item, true)));
    Promise.all(fileArray.map(file => removeBackground(file))).then(data=> console.log(data));
}