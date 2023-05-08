import { Outlet, NavLink } from "react-router-dom";

import "milligram/dist/milligram.css";
import "./index.scss";

import Logo from "./assets/logo-transparent.png";

const Root = () => {
    return (
        <>
            <header>
                <img className="logo" src={Logo}></img>
                <nav>
                    <NavLink to="/">Status</NavLink>
                    <NavLink to="/controls">Controls</NavLink>
                    <NavLink to="/bed">Bed</NavLink>
                    <NavLink to="/settings">Settings</NavLink>
                </nav>
            </header>
            <div className="container">
                <Outlet />
            </div>
        </>
    );
};

export default Root;
