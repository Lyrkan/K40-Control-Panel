import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
    faBolt,
    faEye,
    faFire,
    faSnowflake,
} from "@fortawesome/free-solid-svg-icons";
import styles from "./styles.module.scss";

export default function Status() {
    return (
        <div className={styles.container}>
            <div className={`row ${styles.voltage}`}>
                <div className="column column-25">
                    <h3>
                        <FontAwesomeIcon
                            className={styles.icon}
                            icon={faBolt}
                        ></FontAwesomeIcon>
                        Voltages
                    </h3>
                    <div className={styles.values}>
                        <strong>V1:</strong> 4.2V
                        <br />
                        <strong>V2:</strong> 12.1V
                        <br />
                        <strong>V3:</strong> 23.9V
                    </div>
                </div>
                <div className="column column-75">Graph</div>
            </div>
            <div className={`row ${styles.cooling}`}>
                <div className="column column-25">
                    <h3>
                        <FontAwesomeIcon
                            className={styles.icon}
                            icon={faSnowflake}
                        ></FontAwesomeIcon>
                        Cooling
                    </h3>
                    <div className={styles.values}>
                        <strong>Flow:</strong> 1.5L/mn
                        <br />
                        <strong>Temperature:</strong> 19.4°C
                    </div>
                </div>
                <div className="column column-75">Graph</div>
            </div>
            <div className={`row ${styles.lids}`}>
                <div className="column">
                    <h3>
                        <FontAwesomeIcon
                            className={styles.icon}
                            icon={faEye}
                        ></FontAwesomeIcon>
                        Lids
                    </h3>
                    <div className={styles.values}>
                        <strong>Front:</strong> Opened
                        <br />
                        <strong>Back:</strong> Opened
                    </div>
                </div>
            </div>
            <div className={`row ${styles.fire}`}>
                <div className="column">
                    <h3>
                        <FontAwesomeIcon
                            className={styles.icon}
                            icon={faFire}
                        ></FontAwesomeIcon>
                        Flame sensor
                    </h3>
                    <div className={styles.values}>
                        <strong>Status:</strong> OK
                    </div>
                </div>
            </div>
        </div>
    );
}
