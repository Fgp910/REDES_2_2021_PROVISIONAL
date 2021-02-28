import threading
from socket import *

def conection():
	serverName = '127.0.0.1'
	serverPort = 8080
	clientSocket = socket(AF_INET, SOCK_STREAM)
	clientSocket.connect((serverName,serverPort))
	sentence = "Prueba"
	clientSocket.send(sentence.encode('UTF-8'))
	modifedSentence = clientSocket.recv(1024)
	#print ('Desde el servidor: {}'.format(modifedSentence.decode('UTF-8')))
	print ('Desde el servidor: {}'.format(modifedSentence.decode()))
	clientSocket.close()

for i in range(5000):
	x = threading.Thread(target=conection)
	x.start()