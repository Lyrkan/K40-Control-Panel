import { useStore } from "@nanostores/preact";
import { statusStore, Status as StatusType } from "../../stores/status.js";
import StatusService from "../../services/status.ts";
import { useInterval } from "usehooks-ts";
import { formatNumber } from "../../utils/strings.ts";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
    faBolt,
    faEye,
    faFire,
    faSnowflake,
} from "@fortawesome/free-solid-svg-icons";

import styles from "./styles.module.scss";

export default function Status() {
    const status = useStore(statusStore) as StatusType;

    useInterval(async () => {
        await StatusService.updateSensors();
        await StatusService.updateAlerts();
    }, 2500);

    return (
        <div className={styles.container}>
            <div className={`row card ${styles.voltage}`}>
                <div className="column column-25">
                    <h3>
                        <FontAwesomeIcon
                            className="icon"
                            icon={faBolt}
                        ></FontAwesomeIcon>
                        Voltages
                    </h3>
                    <div className={styles.values}>
                        <strong>V1:</strong>{" "}
                        {formatNumber(status.sensors.voltages.v1, {
                            fractionDigits: 2,
                            fallback: "Unknown",
                            unit: "V",
                        })}
                        <br />
                        <strong>V2:</strong>{" "}
                        {formatNumber(status.sensors.voltages.v2, {
                            fractionDigits: 2,
                            fallback: "Unknown",
                            unit: "V",
                        })}
                        <br />
                        <strong>V3:</strong>{" "}
                        {formatNumber(status.sensors.voltages.v3, {
                            fractionDigits: 2,
                            fallback: "Unknown",
                            unit: "V",
                        })}
                    </div>
                </div>
                <div className="column column-75">Graph</div>
            </div>
            <div className={`row card ${styles.cooling}`}>
                <div className="column column-25">
                    <h3>
                        <FontAwesomeIcon
                            className="icon"
                            icon={faSnowflake}
                        ></FontAwesomeIcon>
                        Cooling
                    </h3>
                    <div className={styles.values}>
                        <strong>Flow:</strong>{" "}
                        {formatNumber(status.sensors.cooling.flow, {
                            fractionDigits: 2,
                            fallback: "Unknown",
                            unit: "L/mn",
                        })}
                        <br />
                        <strong>Temperature:</strong>{" "}
                        {formatNumber(status.sensors.cooling.temp, {
                            fractionDigits: 2,
                            fallback: "Unknown",
                            unit: "°C",
                        })}
                    </div>
                </div>
                <div className="column column-75">Graph</div>
            </div>
            <div className={`row card ${styles.lids}`}>
                <div className="column">
                    <h3>
                        <FontAwesomeIcon
                            className="icon"
                            icon={faEye}
                        ></FontAwesomeIcon>
                        Lids
                    </h3>
                    <div className={styles.values}>
                        <strong>Front:</strong>{" "}
                        {status.sensors.lids.front ?? "Unknown"}
                        <br />
                        <strong>Back:</strong>{" "}
                        {status.sensors.lids.back ?? "Unknown"}
                    </div>
                </div>
            </div>
            <div className={`row card ${styles.fire}`}>
                <div className="column">
                    <h3>
                        <FontAwesomeIcon
                            className="icon"
                            icon={faFire}
                        ></FontAwesomeIcon>
                        Flame sensor
                    </h3>
                    <div className={styles.values}>
                        <strong>Status:</strong>{" "}
                        {status.sensors.flame_sensor.triggered
                            ? "Triggered"
                            : "OK"}
                    </div>
                </div>
            </div>
        </div>
    );
}
