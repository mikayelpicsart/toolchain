import resolve from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';
import babel from '@rollup/plugin-buble';
import replace from '@rollup/plugin-replace';
import glslify from 'rollup-plugin-glslify';
import pkg from './package.json';
require('dotenv').config();

const env = process.env.NODE_ENV

const config = {
    input: 'index.js',
    //external: Object.keys(pkg.peerDependencies || {}),
    output: [
        { file: `dist/${pkg.name}-amd.js`, format: 'amd' },
        { file: `dist/${pkg.name}-es.js`, format: 'es' },
        { file: `dist/${pkg.name}-esm.js`, format: 'esm' },
        { file: `dist/${pkg.name}-cjs.js`, format: 'cjs' },
        { file: `dist/${pkg.name}-umd.js`, format: 'umd', name: pkg.umdModuleName },
        { file: `dist/${pkg.name}-iife.js`, format: 'iife', name: pkg.umdModuleName }
    ],
    plugins: [
        resolve({
            mainFields: ['browser', 'jsnext', 'main']
        }),
        commonjs({
            include: ["./index.js", "node_modules/**"]
        }),
        babel({ exclude: '**/node_modules/**', babelrc: true, objectAssign: 'Object.assign', runtimeHelpers: true }),
        replace({
            'process.env.NODE_ENV': JSON.stringify(env)
        }),
        glslify({ basedir: 'src/shaders' })
    ]
}

export default config
