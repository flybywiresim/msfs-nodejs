'use strict';

import commonjs from '@rollup/plugin-commonjs';
import typescript from 'rollup-plugin-typescript2';
import nativePlugin from 'rollup-plugin-natives';

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
            exclude: ['**/*.test.ts', '**/*.spec.ts'],
        }),
        nativePlugin({
            copyTo: 'dist/libs',
            destDir: './libs',
            dlopen: false,
            map: (_modulePath) => 'msfs.node',
            sourcemap: true,
            targetEsm: false,
        }),
    ],
};
