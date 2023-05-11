import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faSpinner } from "@fortawesome/free-solid-svg-icons";

import styles from "./styles.module.scss";

const Loading = () => (
    <div className={styles.spinner}>
        <FontAwesomeIcon
            className={styles.icon}
            icon={faSpinner}
        ></FontAwesomeIcon>
        Loading...
    </div>
);

export default Loading;
