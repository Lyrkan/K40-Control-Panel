import { render } from "preact";
import { createHashRouter, RouterProvider } from "react-router-dom";

import Root from "./root.tsx";
import Status from "./pages/status/index.tsx";
import Controls from "./pages/controls/index.tsx";
import Bed from "./pages/bed/index.tsx";
import Settings from "./pages/settings/index.tsx";
import NotFound from "./pages/404/index.tsx";

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
                element: <Controls />,
            },
            {
                path: "bed",
                element: <Bed />,
            },
            {
                path: "settings",
                element: <Settings />,
            },
        ],
    },
]);

render(
    <RouterProvider router={router} />,
    document.getElementById("app") as HTMLElement
);
