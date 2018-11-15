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

addrHC05 = "20:16:10:17:34:35"
port = 1
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((addrHC05,port))
data = ""

while True:
    # O caractere "."(ponto) significa o fim de uma mensagem.
    data += str(sock.recv(1024))
    data_end = data.find(".")
    if data_end != -1 :
        rec = data[:data_end]
        print("Mensagem:" + rec )
        #print(rec + "\n")
        #data = data[data_end+1:]
    data = data[data_end+1:]
