import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faGear } from "@fortawesome/free-solid-svg-icons";

const Settings = () => (
    <div className="row card">
        <div className="column">
            <h3>
                <FontAwesomeIcon
                    className="icon"
                    icon={faGear}
                ></FontAwesomeIcon>
                Settings
            </h3>
        </div>
    </div>
);

export default Settings;
