import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faArrowDownUpAcrossLine } from "@fortawesome/free-solid-svg-icons";

const Bed = () => (
    <div className="row card">
        <div className="column">
            <h3>
                <FontAwesomeIcon
                    className="icon"
                    icon={faArrowDownUpAcrossLine}
                ></FontAwesomeIcon>
                Bed
            </h3>
        </div>
    </div>
);

export default Bed;
