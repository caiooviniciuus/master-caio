import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

print("------------------------------------")
print("UNIVERSIDADE FEDERAL DO ABC")
print("Mestrado em Engenharia da Informação")
print("Aluno: Caio Vinícius Ribeiro da Silva")
print("------------------------------------")

print("Base de Dados")
# Nome do Arquivo a ser importado
# nFile = input("Digite o nome do arquivo: ")
nFile = "2022-4-16"
print("Arquivo: " + nFile)

print("------------------------------------")
print("Horário para análise")

# Hora inicial e final para ser analisada
# hInicial = int(input("Digite a hora inicial: "))
# hFinal = int(input("Digite a hora final: "))
hInicial = "11:08"
hFinal = "11:11"

print("Hora inicial: " + hInicial + " - Hora final: " + hFinal)


# Dados brutos
dadosB = pd.read_csv(nFile + "\" + ".csv", sep=',')

# Conversão do horário para datetime
dadosB['datetime'] = pd.to_datetime(dadosB['DATA'])

dadosB['DIA'] = pd.to_datetime(dadosB['DATA']).dt.date
dadosB['t(HH:MM:ss)'] = pd.to_datetime(dadosB['DATA']).dt.time

# Datetime como index
dadosB = dadosB.set_index('datetime')

# Filtra os dados entre o horario desejado
dadosF = dadosB.between_time(hInicial,hFinal)



def gManual():

    # n = int(input("Qual a quantidade de variáveis no eixo Y"))
    #
    # if n == 1:
    #     y = input("Eixo Y: ")
    #     dadosF.plot('t(HH:MM:ss)', y)
    #
    # elif n == 2:
    #     a = input("Primeiro eixo Y")
    #     b = input("Segundo eixo Y")
    #     dadosF.plot('t(HH:MM:ss)', y=[a, b])
    #
    # elif n == 3:
    #     a = input("Primeiro eixo Y")
    #     b = input("Segundo eixo Y")
    #     c = input("Terceiro eixo Y")
    #     dadosF.plot('t(HH:MM:ss)', y=[a, b, c])
    #
    # tFig = input("Qual o título da figura? ")
    # eixoX = input("Qual o eixo x? ")
    # eixoY = input("Qual o eixo y? ")
    #
    # plt.title(tFig)
    # plt.ylabel(eixoX)
    # plt.xlabel(eixoY)
    #
    # plt.grid()
    # plt.show()
    #
    # nFile = input("Qual o nome do arquivo? ")
    # plt.savefig(nFile + ".eps", format='eps')

    # Abaixo, área de testes

    # dadosF.plot('t(HH:MM:ss)', y=["LUX", "N"])
    dadosF.plot('t(HH:MM:ss)', y=["N", "RSSI"])

    plt.grid()

    # plt.ylabel("LUX")
    plt.show()


gManual()


# criar graficos automáticos para cada dia e teste
