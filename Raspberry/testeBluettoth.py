import bluetooth

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


addrHC05 = "20:16:10:17:34:35"
port = 1
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((addrHC05,port))
data = ""

while True:
    data += str(sock.recv(1024))
    data_end = data.find(".")
    if data_end != -1 :
        rec = data[:data_end]
        sensorLDR, sensorMov = tratamentoString(rec)
        print("LDR: "+sensorLDR + "\n" + "Mov: " + sensorMov + "\n" )
        #print(rec + "\n")
        #data = data[data_end+1:]
    data = data[data_end+1:]
