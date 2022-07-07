import pandas as pd
import numpy as np
import matplotlib.pyplot as plt



def coleta_dados(file):
    print("------------------------------------")
    print("UNIVERSIDADE FEDERAL DO ABC")
    print("Mestrado em Engenharia da Informação")
    print("Aluno: Caio Vinícius Ribeiro da Silva")
    print("------------------------------------")

    print("Base de Dados")

    print("Caminho do arquivo: " + file + "/" + file + ".csv")

    # Dados brutos
    dadosBrutos = pd.read_csv(path + ".csv", sep=',')

    # Conversão do horário para datetime
    dadosBrutos['datetime'] = pd.to_datetime(dadosBrutos['DATA'])
    dadosBrutos['DIA'] = pd.to_datetime(dadosBrutos['DATA']).dt.date
    dadosBrutos['t(s)'] = pd.to_datetime(dadosBrutos['DATA']).dt.time

    # Datetime como index
    dadosBrutos = dadosBrutos.set_index('datetime')

    # Renomeando colunas
    # dadosBrutos = dadosBrutos.rename(columns={"ID1(mA)": "ID1(A)"})

    # Convertendo float64
    # dadosBrutos['N'] = dadosBrutos['N'].astype(np.int64)
    # dadosBrutos['ID'] = dadosBrutos['ID'].astype(np.int64)


    # Mudando escala
    # dadosBrutos["ID1"] = dadosBrutos["ID1"]/1000

    return dadosBrutos

def instrucoes_graficos():
    print("Para imprimir graficos, chamar g1 ou g2 ou g3 e enviar dados, hi (string), hf(string), y (string), s (salvar, 1 sim; 0 não)")

# imprime gráfico x,y
def g1(dados, y1, s):
    dados.plot('t(s)', y1)

    # salvar figura
    if s == 1:
        name = input("Qual o nome do arquivo? ")
        plt.savefig(path + "-" + name + ".eps", format='eps')

    plt.show()

    # instruções para imprimir graficos
    dados.info()
    instrucoes_graficos()


# imprime gráfico x,y1,y2
def g2(dados, y1, y2, s):
    dados.plot('t(s)', y=[y1, y2])
    plt.show()

    # salvar figura
    if s == 1:
        name = input("Qual o nome do arquivo? ")
        plt.savefig(path + "-" + name + ".eps", format='eps')

    # instruções para imprimir graficos
    dados.info()
    instrucoes_graficos()

# imprime gráfico x,y1,y2,y3
def g3(dados, y1, y2, y3, s):
    dados.plot('t(s)', y=[y1,y2,y3])
    plt.show()

    # salvar figura
    if s == 1:
        name = input("Qual o nome do arquivo? ")
        plt.savefig(path + "-" + name + ".eps", format='eps')

    # instruções para imprimir graficos
    dados.info()
    instrucoes_graficos()


# imprime gráfico x,y1,y2,y3
def g4(dados, y1, y2, y3, y4, s):
    dados.plot('t(s)', y=[y1,y2,y3,y4])
    plt.show()

    # salvar figura
    if s == 1:
        name = input("Qual o nome do arquivo? ")
        plt.savefig(path + "-" + name + ".eps", format='eps')

    # instruções para imprimir graficos
    dados.info()
    instrucoes_graficos()

def filtro_dados(dados, hI, hF):
    # filtra pelo horário selecionado
    dadosFiltrados = dados.between_time(hI, hF)

    # dadosFiltrados["COMUNICAÇÃO"] = dadosFiltrados["N"]
    #
    # dadosFiltrados['COMUNICAÇÃO'].loc[dadosFiltrados['COMUNICAÇÃO'] >= 1] = 1

    dadosFiltrados.info()
    instrucoes_graficos()
    return dadosFiltrados



# nome para as imagens
global path

# mudar sempre para o arquivo a ser analisado
file = "2022-7-3"
path = file + "/" + file
dados_brutos = coleta_dados(file)

