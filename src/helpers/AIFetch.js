
import cryptoJs from 'crypto-js';
import uuid from 'uuid/v1';

const TOKEN = process.env.AI_TOKEN || '';
const AI_API_BASE_URL = process.env.AI_API_BASE_URL || '';
const AI_TOKEN_SECRET_KEY = process.env.AI_TOKEN_SECRET_KEY || '';

function genToken(sid = uuid()) {
    const decoded = cryptoJs.AES.decrypt(TOKEN, AI_TOKEN_SECRET_KEY).toString(cryptoJs.enc.Utf8);
    const plaintext = decoded.replace(/r/ig, '');
    const auth = cryptoJs.SHA256(sid + plaintext);
    return [auth, sid];
}

function fetchWithTimeout(url, options) {
    const { timeout = 15000, ...rest } = options;
    const controller = new AbortController();
    const { signal } = controller;

    return new Promise((resolve, reject) => {
        const timer = setTimeout(() => {
            reject(new Error('Timeout for request ' + url));
            controller.abort();
        }, timeout);

        fetch(url, { signal, ...rest })
            .then(resolve, reject)
            .finally(() => clearTimeout(timer));
    });
};

async function AiFetch(url, options) {
    try {
        const response = await fetchWithTimeout(`${AI_API_BASE_URL}/${url}`, options);
        if (!response.ok) throw new Error(`response status is ${response.status}`);
        return response;
    } catch (error) {
        throw error;
    }
}

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

