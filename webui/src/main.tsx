import { render } from "preact";
import { lazy, Suspense } from "preact/compat";
import { createBrowserRouter, RouterProvider } from "react-router-dom";
import StatusService from "./services/status.ts";

import Root from "./root.tsx";
import Loading from "./pages/loading/index.tsx";

const Status = lazy(() => import("./pages/status/index.tsx"));
const Controls = lazy(() => import("./pages/controls/index.tsx"));
const Bed = lazy(() => import("./pages/bed/index.tsx"));
const Settings = lazy(() => import("./pages/settings/index.tsx"));
const NotFound = lazy(() => import("./pages/404/index.tsx"));

const lazyElement = (component: any) => (
    <Suspense fallback={<Loading />}>{component}</Suspense>
);

const router = createBrowserRouter([
    {
        path: "/",
        element: <Root />,
        errorElement: <NotFound />,
        children: [
            {
                index: true,
                element: lazyElement(<Status />),
                loader: async () => {
                    return {
                        ...(await StatusService.updateSensors()),
                        ...(await StatusService.updateAlerts()),
                    };
                },
            },
            {
                path: "controls",
                element: lazyElement(<Controls />),
                loader: async () => {
                    return StatusService.updateRelays();
                },
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
