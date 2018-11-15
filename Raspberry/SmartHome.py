#Para quando a biblioteca "bluetooth" não for reconhecida(provavelmente porque
#as outras instalações foram para python2):
#
#   sudo apt-get install bluetooth libbluetooth-dev
#   sudo python3 -m pip install pybluez
#   
#Funcionou depois que executei esses comandos, mas antes deles estes foram os
#principais comandos que executei:
#   sudo apt-get update
#   sudo apt-get install bluetooth
#   sudo apt-get install bluez
#   sudo apt-get install python-bluez
#   sudo reboot


import bluetooth
import time
import csv
from datetime import datetime


# função para tratamento da string
# INPUT: string poluida com b e '
# OUTPUT: dois valores, sensorLDR e sensorMov, do tipo String
def tratamentoString(string):
    resul = ""
    # laço para remover os caracteres indesejados
    for i in range(string.__len__()):
        if (string[i]!="'" and string[i]!="b"):
            resul += string[i]
    # separar os dados em duas variáveis
    aux = resul.find(':')
    sensorLDR = resul[:aux]
    sensorMov = resul[aux+1:]
    return sensorLDR, sensorMov




# Configurações do Bluetooth
addrHC05 = "20:16:10:17:34:35"
port = 1
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((addrHC05,port))
data = ""


#Configurações para abrir e escrever no arquivo        
colunas = ["sensorMov", "hora", "dia", "dia_semana", "sensorLDR", "lamp"]
with open("/home/pi/Documents/Juliana/Arquivo.csv", "w") as saida:
    
    escrever = csv.writer(saida)
    escrever.writerow(colunas)
    print("done")
    while True:
        #pega os dados do bluetooth
        data += str(sock.recv(1024))
        data_end = data.find(".")
        if data_end != -1 :
                rec = data[:data_end]
                sensorLDR, sensorMov = tratamentoString(rec)
                
                #Armazena o dia (dd-mm-aa)
                date_now = datetime.now().strftime('%d-%m-%y')

                #Armazena a hora (hh:mm:ss)
                hora_now = datetime.now().strftime('%T')

                #Armazena o dia da semana [dom=0, ..., sab=6]
                week_day = datetime.now().isoweekday()

                escrever.writerow([sensorMov, hora_now, date_now, week_day, sensorLDR, "lamp" ])
                print(sensorMov + "  ;  " + hora_now + "  ;  " + date_now + "  ;  " + str(week_day) +  "  ;  " + sensorLDR + "  ;  lamp")





        # OLD
        #Atualiza os dados recebidos do Arduino
        data = data[data_end+1:]
        #if data:
            #ldr = str(sock.recv(4096))
        
            #Armazena o dia (dd-mm-aa)
            #date_now = datetime.now().strftime('%d-%m-%y')
        
            #Armazena a hora (hh:mm:ss)
            #hora_now = datetime.now().strftime('%T')
        
            #Armazena o dia da semana [dom=0, ..., sab=6]
            #week_day = datetime.now().isoweekday()
            
            #escrever.writerow([data, hora_now, date_now, week_day, ldr, "lamp" ])
            #print(data + "  ;  " + hora_now + "  ;  " + date_now + "  ;  " + str(week_day) +  "  ;  " + ldr + "  ;  lamp")
        
