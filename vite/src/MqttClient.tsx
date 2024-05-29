import { useEffect, useState } from "react"
import mqtt from 'mqtt'

const HOST = 'mqtt://65.108.235.54:9001';

const TEMPERATURE_TOPIC = 'esp32/temperature';
const HUMIDITY_TOPIC = 'esp32/humidity';
const FEELS_LIKE_TOPIC = 'esp32/heatindex';
const AC_ON_TOPIC = 'esp32/coolerOn';

const TARGET_TEMPERATURE_TOPIC = 'esp32/targetTemperature';

export default function MqttClient({ setTemperature, setHumidity, setFeelsLike, setAcOn, targetTemperature }: { setTemperature: Function, setHumidity: Function, setFeelsLike: Function, setAcOn: Function, targetTemperature: number }) {
    const [client, setClient] = useState<mqtt.MqttClient | null>(null);

    useEffect(() => {
        if (client) {
            client.publish(TARGET_TEMPERATURE_TOPIC, targetTemperature.toString());
            console.log('Published target temperature:', targetTemperature);
        }
    }, [targetTemperature]);

    useEffect(() => {
        const c = mqtt.connect(HOST);
        setClient(c);
        console.log('Connected to MQTT broker:', HOST);

        // subscribe to topics
        c.subscribe(TEMPERATURE_TOPIC);
        c.subscribe(HUMIDITY_TOPIC);
        c.subscribe(FEELS_LIKE_TOPIC);
        c.subscribe(AC_ON_TOPIC);

        console.log('Subscribed to topics:', TEMPERATURE_TOPIC, HUMIDITY_TOPIC, FEELS_LIKE_TOPIC);

        // handle incoming messages
        c.on('message', (topic, message) => {
            console.log('Received message:');
            if (topic === TEMPERATURE_TOPIC) {
                console.log('Temperature:', message.toString());
                setTemperature(message.toString());
            }
            if (topic === HUMIDITY_TOPIC) {
                console.log('Humidity:', message.toString());
                setHumidity(message.toString());
            }
            if (topic === FEELS_LIKE_TOPIC) {
                console.log('Feels like:', message.toString());
                setFeelsLike(message.toString());
            }
            if (topic === AC_ON_TOPIC) {
                console.log('AC is:', message.toString());
                setAcOn(message.toString());
            }
        });
    }, []);

    return (<></>)
}
