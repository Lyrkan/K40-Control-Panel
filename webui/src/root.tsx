import { Outlet, NavLink } from "react-router-dom";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
    faToggleOff,
    faMagnifyingGlass,
    faArrowDownUpAcrossLine,
    faGear,
} from "@fortawesome/free-solid-svg-icons";

import "milligram/dist/milligram.css";
import "rc-switch/assets/index.css";
import "./index.scss";

import Logo from "./assets/logo-transparent.png";

const Root = () => {
    return (
        <>
            <header>
                <div className="container">
                    <img className="logo" src={Logo}></img>
                    <nav>
                        <NavLink to="/">
                            <FontAwesomeIcon
                                className="icon"
                                icon={faMagnifyingGlass}
                            ></FontAwesomeIcon>
                            Status
                        </NavLink>
                        <NavLink to="/controls">
                            <FontAwesomeIcon
                                className="icon"
                                icon={faToggleOff}
                            ></FontAwesomeIcon>
                            Controls
                        </NavLink>
                        <NavLink to="/bed">
                            <FontAwesomeIcon
                                className="icon"
                                icon={faArrowDownUpAcrossLine}
                            ></FontAwesomeIcon>
                            Bed
                        </NavLink>
                        <NavLink to="/settings">
                            <FontAwesomeIcon
                                className="icon"
                                icon={faGear}
                            ></FontAwesomeIcon>
                            Settings
                        </NavLink>
                    </nav>
                </div>
            </header>
            <div className="container">
                <Outlet />
            </div>
        </>
    );
};

export default Root;
