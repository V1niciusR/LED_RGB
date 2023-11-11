#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "GJPSISTEM";
const char *password = "1256789612567896GJPR";

const int pinRed = D3;
const int pinGreen = D2;
const int pinBlue = D1;

ESP8266WebServer server(80);

unsigned long lastUpdateTime = 0;
unsigned long transitionDuration = 5000; // Duração da transição em milissegundos
int targetRed = 255;
int targetGreen = 255;
int targetBlue = 255;
int currentRed = 255;
int currentGreen = 255;
int currentBlue = 255;

void handleSetColor();
void handleToggleColor();
void updateColor();
void setColor(String color);
void updateChannel(int &current, int target);

void setup()
{
    Serial.begin(115200);

    pinMode(pinRed, OUTPUT);
    pinMode(pinGreen, OUTPUT);
    pinMode(pinBlue, OUTPUT);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    server.on("/", HTTP_GET, handleRoot);
    server.on("/setColor", HTTP_POST, handleSetColor);
    server.on("/toggleColor", HTTP_POST, handleToggleColor);

    server.begin();
}

void loop()
{
    server.handleClient();

    // Atualizar a cor suavemente
    updateColor();
}

void handleRoot()
{
    String html = R"(
    <html>
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Projeto de Redes - Luzes no Sono</title>
        <link rel="icon" type="image/x-icon" href="./img/lampada.png">
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0/dist/css/bootstrap.min.css">
        <link href="https://cdn.jsdelivr.net/npm/remixicon@2.5.0/fonts/remixicon.css" rel="stylesheet">
        <link rel="stylesheet" href="https://unpkg.com/aos@next/dist/aos.css" />
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/@fancyapps/ui@4.0/dist/fancybox.css">
        <link rel="stylesheet" href="./style.css">
        <style>
          :root {
    --c-brand: #008080;
    --c-brand-rgb: 0, 128, 128;
    --c-dark: #303050;
    --c-body: #636383;
    --c-light: #f6f6f6;
    --f-main: "Bai Jamjuree", sans-serif;
    --transition: all .4s cubic-bezier(.165, .84, .44, 1);
    --shadow: 0px 15px 40px rgba(0, 0, 0, 0.1);
}

/* Reset & Helpers */
body {
    font-family: var(--f-main);
    color: var(--c-body);
    line-height: 1.7;
}

h1,
.h1,
h2,
.h2,
h3,
.h3,
h4,
.h4,
h5,
.h5,
h6,
.h6 {
    font-weight: 700;
    color: var(--c-dark);
}

a {
    text-decoration: none;
    color: var(--c-dark);
    transition: var(--transition);
}

a:hover {
    color: var(--c-brand);
}

img {
    width: 100%;
}


.section-padding {
    padding-top: 120px;
    padding-bottom: 120px;
}

.text-brand {
    color: var(--c-brand);
}

.setion-title {
    max-width: 560px;
    margin: 0 auto 60px auto;
}

.setion-title .line {
    width: 60px;
    height: 2px;
    background-color: var(--c-brand);
    display: inline-block;
}

.section-connector {
    position: relative;
}

.section-connector::after {
    content: '';
    width: 2px;
    height: 100px;
    background-color: var(--c-brand);
    position: absolute;
    top: -50px;
    left: 50%;
}

/* Resume */
.eduexp {
    position: relative;
}

.eduexp::before {
    content: '';
    height: 100%;
    width: 1px;
    background-color: rgba(var(--c-brand-rgb), 0.4);
    position: absolute;
    left: 0;
    top: 0;
}

.eduexp::after {
    content: '';
    width: 14px;
    height: 14px;
    background-color: var(--c-brand);
    position: absolute;
    left: -7px;
    top: 0;
}

.footer {
    margin-top: 15px;
}



.button-container {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
}

.button {
    position: relative;
    list-style: none;
    border: none;
    background-color: transparent;
    border-radius: 50%;
    cursor: grab;
    display: flex;
    flex-direction: column;
    align-items: center;
    text-align: center;
}

.button .svg {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    transition: all 0.3s;
}

.button .svg2 {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    fill: #f4af03;
    opacity: 0;
    transition: opacity 0.3s;
}

.button:hover .svg {
    fill: #f4af03;
}

.button:hover .svg2 {
    opacity: 1;
    fill: #f4af03;
}


/* Improved Styles for LED Control Section */
          #Simulador {
              margin-top: 60px; /* Adjust the margin-top to move it up */
          }

          .led-control-container {
              text-align: center;
              margin-top: 30px;
          }

          .led-control-title {
              font-size: 28px;
              color: var(--c-brand);
              margin-bottom: 20px;
          }

          .led-control-description {
              font-size: 16px;
              color: var(--c-dark);
              margin-bottom: 40px;
          }

          .led-control-form {
              display: flex;
              flex-direction: column;
              align-items: center;
          }

          .led-control-form input {
              margin-bottom: 15px;
              padding: 8px;
              border: 1px solid var(--c-brand);
              border-radius: 5px;
          }

          .led-control-form input[type="submit"] {
              background-color: var(--c-brand);
              color: #fff;
              cursor: pointer;
          }

          .led-control-form input[type="submit"]:hover {
              background-color: #006666; /* Darken the color on hover */
          }
        </style>
      </head>
      <body>


    <!-- Improved LED Control Section -->
        <section id="Simulador" class="section-padding section-connector">
            <div class="container">
                <div class="row">
                    <div class="col-12">
                        <div class="section-title text-center">
                            <h1>Controle do Led RGB</h1>
                            <p>Utilize abaixo esse botão caso queira testar nosso projeto da utilização de luzes RGB</p>
                            <div class="line"></div>
                        </div>
                    </div>
                </div>

                <div class="led-control-container">
                    <div class="led-control-description">Personalize a cor das luzes RGB para uma experiência única.</div>

                    <div class="led-control-form">
                        <form action='/setColor' method='post'>
                            Cor: <input type='color' name='color' value='#ffffff'><br><br>
                            <input type='submit' value='Definir Cor'>
                        </form>
                        <form action='/toggleColor' method='post'>
                            <input type='submit' value='Alternar'>
                        </form>
                    </div>
                </div>
            </div>
        </section>




    <!-- Tudo sobre as modalidades -->
    <section id="Modalidades" class="section-padding section-connector">
        <div class="container">
            <div class="row">
                <div class="col-12">
                    <div class="setion-title text-center">
                        <h1>Sobre o nosso projeto</h1>
                        <p>Abaixo temos uma breve explicação sobre o nosso projeto. Luz amarela e Luz azul, onde devemos
                            usá-las?</p>
                        <div class="line"></div>
                    </div>
                </div>
            </div>
            <div class="row gy-5">
                <div class="col-md-6">
                    <div class="row">
                        <div class="col-12">
                            <div class="eduexp p-5 pt-0">
                                <h5>Introdução</h5>
                                <small class="text-brand">Nosso projeto</small>
                                <p class="mb-0">A influência da luz na qualidade do sono e na produtividade é um tema de
                                    crescente interesse em pesquisas nas áreas de saúde, neurociência e psicologia. A
                                    cor da luz, especificamente, desempenha um papel crucial nesse contexto, com a luz
                                    amarela e a luz branca apresentando efeitos distintos na regulação do ciclo
                                    circadiano, no sono e na capacidade de produtividade.
                                </p>
                            </div>
                        </div>
                        <div class="col-12">
                            <div class="eduexp p-5 pt-0">
                                <h5>Luz Amarela e Qualidade do Sono</h5>
                                <small class="text-brand">Breve explicação</small>
                                <p class="mb-0">A luz amarela, muitas vezes associada ao pôr do sol, tem uma tonalidade
                                    mais quente e suave em comparação com a luz branca. Essa tonalidade está mais
                                    próxima das cores naturais da iluminação ambiente durante o crepúsculo e a noite. A
                                    exposição à luz amarela durante a noite é crucial para a regulação saudável do ritmo
                                    circadiano, que é o sistema interno de relógio biológico do corpo. A luz amarela tem
                                    uma menor proporção de luz azul, que é a faixa de comprimento de onda que mais
                                    suprime a produção de melatonina, o hormônio que regula o sono.

                                    <br><br>Ao evitar a exposição excessiva à luz branca, principalmente àquela rica em
                                    luz azul, à noite, o corpo é capaz de produzir melatonina de maneira adequada,
                                    facilitando a transição para o sono e melhorando sua qualidade. Isso é especialmente
                                    importante para evitar distúrbios do sono, como insônia, que podem prejudicar a
                                    saúde mental, a cognição e o bem-estar geral.
                                </p>
                            </div>
                        </div>
                        <div class="col-12">
                            <div class="eduexp p-5 pt-0">
                                <h5>Sono, Produtividade e Saúde Mental</h5>
                                <small class="text-brand">Breve explicação</small>
                                <p class="mb-0">A qualidade do sono está inextricavelmente ligada à produtividade e à
                                    saúde mental. O sono é essencial para o funcionamento cognitivo adequado, a memória,
                                    a tomada de decisões e a criatividade. Quando a qualidade do sono é comprometida, a
                                    produtividade no trabalho ou em outras atividades diárias pode sofrer
                                    significativamente. Pessoas privadas de sono muitas vezes apresentam dificuldade em
                                    manter o foco, aprender novas informações e realizar tarefas complexas. Além disso,
                                    a privação crônica do sono está associada a um maior risco de problemas de saúde
                                    mental, como ansiedade e depressão.</p>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="col-md-6">
                    <div class="row">
                        <div class="col-12">
                            <div class="eduexp p-5 pt-0">
                                <h5>Associação entre Qualidade do Sono e Produtividade/Saúde Mental</h5>
                                <small class="text-brand">Breve explicação</small>
                                <p class="mb-0">A conexão entre a qualidade do sono, produtividade e saúde mental é
                                    notável. Ao usar luz amarela à noite, é possível proteger o ritmo circadiano e a
                                    produção de melatonina, otimizando assim a qualidade do sono. Isso, por sua vez,
                                    resulta em melhorias na capacidade de concentração, aprendizado e resolução de
                                    problemas, o que impacta diretamente na produtividade. Além disso, a manutenção de
                                    um sono adequado reduz o risco de problemas de saúde mental, promovendo um maior
                                    bem-estar emocional.

                                </p>
                            </div>
                        </div>
                        <div class="col-12">
                            <div class="eduexp p-5 pt-0">
                                <h5>Conclusão</h5>
                                <small class="text-brand">Conclusão do nosso projeto</small>
                                <p class="mb-0">Em última análise, a escolha da iluminação, seja luz amarela ou branca,
                                    deve ser feita de acordo com o contexto e as necessidades individuais. No entanto,
                                    compreender os efeitos da luz na regulação do sono e na produtividade é fundamental
                                    para tomar decisões informadas sobre o ambiente de iluminação, especialmente à
                                    noite. Ao utilizar luz amarela durante a noite, é possível promover um sono
                                    saudável, melhorar a produtividade diurna e salvaguardar a saúde mental. Portanto,
                                    equilibrar o uso de luz amarela para a qualidade do sono e luz branca para a
                                    produtividade pode ser uma abordagem benéfica para alcançar um estilo de vida mais
                                    equilibrado e saudável.
                                </p>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </section>




    <footer class="mt-5 py-5">
        <div class="container">
            <div class="row justify-content-center">
                <div class="col-md-4 text-center">
                    <div class="footer-logo">
                        <img src="./img/47_Campos-Belos_Horizontal-Transparente.png" class="img-fluid" style="max-width: 250px; height: auto;">
                    </div>
                </div>
            </div>
            <div class="row justify-content-center mt-3">
                <div class="col-md-4 text-center">
                    <p class="mb-0">Copyright 2023 | Vinícius Raniery e Adiel Trani</p>
                </div>
            </div>
        </div>
    </footer>



    <div class="scroll-up-btn">
        <ion-icon name="arrow-up-circle-outline"></ion-icon>
    </div>



    <!-- Bootstrap JS-->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.0/dist/js/bootstrap.bundle.min.js"></script>

    <!-- Outros-->
    <script src="https://cdn.jsdelivr.net/npm/@fancyapps/ui@4.0/dist/fancybox.umd.js"></script>

    <!-- Icons e outros-->
    <script src="https://unpkg.com/ionicons@latest/dist/ionicons.js"></script>
    <script src="https://unpkg.com/aos@next/dist/aos.js"></script>

    <!--JS do código-->
