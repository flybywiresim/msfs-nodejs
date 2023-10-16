'use strict';

import commonjs from '@rollup/plugin-commonjs';
import copy from 'rollup-plugin-copy';
import typescript from 'rollup-plugin-typescript2';

export default {
    input: 'src/index.ts',
    output: [
        {
            file: 'dist/index.js',
            format: 'cjs',
            sourcemap: true,
        },
        {
            file: 'dist/index.esm.js',
            format: 'esm',
            sourcemap: true,
        },
    ],
    plugins: [
        commonjs(),
        typescript({
            useTsconfigDeclarationDir: true,
            exclude: ['**/*.test.ts', '**/*.spec.ts', 'examples/**/*.ts'],
        }),
        copy({
            targets: [
                { src: 'src/bindings/simconnect/build/Release/simconnect.node', dest: 'dist/libs/' },
            ],
        }),
    ],
};
