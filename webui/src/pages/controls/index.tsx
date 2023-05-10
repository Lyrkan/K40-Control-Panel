import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faToggleOff } from "@fortawesome/free-solid-svg-icons";
import styles from "./styles.module.scss";

const Controls = () => (
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
                        <td>Enabled</td>
                        <td>
                            Allow laser to be fired.
                            <br />
                            Can only be enabled if cooling is active and all
                            lids are closed.
                        </td>
                    </tr>
                    <tr>
                        <th>Air assist</th>
                        <td>Enabled</td>
                        <td>Enable air assist pump</td>
                    </tr>
                    <tr>
                        <th>Cooling</th>
                        <td>Enabled</td>
                        <td>Enable cooling pump</td>
                    </tr>
                    <tr>
                        <th>Lights</th>
                        <td>Enabled</td>
                        <td>Enable inner lights</td>
                    </tr>
                    <tr>
                        <th>Diodes</th>
                        <td>Enabled</td>
                        <td>Enable positioning laser diodes</td>
                    </tr>
                </table>
            </div>
        </div>
    </div>
);

export default Controls;
