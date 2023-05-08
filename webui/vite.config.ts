import { defineConfig } from "vite";
import preact from "@preact/preset-vite";

// https://vitejs.dev/config/
export default defineConfig({
    plugins: [preact()],
    build: {
        rollupOptions: {
            output: {
                entryFileNames: "[hash:6].js",
                assetFileNames: "[hash:6].[ext]",
                chunkFileNames: "[hash:6].js",
            },
        },
    },
    resolve: {
        alias: {
            react: "preact/compat",
            "react-dom": "preact/compat",
        },
    },
});
