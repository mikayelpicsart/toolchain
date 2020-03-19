import resolve from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';
import babel from 'rollup-plugin-babel';
import replace from '@rollup/plugin-replace';
import glslify from 'rollup-plugin-glslify';
import { terser } from 'rollup-plugin-terser'
import pkg from './package.json';
require('dotenv').config();

const env = process.env.NODE_ENV

const configMain = {
    input: 'src/index.js',
    external: Object.keys(pkg.peerDependencies || {}),
    output: {
        file: `dist/${pkg.name}-umd.js`,
        format: 'umd',
        name: pkg.umdModuleName,
        globals: {
            'uuid': 'uuid',
            'uuid/v1': 'uuidV1',
            'crypto-js': 'cryptoJs'
        },
    },
    plugins: [
        resolve({
            mainFields: ['browser', 'jsnext', 'main']
        }),
        commonjs({
            include: ["node_modules/**"]
        }),
        babel({ exclude: 'node_modules/**', runtimeHelpers: true }),
        replace({
            'process.env.NODE_ENV': JSON.stringify(process.env.NODE_ENV),
            'process.env.AI_TOKEN': JSON.stringify(process.env.AI_TOKEN),
            'process.env.AI_TOKEN_SECRET_KEY': JSON.stringify(process.env.AI_TOKEN_SECRET_KEY),
            'process.env.AI_API_BASE_URL': JSON.stringify(process.env.AI_API_BASE_URL)
        }),
        glslify({ basedir: 'src/shaders' }),
        terser({
            compress: {
                pure_getters: true,
                unsafe: true,
                unsafe_comps: true,
                warnings: false
            }
        })
    ]
}

export default [configMain];
