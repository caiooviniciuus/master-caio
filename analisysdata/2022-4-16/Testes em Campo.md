# Testes em campo


## Teste 01 - 2022-04-16 15:00:00 to 2022-04-16 15:49:23

- Local: Parque Villa Lobos
- Data: 2022-04-16
- Se o kit solar não alimentar o circuito, fazer os testes com alimentação via powerbank e registrar, assim tenho material para o artigo

---

### Medições  

- colocar horário do início e fim da medição

### Powerbank, 1,5m

- Verificar a hora que ligou
- 14:19 (marca de 200m)
- Verificar antes
- Altura 1,5M, powerbank, 200m (d)
- Comecei a retornar ao ponto inicial  as 14:24

### Powerbank 1M

- 14:36 iniciei os 200m
- 14:43 iniciei os 200m
- 14:55 iniciei os 200m

- Considerações: D1 perdeu comunicação, mas NodeMCU manteve
- Estranho, pq o D1 tem antena, o MCU não…
- Deixei o D1 em loop para conectar na rede, de modo a conseguir o RSSI. Se não fosse por isso, o D1 estaria de enviando sinal
- Voltou a se conectar as 15:05

- 15:13 iniciando 50m
- Cheguei nos 50 metros, manteve a comunicação
- Aguardando alguns minutos
- 15: 24 Comecei a me movimentar a partir dos 50m para ver onde perde conexão
- Ouvir áudio desse horário para verificar em que momento perde a conexão
- Andei mais 30m e a conexão esta estável
- 15:27 nos 80 m, com comunicação estável

- 15:29 perdeu conexão aos 80m da base
- NodeMCU ainda envia dados
- 15:30 vou andar a partir dos 80m para ver em que ponto perde conexão com o NodeMCU. O D1 ainda não voltou
- 15:38 o NODEMCU começou a perde comunicação apos 73m (somando com os outros 80m)
- 15:40 retornando para a base, D1 e NodeMCU reconectaram
- 15:49 fim dos testes

- Ouvir os áudios para ver o momento (N) e a hora que perdeu comunicação

### Solar

- 14:48 tentei ligar no kit solar, mas o D1 não ligou, verificar tensão, corrente e bateria
- 14:55 voltei para alimentação powerbank
- Verificar tensão da bateria nesses intervalos
- Verificar a medida lux, aparecia zerada no display, mas acho que ele estourou e por isso mostrava zero (apareceu mais de 17mil uma hora)
- Verificar o local das medições

### Relatório

- Solar: VD1: 1,57V, ID1: 142mA, VBAT: 0,96
- Funcionou bem com alimentação via powerbank
- Verificar a conexão do D1 com a central, se é necessário ter RSSI
- Provavelmente se tirar a conexão AP/STA, melhora o alcance do D1
- Prender powerbank com velcro para facilitar
