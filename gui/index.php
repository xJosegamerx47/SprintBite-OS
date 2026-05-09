<?php
$response = "";
$order = "";

function isServerRunning($host = "127.0.0.1", $port = 5000) {
    $socket = @fsockopen($host, $port, $errno, $errstr, 1);
    if ($socket) {
        fclose($socket);
        return true;
    }
    return false;
}

function sendToServer($message) {
    $host = "127.0.0.1";
    $port = 5000;

    $socket = @fsockopen($host, $port, $errno, $errstr, 3);

    if (!$socket) {
        return "ERROR: No se pudo conectar con el servidor TCP. Enciende el servidor primero.";
    }

    fwrite($socket, $message);
    $response = fgets($socket, 256);
    fclose($socket);

    return $response;
}

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $action = $_POST["action"] ?? "send";
    $order = trim($_POST["order"] ?? "");

    if ($action === "start") {
        if (isServerRunning()) {
            $response = "El servidor ya está encendido en el puerto 5000.";
        } else {
            $baseDir = realpath(__DIR__ . "/..");
            $logFile = $baseDir . "/logs/server_gui.log";

            if (!file_exists($baseDir . "/bin/server")) {
                $response = "ERROR: No existe bin/server. Ejecuta primero ./scripts/build.sh";
            } else {
                shell_exec("cd " . escapeshellarg($baseDir) . " && nohup ./bin/server > " . escapeshellarg($logFile) . " 2>&1 &");
                sleep(1);

                if (isServerRunning()) {
                    $response = "Servidor encendido correctamente en el puerto 5000.";
                } else {
                    $response = "ERROR: Se intentó encender el servidor, pero no respondió. Revisa logs/server_gui.log";
                }
            }
        }
    }

    if ($action === "send") {
        if ($order !== "") {
            $response = sendToServer($order);
        } else {
            $response = "ERROR: Escribe un pedido antes de enviarlo.";
        }
    }

    if ($action === "stop") {
        if (isServerRunning()) {
            $response = sendToServer("exit");
        } else {
            $response = "El servidor ya está apagado.";
        }
    }
}

$serverIsRunning = isServerRunning();
$status = $serverIsRunning ? "ENCENDIDO" : "APAGADO";
$statusClass = $serverIsRunning ? "online" : "offline";

