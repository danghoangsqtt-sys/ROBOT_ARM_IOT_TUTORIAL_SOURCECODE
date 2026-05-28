### 1. REST API Specification

| Endpoint | Method | Query/POST Parameters | Description | Sample JSON Response |
| :--- | :--- | :--- | :--- | :--- |
| `/api/status` | `GET` | *None* | Get real-time joint positions, speeds, target angles, and motion status. | See status snippet below |
| `/api/info` | `GET` | *None* | Retrieve hardware mechanical limits (min, max, home) for all joints. | See info snippet below |
| `/api/move` | `POST` | `id` (0-5), `angle` (min..max) | Direct single-joint movement command. | `{"status":"success","msg":"OK"}` |
| `/api/move_all` | `POST` | `angles` (space-separated) OR `a0`..`a5` | Drive multiple joints simultaneously. | `{"status":"success","msg":"OK"}` |
| `/api/speed` | `POST` | `id` (0-5), `speed` (1-20) | Set steps size (deg/tick) for a specific joint. | `{"status":"success","msg":"OK"}` |
| `/api/home` | `POST` | `id` (optional, 0-5) | Drive a single joint or all joints to home positions. | `{"status":"success","msg":"OK"}` |
| `/api/stop` | `POST` | *None* | **Emergency Stop**: Immediately halt all joint movements. | `{"status":"success","msg":"OK"}` |
| `/api/wait` | `POST` | *None* | Cooperatively blocks the HTTP request until all movement stops. | `{"status":"success","msg":"DONE"}` |
| `/api/command` | `POST` | `cmd` (Serial string) | Raw command bridge. Runs serial command line and returns exact response. | `{"status":"success","response":"STA:90,150,90,90,90,90"}` |

---

### API Payloads & Telemetry Examples

#### `GET /api/status`
```json
{
  "status": "success",
  "moving": false,
  "servos": [
    { "id": 0, "name": "Base", "angle": 90, "target": 90, "speed": 1, "moving": false },
    { "id": 1, "name": "Shoulder", "angle": 150, "target": 150, "speed": 1, "moving": false },
    { "id": 2, "name": "Elbow", "angle": 90, "target": 90, "speed": 1, "moving": false },
    { "id": 3, "name": "Wrist Pitch", "angle": 90, "target": 90, "speed": 1, "moving": false },
    { "id": 4, "name": "Wrist Roll", "angle": 90, "target": 90, "speed": 1, "moving": false },
    { "id": 5, "name": "Gripper", "angle": 90, "target": 90, "speed": 1, "moving": false }
  ]
}
```

#### `GET /api/info`
```json
{
  "status": "success",
  "servos": [
    { "id": 0, "name": "Base", "min": 0, "max": 180, "home": 90, "defaultSpeed": 1 },
    { "id": 1, "name": "Shoulder", "min": 0, "max": 180, "home": 150, "defaultSpeed": 1 },
    { "id": 2, "name": "Elbow", "min": 0, "max": 150, "home": 90, "defaultSpeed": 1 },
    { "id": 3, "name": "Wrist Pitch", "min": 0, "max": 180, "home": 90, "defaultSpeed": 1 },
    { "id": 4, "name": "Wrist Roll", "min": 0, "max": 180, "home": 90, "defaultSpeed": 1 },
    { "id": 5, "name": "Gripper", "min": 60, "max": 120, "home": 90, "defaultSpeed": 1 }
  ]
}
```
