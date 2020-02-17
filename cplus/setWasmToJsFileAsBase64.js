var args = process.argv.slice(2);
var jsSourcePath = args[0];
var wasmSourcePath = args[1];
var projectName = args[2];
const fs = require('fs');
const path = require('path');
console.log(jsSourcePath, wasmSourcePath, projectName);
function base64_encode(file) {
    // read binary data
    var bitmap = fs.readFileSync(file);
    // convert binary data to base64 encoded string
    return new Buffer.from(bitmap).toString('base64');
}

const jsSource = fs.readFileSync(jsSourcePath, "utf8");

var result = jsSource.replace(`var wasmBinaryFile="${projectName}.wasm";`, `var wasmBinaryFile='data:application/octet-stream;base64,${base64_encode(wasmSourcePath)}';`);
fs.writeFileSync(path.join(__dirname, "../src/helpers/", `${projectName}.js`), result.concat("export default Module;"));
