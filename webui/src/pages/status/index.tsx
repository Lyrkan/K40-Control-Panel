import styles from "./styles.module.scss";

export default function Status() {
    return (
        <div className={styles.container}>
            <div className={`row ${styles.voltage}`}>
                <div className="column column-25">
                    <strong>V1:</strong> 4.2V
                    <br />
                    <strong>V2:</strong> 12.1V
                    <br />
                    <strong>V3:</strong> 23.9V
                </div>
                <div className="column column-75">Graph</div>
            </div>
            <div className={`row ${styles.cooling}`}>
                <div className="column column-25">
                    <strong>Flow:</strong> 1.5L/mn
                    <br />
                    <strong>Temperature:</strong> 19.4°C
                </div>
                <div className="column column-75">Graph</div>
            </div>
            <div className={`row ${styles.lids}`}>
                <div className="column">
                    <strong>Front lid:</strong> Opened
                    <br />
                    <strong>Back lid:</strong> Opened
                    <br />
                </div>
            </div>
            <div className={`row ${styles.fire}`}>
                <div className="column">
                    <strong>Flame sensor:</strong> OK
                </div>
            </div>
        </div>
    );
}
