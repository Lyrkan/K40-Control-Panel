import { render } from "preact";
import { lazy, Suspense } from "preact/compat";
import { createHashRouter, RouterProvider } from "react-router-dom";

import Root from "./root.tsx";
import Status from "./pages/status/index.tsx";

const Controls = lazy(() => import("./pages/controls/index.tsx"));
const Bed = lazy(() => import("./pages/bed/index.tsx"));
const Settings = lazy(() => import("./pages/settings/index.tsx"));
const NotFound = lazy(() => import("./pages/404/index.tsx"));

const lazyElement = (component: any) => (
    <Suspense fallback={<div>Loading...</div>}>{component}</Suspense>
);

const router = createHashRouter([
    {
        path: "/",
        element: <Root />,
        errorElement: <NotFound />,
        children: [
            {
                index: true,
                element: <Status />,
            },
            {
                path: "controls",
                element: lazyElement(<Controls />),
            },
            {
                path: "bed",
                element: lazyElement(<Bed />),
            },
            {
                path: "settings",
                element: lazyElement(<Settings />),
            },
        ],
    },
]);

render(
    <RouterProvider router={router} />,
    document.getElementById("app") as HTMLElement
);