# 2022-07-03 15:26:00 to 2022-07-03 15:31:00
# 20m powerbank
# dados = filtro_dados(dados=coleta_dados(file), hI="15:26", hF="15:31")
# g1(dados, y1="RSSI", s=0)
# g2(dados, y1="C", y2="PER", s=0)


# ----------------------------------------------------------------------
# 2022-07-03 15:40:00 to 2022-07-03 15:45:00
# 30m powerbank
# dados = filtro_dados(dados=coleta_dados(file), hI="15:40", hF="15:45")
# g1(dados, y1="PER", s=0)
# g2(dados, y1="C", y2="PER", s=0)



# ----------------------------------------------------------------------
# 2022-07-03 15:47:00 to 2022-07-03 15:53:00
# 40m powerbank
# dados = filtro_dados(dados=coleta_dados(file), hI="15:47", hF="15:53")
# g1(dados, y1="PER", s=0)
# g2(dados, y1="C", y2="PER", s=0)


# ----------------------------------------------------------------------
# 2022-07-03 15:47:00 to 2022-07-03 15:53:00
# 50m powerbank
# 15:55 - Perdeu
# recomencando
# 15:58 - 16:02

dados = filtro_dados(dados=coleta_dados(file), hI="15:58", hF="16:03")
# g1(dados, y1="PER", s=0)
# g2(dados, y1="C", y2="PER", s=0)







# 2022-07-03
    # 20m powerbank
        # 15:26 - 15h30
    # 30m powerbank
        # 15h40 - 15:44
    # 40m powerbank
        # 15h47 - 15h52
    # 50m powerbank
        # 15h55 - Perdeu
        # recomencando
        # 15h58 - 16h02


# 2022-04-16 das 14:11:02 às 2022-04-16 15:49:23
    # momentos- chaves
    # '14:19' - marca de 200m
    # '14:24' - comecei a retornar ao ponto inicial, a partir os 200m
    # '14:36' - iniciei os 200m
    # '14:43' - iniciei os 200m

    # '14:48' - tentei ligar no kit solar, mas o D1 não ligou, verificar tensão, corrente e bateria
    # '14:55' - voltei para alimentação powerbank

    # '14:55' - iniciei os 200m


    # ----------- 1m/50m--------------
    # g1(dados, '15:07', '15:49', 'N')

    # nas gravações, começo fazendo o teste às '15:07'

    # '15:13' - iniciando 50m
    # '15:24' - Comecei a me movimentar a partir dos 50m para ver onde perde conexão
    # '15:27' - nos 80 m, com comunicação estável
    # '15:29' - perdeu conexão aos 80m da base
    # '15:30' - vou andar a partir dos 80m para ver em que ponto perde conexão com o NodeMCU. O D1 ainda não voltou
    # '15:38' - o NODEMCU começou a perder comunicação apos 73m (somando com os outros 80m)
    # '15:40' - retornando para a base, D1 e NodeMCU reconectaram
    # '15:49' - fim dos testes

    # chamando métodos

    # período completo
    # g1(hI="14:11", hF="15:50", dados=dados, y1="lux")
    # g1(hI="14:11", hF="15:50", dados=dados, y1="VD1(V)")
    # g1(hI="14:11", hF="15:50", dados=dados, y1="ID1(A)")
    # g1(hI="14:11", hF="15:50", dados=dados, y1="N")
    # g1(hI="14:11", hF="15:50", dados=dados, y1="COMUNICAÇÃO")
    # g1(hI="14:11", hF="15:50", dados = dados, y1 = "VBAT(V)")

    # d=50m
    # g1(hI="15:00", hF="15:50", dados=dados, y1="RSSI")
    # g2(hI="15:00", hF="15:50", dados=dados, y1="RSSI", y2="ID")
    # g3(hI="15:00", hF="15:50", dados=dados, y1="RSSI", y2="ID", y3="N")

    # solar
    # g4(hI="14:48", hF="15:00", dados=dados, y1="VD1(V)", y2="ID1(A)", y3="N", y4="VBAT(V)")
    # g4(hI="14:48", hF="15:00", dados=dados, y1="VD1(V)", y2="ID1(A)", y3="COMUNICAÇÃO", y4="VBAT(V)")
