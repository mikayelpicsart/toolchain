
const cryptoJs = require('crypto-js');
const uuid = require('uuid/v1');
const TOKEN = process.env.AI_TOKEN || '';
const AI_API_BASE_URL = process.env.AI_API_BASE_URL || '';

function genToken(sid = uuid()) {
    const decoded = cryptoJs.AES.decrypt(TOKEN, "").toString(cryptoJs.enc.Utf8);
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

function AiFetch(url, options) {
    return fetchWithTimeout(`${AI_API_BASE_URL}/${url}`, rest);
}

export function removeBackground(image) {
    const formData = new FormData();
    formData.append('image', image);
    const [token, sid] = genToken();
    const options = {
        body: formData,
        method: 'POST',
        headers: { sid, Authorization: `Bearer ${token}` },
    }
    return AiFetch(`/matting/${auth.sid}`, { ...options });
}

