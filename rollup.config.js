'use strict';

import commonjs from '@rollup/plugin-commonjs';
import copy from 'rollup-plugin-copy';
import typescript from 'rollup-plugin-typescript2';

import packageJson from './package.json';

export default {
    input: 'src/index.ts',
    output: [
        {
            file: packageJson.main,
            format: 'cjs',
            sourcemap: true,
        },
        {
            file: packageJson.module,
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