$responseClass = "info";
if ($response !== "") {
    if (stripos($response, "ERROR") !== false) {
        $responseClass = "error";
    } elseif (
        stripos($response, "encendido correctamente") !== false ||
        stripos($response, "accepted") !== false ||
        stripos($response, "Shutdown") !== false ||
        stripos($response, "apagado") !== false
    ) {
        $responseClass = "success";
    }
}
?>
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SprintBite OS</title>
    <style>
        :root{
            --bg:#f7f7fb;
            --card:#ffffff;
            --text:#202124;
            --muted:#6b7280;
            --primary:#ff335f;
            --primary-dark:#e82652;
            --green:#19b46b;
            --red:#ef4444;
            --orange:#ff9f1c;
            --shadow:0 10px 30px rgba(0,0,0,.08);
            --radius:22px;
        }

        *{
            box-sizing:border-box;
        }

        body{
            margin:0;
            font-family: Inter, "Segoe UI", Arial, sans-serif;
            background: var(--bg);
            color: var(--text);
        }

        .topbar{
            background: linear-gradient(135deg, #ff335f, #ff5b7f);
            color:white;
            padding:18px 28px;
            box-shadow:0 6px 18px rgba(255,51,95,.25);
            position:sticky;
            top:0;
            z-index:10;
        }

        .topbar-inner{
            max-width:1200px;
            margin:0 auto;
            display:flex;
            align-items:center;
            justify-content:space-between;
            gap:20px;
            flex-wrap:wrap;
        }

        .brand{
            display:flex;
            flex-direction:column;
        }

        .brand h1{
            margin:0;
            font-size:2rem;
            line-height:1;
            font-weight:800;
            letter-spacing:-.03em;
        }

        .brand p{
            margin:6px 0 0;
            opacity:.92;
            font-size:.95rem;
        }

        .server-chip{
            background:rgba(255,255,255,.18);
            backdrop-filter: blur(8px);
            border:1px solid rgba(255,255,255,.2);
            padding:12px 18px;
            border-radius:999px;
            font-weight:700;
        }

        .wrapper{
            max-width:1200px;
            margin:28px auto 50px;
            padding:0 18px;
        }

        .hero{
            background: linear-gradient(135deg, #ff335f 0%, #ff6584 100%);
            border-radius:30px;
            padding:36px;
            color:white;
            display:grid;
            grid-template-columns: 1.3fr .9fr;
            gap:24px;
            box-shadow: var(--shadow);
            margin-bottom:28px;
        }

        .hero h2{
            margin:0 0 10px;
            font-size:2.1rem;
            line-height:1.05;
        }

        .hero p{
            margin:0;
            opacity:.96;
            line-height:1.6;
            max-width:620px;
        }

        .hero-side{
            background:rgba(255,255,255,.14);
            border:1px solid rgba(255,255,255,.18);
            border-radius:24px;
            padding:22px;
            align-self:stretch;
        }

        .hero-side h3{
            margin:0 0 12px;
            font-size:1.1rem;
        }

        .hero-tags{
            display:flex;
            flex-wrap:wrap;
            gap:10px;
        }

        .tag{
            background:rgba(255,255,255,.16);
            padding:10px 14px;
            border-radius:999px;
            font-size:.9rem;
            font-weight:600;
        }

        .layout{
            display:grid;
            grid-template-columns: 1fr 1.2fr;
            gap:24px;
        }

        .stack{
            display:grid;
            gap:24px;
        }

        .card{
            background:var(--card);
            border-radius:var(--radius);
            padding:24px;
            box-shadow:var(--shadow);
        }

        .card h3{
            margin:0 0 8px;
            font-size:1.4rem;
        }

        .card .desc{
            margin:0 0 18px;
            color:var(--muted);
            line-height:1.55;
            font-size:.96rem;
        }

        .status-box{
            display:flex;
            justify-content:space-between;
            align-items:center;
            gap:16px;
            flex-wrap:wrap;
            margin-bottom:18px;
        }

        .status-meta{
            display:grid;
            gap:10px;
        }

        .meta{
            background:#f3f4f6;
            padding:12px 14px;
            border-radius:14px;
            color:#374151;
            font-size:.95rem;
        }

        .meta strong{
            color:#111827;
        }

        .status-pill{
            display:inline-flex;
            align-items:center;
            gap:10px;
            padding:12px 16px;
            border-radius:999px;
            font-weight:800;
            font-size:.95rem;
        }

        .dot{
            width:10px;
            height:10px;
            border-radius:50%;
            display:inline-block;
        }

        .online{
            background:rgba(25,180,107,.12);
            color:#13804d;
        }

        .online .dot{
            background:#19b46b;
        }

        .offline{
            background:rgba(239,68,68,.12);
            color:#b91c1c;
        }

        .offline .dot{
            background:#ef4444;
        }

        .button-row{
            display:flex;
            flex-wrap:wrap;
            gap:12px;
        }

        button{
            border:none;
            border-radius:16px;
            padding:14px 20px;
            font-size:.97rem;
            font-weight:700;
            cursor:pointer;
            transition:.18s ease;
        }

        button:hover{
            transform:translateY(-1px);
            filter:brightness(1.03);
        }

        .btn-primary{
            background:var(--primary);
            color:white;
        }

        .btn-primary:hover{
            background:var(--primary-dark);
        }

        .btn-green{
            background:var(--green);
            color:white;
        }

        .btn-red{
            background:var(--red);
            color:white;
        }

        .order-form{
            display:grid;
            gap:14px;
        }

        label{
            font-weight:700;
            color:#374151;
            font-size:.95rem;
        }

        input[type="text"]{
            width:100%;
            padding:16px 18px;
            border-radius:18px;
            border:1px solid #e5e7eb;
            font-size:1rem;
            outline:none;
            background:#fafafa;
        }

        input[type="text"]:focus{
            border-color:#ff335f;
            box-shadow:0 0 0 4px rgba(255,51,95,.12);
            background:white;
        }

        .quick-grid{
            display:grid;
            grid-template-columns:repeat(2,1fr);
            gap:14px;
        }

        .quick-btn{
            background:#fff5f7;
            border:1px solid #ffd6e0;
            color:#b42345;
            border-radius:18px;
            padding:16px;
            text-align:left;
            font-weight:700;
            cursor:pointer;
            transition:.18s ease;
        }

        .quick-btn span{
            display:block;
            color:#6b7280;
            font-weight:500;
            font-size:.88rem;
            margin-top:6px;
        }

        .quick-btn:hover{
            background:#ffe8ee;
            transform:translateY(-1px);
        }

        .response{
            padding:16px 18px;
            border-radius:18px;
            white-space:pre-wrap;
            line-height:1.6;
            font-family:Consolas, monospace;
            font-size:.94rem;
        }

        .response.success{
            background:#ecfdf5;
            color:#065f46;
            border-left:5px solid #10b981;
        }

        .response.error{
            background:#fef2f2;
            color:#991b1b;
            border-left:5px solid #ef4444;
        }

        .response.info{
            background:#eff6ff;
            color:#1e3a8a;
            border-left:5px solid #3b82f6;
        }

        .info-list{
            display:grid;
            gap:12px;
        }

        .info-item{
            background:#fafafa;
            border:1px solid #f0f0f0;
            padding:14px 16px;
            border-radius:16px;
            line-height:1.55;
            color:#4b5563;
        }

        .info-item strong{
            color:#111827;
        }

        .footer{
            text-align:center;
            color:#6b7280;
            margin-top:26px;
            font-size:.95rem;
        }

        @media (max-width: 980px){
            .hero{
                grid-template-columns:1fr;
            }

            .layout{
                grid-template-columns:1fr;
            }
        }

        @media (max-width: 640px){
            .wrapper{
                padding:0 12px;
            }

            .hero{
                padding:24px;
            }

            .quick-grid{
                grid-template-columns:1fr;
            }

            .button-row{
                flex-direction:column;
            }

            .button-row button{
                width:100%;
            }
        }
    </style>

    <script>
        function setOrder(value) {
            const input = document.getElementById('order');
            input.value = value;
            input.focus();
        }
    </script>
</head>
<body>
    <header class="topbar">
        <div class="topbar-inner">
            <div class="brand">
                <h1>SprintBite OS</h1>
                <p>Tu panel de pedidos estilo app de delivery</p>
            </div>
            <div class="server-chip">
                Puerto TCP 5000
            </div>
        </div>
    </header>

    <main class="wrapper">
        <section class="hero">
            <div>
                <h2>Pide, controla y monitorea tu servidor en una sola interfaz</h2>
                <p>
                    Esta interfaz conecta PHP con tu servidor TCP en C para encenderlo, apagarlo
                    y enviar pedidos de forma visual, rápida y mucho más amigable.
                </p>
            </div>

            <div class="hero-side">
                <h3>Funciones principales</h3>
                <div class="hero-tags">
                    <div class="tag">Encender servidor</div>
                    <div class="tag">Enviar pedidos</div>
                    <div class="tag">Apagar servidor</div>
                    <div class="tag">Pedidos rápidos</div>
                </div>
            </div>
        </section>

        <div class="layout">
            <div class="stack">
                <section class="card">
                    <h3>Estado del servidor</h3>
                    <p class="desc">Panel de control principal del módulo cliente-servidor.</p>

                    <div class="status-box">
                        <div class="status-meta">
                            <div class="meta">📡 Puerto: <strong>5000</strong></div>
                            <div class="meta">📝 Log: <strong>logs/server_gui.log</strong></div>
                        </div>

                        <div class="status-pill <?php echo $statusClass; ?>">
                            <span class="dot"></span>
                            Servidor <?php echo $status; ?>
                        </div>
                    </div>

                    <form method="POST">
                        <div class="button-row">
                            <button type="submit" name="action" value="start" class="btn-green">
                                Encender servidor
                            </button>
                            <button type="submit" name="action" value="stop" class="btn-red">
                                Apagar servidor
                            </button>
                        </div>
                    </form>
                </section>

                <?php if ($response !== ""): ?>
                <section class="card">
                    <h3>Respuesta del sistema</h3>
                    <p class="desc">Resultado de la última acción realizada.</p>
                    <div class="response <?php echo $responseClass; ?>">
<?php echo htmlspecialchars($response); ?>
                    </div>
                </section>
                <?php endif; ?>

                <section class="card">
                    <h3>Cómo funciona</h3>
                    <p class="desc">Resumen rápido del comportamiento de la interfaz.</p>

                    <div class="info-list">
                        <div class="info-item">
                            <strong>Encender servidor:</strong> inicia el servidor TCP en C desde la interfaz.
                        </div>
                        <div class="info-item">
                            <strong>Enviar pedido:</strong> PHP abre un socket TCP hacia <strong>127.0.0.1:5000</strong>.
                        </div>
                        <div class="info-item">
                            <strong>Apagar servidor:</strong> se envía el mensaje especial <strong>exit</strong>.
                        </div>
                        <div class="info-item">
                            <strong>Log:</strong> la salida del servidor iniciado desde GUI se guarda en <strong>logs/server_gui.log</strong>.
                        </div>
                    </div>
                </section>
            </div>

            <div class="stack">
                <section class="card">
                    <h3>Enviar pedido</h3>
                    <p class="desc">Escribe el pedido manualmente y envíalo al servidor.</p>

                    <form method="POST" class="order-form">
                        <label for="order">Pedido del cliente</label>
                        <input
                            type="text"
                            id="order"
                            name="order"
                            placeholder="Ejemplo: Pizza pepperoni"
                            value="<?php echo htmlspecialchars($order); ?>"
                        >

                        <div class="button-row">
                            <button type="submit" name="action" value="send" class="btn-primary">
                                Enviar pedido
                            </button>
                        </div>
                    </form>
                </section>

                <section class="card">
                    <h3>Pedidos rápidos</h3>
                    <p class="desc">Selecciona uno para llenar automáticamente el campo del pedido.</p>

                    <div class="quick-grid">
                        <button type="button" class="quick-btn" onclick="setOrder('Pizza pepperoni')">
                            Pizza pepperoni
                            <span>Clásica y rápida</span>
                        </button>

                        <button type="button" class="quick-btn" onclick="setOrder('Hamburguesa doble')">
                            Hamburguesa doble
                            <span>Extra carne</span>
                        </button>

                        <button type="button" class="quick-btn" onclick="setOrder('Tacos al pastor')">
                            Tacos al pastor
                            <span>Favorito del día</span>
                        </button>

                        <button type="button" class="quick-btn" onclick="setOrder('Sushi roll')">
                            Sushi roll
                            <span>Opción fresca</span>
                        </button>

                        <button type="button" class="quick-btn" onclick="setOrder('Orden premium: ramen')">
                            Orden premium: ramen
                            <span>Pedido premium</span>
                        </button>

                        <button type="button" class="quick-btn" onclick="setOrder('Orden urgente: burrito')">
                            Orden urgente: burrito
                            <span>Entrega urgente</span>
                        </button>

                        <button type="button" class="quick-btn" onclick="setOrder('Combo familiar')">
                            Combo familiar
                            <span>Ideal para compartir</span>
                        </button>

                        <button type="button" class="quick-btn" onclick="setOrder('Ensalada vegetariana')">
                            Ensalada vegetariana
                            <span>Opción ligera</span>
                        </button>
                    </div>
                </section>
            </div>
        </div>

        <div class="footer">
            SprintBite OS | PHP GUI + TCP Server en C
        </div>
    </main>
</body>
</html>