</body>
    </html>
  )";

    server.send(200, "text/html", html);
}

void handleSetColor()
{
    String color = server.arg("color");
    setColor(color);
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleToggleColor()
{
    // Inverte as cores alvo entre branco e amarelo
    if (targetRed == 255 && targetGreen == 255 && targetBlue == 255)
    {
        targetRed = 255;
        targetGreen = 150;
        targetBlue = 0; // Amarelo
    }
    else
    {
        targetRed = 255;
        targetGreen = 255;
        targetBlue = 255; // Branco
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

void setColor(String color)
{
    long number = strtol(color.c_str() + 1, NULL, 16);
    targetRed = number >> 16;
    targetGreen = (number >> 8) & 0xFF;
    targetBlue = number & 0xFF;
}

void updateColor()
{
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= 20)
    { // Atualiza a cada 50 milissegundos
        lastUpdateTime = currentTime;

        // Atualiza suavemente cada canal de cor em direção ao alvo
        updateChannel(currentRed, targetRed);
        updateChannel(currentGreen, targetGreen);
        updateChannel(currentBlue, targetBlue);

        analogWrite(pinRed, currentRed);
        analogWrite(pinGreen, currentGreen);
        analogWrite(pinBlue, currentBlue);
    }
}

void updateChannel(int &current, int target)
{
    if (current < target)
    {
        current++;
    }
    else if (current > target)
    {
        current--;
    }
}