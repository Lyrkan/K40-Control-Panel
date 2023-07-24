import { useStore } from "@nanostores/preact";
import { statusStore, Status as StatusType } from "../../stores/status.js";
import StatusService from "../../services/status.ts";
import { useInterval } from "usehooks-ts";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faToggleOff } from "@fortawesome/free-solid-svg-icons";
import Switch from "rc-switch";

import styles from "./styles.module.scss";

const Controls = () => {
    const status = useStore(statusStore) as StatusType;

    useInterval(async () => {
        await StatusService.updateSensors();
        await StatusService.updateAlerts();
    }, 2500);

    return (
        <div className={styles.container}>
            <div className="row card">
                <div className="column">
                    <h3>
                        <FontAwesomeIcon
                            className="icon"
                            icon={faToggleOff}
                        ></FontAwesomeIcon>
                        Controls
                    </h3>
                    <table>
                        <tr>
                            <th>Laser</th>
                            <td>
                                <Switch defaultChecked={status.relays.laser} />
                            </td>
                            <td>
                                Allow laser to be fired.
                                <br />
                                Can only be enabled if cooling is active and all
                                lids are closed.
                            </td>
                        </tr>
                        <tr>
                            <th>Air assist</th>
                            <td>
                                <Switch
                                    defaultChecked={status.relays.air_assist}
                                />
                            </td>
                            <td>Enable air assist pump</td>
                        </tr>
                        <tr>
                            <th>Cooling</th>
                            <td>
                                <Switch
                                    defaultChecked={status.relays.cooling}
                                />
                            </td>
                            <td>Enable cooling pump</td>
                        </tr>
                        <tr>
                            <th>Lights</th>
                            <td>
                                <Switch defaultChecked={status.relays.lights} />
                            </td>
                            <td>Enable inner lights</td>
                        </tr>
                        <tr>
                            <th>Beam preview</th>
                            <td>
                                <Switch
                                    defaultChecked={status.relays.beam_preview}
                                />
                            </td>
                            <td>Enable beam preview laser diodes</td>
                        </tr>
                    </table>
                </div>
            </div>
        </div>
    );
};

export default Controls;
