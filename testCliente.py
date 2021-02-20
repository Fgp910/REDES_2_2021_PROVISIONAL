from socket import *

serverName = '127.0.0.1'
serverPort = 8080
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverName,serverPort))
sentence = input('Introduce una frase: ')
clientSocket.send(sentence.encode('UTF-8'))
modifedSentence = clientSocket.recv(1024)
#print ('Desde el servidor: {}'.format(modifedSentence.decode('UTF-8')))
print ('Desde el servidor: {}'.format(modifedSentence.decode()))
clientSocket.close()
