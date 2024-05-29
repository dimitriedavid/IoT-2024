import { useState } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "./components/ui/card";
import MqttClient from "./MqttClient";
import { Separator } from "./components/ui/separator";
import { Button } from "./components/ui/button";

export default function ControllerCard() {
    const [temperature, setTemperature] = useState("loading");
    const [humidity, setHumidity] = useState("loading");
    const [feelsLike, setFeelsLike] = useState("loading");
    const [targetTemperature, setTargetTemperature] = useState(25);
    const [acOn, setAcOn] = useState("loading");

    return (
        <Card>
            <CardHeader>
                <CardTitle>AC controller</CardTitle>
            </CardHeader>
            <CardContent>
                <div className="flex-column space-y-4 rounded-md border p-4">
                    <p><strong>Temperature:</strong> {temperature} °C</p>
                    <p><strong>Humidity:</strong> {humidity} %</p>
                    <p><strong>Feels like:</strong> {feelsLike} °C</p>
                </div>
                <Separator />
                <div className="pt-6">
                    <p>Set temperature: <strong>{targetTemperature} °C</strong> </p>
                    <div className="flex space-x-4 pt-2 pb-2">
                        {/* lets draw a temperature with a + and - button */}
                        <Button onClick={() => setTargetTemperature(targetTemperature - 1)}>-</Button>
                        <Button onClick={() => setTargetTemperature(targetTemperature + 1)}>+</Button>
                    </div>
                    {acOn === "1" && (
                        <p><strong>AC is on ❄️</strong></p>
                    )}
                </div>
            </CardContent>
            <MqttClient setTemperature={setTemperature} setHumidity={setHumidity} setFeelsLike={setFeelsLike} setAcOn={setAcOn} targetTemperature={targetTemperature} />
        </Card>
    )
}
