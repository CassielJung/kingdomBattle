# echo-server.py

from random import randint
import random
import socket
import threading
import string
import time

HOST = "192.168.0.101"  # This should be set to the IP address of host computer
PORT = 3000  # Port to listen on (non-privileged ports are > 1023)
MAXROUNDS = 10 #number of rounds that should be played

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen() # listen incoming connections

#global variable

cards_Remain_list = []
ai_card_Remain_list = []
AIList_list = []

learning_list = []
learning_list_result = []

clients = [] # store clients that connected
nicknames = [] # store the nickname of choice for each client that connects
rooms = [] # Store tuples of clients to connect them together
roomsAI = []
waitingMM = [] # Store clients waiting for matchmaking

# Sends a message to every client
def broadcast(message):
    for client in clients:
        client.send(message)

# Creates room and "isolate" two players for an 1v1 battle
def createRooms(index1, index2):
    order = randint(0, 1)
    if(order):
        print(f'Creating room: {nicknames[index1]} vs {nicknames[index2]}')
        rounds = 0
        roundBuff = str(randint(0,2))
        player1Ult = str(randint(0,4))
        player2Ult = str(randint(0,4))
        rooms.append( [clients[index1], clients[index2], rounds, roundBuff])
        print(rooms[0][2])
        clients[index1].send(f'{player1Ult}{player2Ult}'.encode('ascii'))
        clients[index2].send(f'{player2Ult}{player1Ult}'.encode('ascii'))
        clients[index1].send(f'You will start the round{roundBuff}'.encode('ascii'))
    else:
        print(f'Creating room: {nicknames[index2]} vs {nicknames[index1]}')
        rounds = 0
        roundBuff = str(randint(0,2))
        player1Ult = str(randint(0,4))
        player2Ult = str(randint(0,4))
        rooms.append( [clients[index2], clients[index1], rounds, roundBuff] )
        print(rooms[0][2])
        clients[index1].send(f'{player1Ult}{player2Ult}'.encode('ascii'))
        clients[index2].send(f'{player2Ult}{player1Ult}'.encode('ascii'))
        clients[index2].send(f'You will start the round{roundBuff}'.encode('ascii'))

# Sends a message to your opponent
def chat(message, client):
    try:
        for tuples in rooms:
            #print(len(tuples))
            if client in tuples:
                if client == tuples[1]:
                    try:
                        if message.decode('ascii').find("gameEnded") != -1:
                            endGame(tuples[0], tuples[1])
                    except:
                        print("continue...")
                    tuples[0].send(message)
                    tuples[0].send(f'{tuples[3]}'.encode('ascii'))
                    tuples[2] += 1
                    break
                else:
                    try:
                        if message.decode('ascii').find("gameEnded") != -1:
                            endGame(tuples[0], tuples[1])
                    except:
                        print("continue...")
                    tuples[1].send(message)
                    tuples[1].send(f'{tuples[3]}'.encode('ascii'))
                    tuples[3] = str(randint(0,2))
                    break
    except:
        print("Exception in chat method")

# Function to send signals that the game should now end
#changed
def endGame(client1, client2):
    print("Rounds limit reached, closing room and clients")
    client1.close()
    client2.close()
    return 0

# Handle connections
def handle(client):
    while True:
        try:
            message = client.recv(1024) #1024 bytes
            chat(message, client)
        except: # remove client if it has left
            index = clients.index(client)
            nickname = nicknames[index]
            chat(f'{nickname} left the room'.encode('ascii'), client)
            for tuples in rooms:
                if client in tuples:
                    if client == tuples[0]:
                        if len(waitingMM) != 0:
                            index1 = clients.index(waitingMM[0])
                            waitingMM.remove(clients[index1])
                            index2 = clients.index(client)
                            createRooms(index1, index2)
                    else:
                        if len(waitingMM) != 0:
                            index1 = clients.index(waitingMM[0])
                            waitingMM.remove(clients[index1])
                            index2 = clients.index(client)
                            createRooms(index1, index2)
                    rooms.remove(tuples)
                    break
            clients.remove(client)
            client.close()
            print(f'{nickname} left the server'.encode('ascii'))
            nicknames.remove(nickname)
            break

# Accepting every connection
def receive():
    while True:
        client, address = server.accept()
        thread = threading.Thread(target=dispatch, args=(client,address))
        thread.start()    

def dispatch(client, address):
    print(f"Connected with {str(address)}")
    client.send('SINGLE'.encode('ascii'))
    string = client.recv(1024).decode('ascii')
    string = string[0]
    if string == "Y":
        # Run singleplayer mode
        print("Starting AI...")

        #changed
        client.send('NICK'.encode('ascii'))
        nickname = client.recv(1024).decode('ascii').rstrip("\x00") #Clean buffer sent from C file
        nickname = nickname[0:len(nickname) - 1] #Clean buffer sent from C file
        nicknames.append(nickname)
        clients.append(client)

        #changed for many access to AI
        cards_Remain_list.append([0,1,2,3,4,5,6,7,8,9,10,11,12])
        ai_card_Remain_list.append([0,1,2,3,4,5,6,7,8,9,10,11,12])
        AIList_list.append([0,0,0,0]) #shares index
        learning_list.append([])
        learning_list_result.append([])

        #create room for AI
        createRoomsAI(clients.index(client))

        print(f'Nickname is: {nickname}')

        handleAI(client)

    else:
        # add client to the list
        client.send('NICK'.encode('ascii'))
        nickname = client.recv(1024).decode('ascii').rstrip("\x00") #Clean buffer sent from C file
        nickname = nickname[0:len(nickname) - 1] #Clean buffer sent from C file
        nicknames.append(nickname)
        clients.append(client)
        if len(waitingMM) == 0:
            waitingMM.append(client)
        else:
            index1 = clients.index(waitingMM[0])
            waitingMM.remove(clients[index1])
            index2 = clients.index(client)
            createRooms(index1, index2)

        print(f'Nickname is: {nickname}')

        handle(client)

#changed
def createRoomsAI(index1):
    print(f'Creating room: {nicknames[index1]} vs AI')
    rounds = 0
    roundBuff = str(randint(0,2))
    player1Ult = str(randint(0,4))
    player2Ult = str(randint(0,4))
    ai_Ult = int(player2Ult)
    roomsAI.append( [clients[index1], rounds, roundBuff, ai_Ult])
    clients[index1].send(f'{player1Ult}{player2Ult}'.encode('ascii'))
    clients[index1].send(f'You will start the round{roundBuff}'.encode('ascii'))

#changed
# Handle connections
def handleAI(client):
    while True:
        try:
            message = client.recv(1024) #1024 bytes should be card information
            chatAI(message, client)
        except: # remove client if it has left
            index = clients.index(client)
            nickname = nicknames[index]
            for i, tuples in enumerate(roomsAI):
                if client in tuples:
                    if AIList_list[i][1] + AIList_list[i][0] > AIList_list[i][2] + AIList_list[i][3]: #AI wins
                        f = open("../AI/Generating_Database/cleaned_data.txt", "a")
                        for item in learning_list[i]:
                            f.write("%s\n" % item)
                        f.close()
                        f = open("../AI/Generating_Database/cleaned_data_result.txt", "a")
                        for item in learning_list_result[i]:
                            f.write("%s\n" % item)
                        f.close()
                    cards_Remain_list.pop(i)
                    ai_card_Remain_list.pop(i)
                    AIList_list.pop(i)
                    learning_list.pop(i)
                    roomsAI.remove(tuples)
                    break
            clients.remove(client)
            client.close()
            print(f'{nickname} left the server'.encode('ascii'))
            nicknames.remove(nickname)
            break

#changed
def chatAI(message, client): #player_array[myID]->ultimate, player_array[myID]->ultUseThisRound, myCard
    try:
        for i, tuples in enumerate(roomsAI):
            if client in tuples:
                stringMessage = message[0:25]
                stringMessage = str(stringMessage)
                try:
                    if stringMessage.find("gameEnded") != -1:
                        endGameAI(tuples[0])
                except:
                    print("continue...")

                # update status
                stringMessage = stringMessage[2:]
                if stringMessage.find("thestatus") != -1: # maybe in ?
                    stringMessage = stringMessage.split(' ')
                    for j in range(4):
                        AIList_list[i][j] = int(stringMessage[j])
                    break
                else: # the message that is not for status
                    stringMessage = stringMessage.split(' ')
                    if stringMessage[2][1] == '\\':
                        stringMessage[2] = stringMessage[2][0]
                    else:
                        stringMessage[2] = stringMessage[2][0:1]
                    client_ultimate = int(stringMessage[0])
                    client_ultuse = int(stringMessage[1]) 
                    client_card = int(stringMessage[2])
                    sleepTime = random.uniform(1.5,3.0)
                    time.sleep(sleepTime)

                    if client_card in cards_Remain_list[i]: # the cards get tidied
                        cards_Remain_list[i].remove(client_card)

                    # choose card here
                    card_i1 = randint(0, len(ai_card_Remain_list[i]) - 1)

                    while True:
                        card_i2 = randint(0, len(ai_card_Remain_list[i]) - 1)
                        if (card_i2 != card_i1):
                            break

                    while True:
                        card_i3 = randint(0, len(ai_card_Remain_list[i]) - 1)
                        if (card_i3 != card_i1 and card_i3 != card_i2):
                            break

                    ai_card_on_deck = []

                    ai_card_on_deck.append(ai_card_Remain_list[i][card_i1])
                    ai_card_on_deck.append(ai_card_Remain_list[i][card_i2])
                    ai_card_on_deck.append(ai_card_Remain_list[i][card_i3])

                    stat = str(AIList_list[i][0]) + ',' + str(AIList_list[i][1]) + ',' + str(AIList_list[i][2]) + ',' + str(AIList_list[i][3])

                    if AIList_list[i][0] <= 0:
                        endGameAI(tuples[0])

                    if AIList_list[i][2] <= 0:
                        endGameAI(tuples[0])

                    ai_card_chosen = int(output_function(int(tuples[2]), cards_Remain_list[i], stat, ai_card_on_deck, i)) #trigger AI in string (should in int???)

                    ai_card_Remain_list[i].remove(ai_card_chosen) # update the card_remain array

                    tuples[2] = str(randint(0,2)) #update roundbuff

                    ai_used = 0
                    # this can be more complicated, consider later
                    if tuples[3] == 4:
                        if AIList_list[i][3] <= 6:
                            ai_used = 0
                        elif randint(0,10) >= 7:
                            ai_used = 1
                        else:
                            ai_used = 0
                    elif tuples[3] != 0:
                        if randint(0,10) >= 4:
                            ai_used = 0
                        else:
                            ai_used = 1 #used the ult

                    tuples[0].send(f'{str(tuples[3])} {str(ai_used)} {str(ai_card_chosen)}'.encode('ascii')) #AI send information back
                    tuples[3] = 0 # changed after it is sent to the client? Important!
                    tuples[0].send(f'{str(tuples[2])}'.encode('ascii')) #update roundbuff
                    tuples[1] += 1
                    #Check round limit
                    if tuples[1] == MAXROUNDS:
                        endGameAI(tuples[0])
                    break
    except:
        print("Exception in chatAI method")

#changed
def endGameAI(client1):
    print("Rounds limit reached, closing room and clients")
    client1.close()
    return 0

#changed
def output_function(roundbuff, left_cards, stat, card_on_deck, index): #stat in string

    # read current status
    # suppose the file name is read_me.txt, it is kept overwrote
    # type is string

    result = 's' #initialize

    #2,[0,1,3,4,5,7,8,9,10,11,12],26,3,8,0

    #here leftcards
    user_action = str(roundbuff) + ',' + str(left_cards).replace(' ', '') + ',' + stat

    learning_list[index].append(user_action)

    for i, line in enumerate(cleaned_data):
        if line == user_action:
            if cleaned_data_result[i].split(',')[0] in card_on_deck:
                result = str(cleaned_data_result[i])
                break

    # other case... using threshold

    player_score = 0
    ai_score = 0

    if result == 's':
        user_input = stat.split(',') #okay we only wants last 4 and first 1
        user_input = user_input[0:1] + user_input[-4:]
        player_score = int(user_input[1]) + int(user_input[2]) * 0.6
        ai_score = int(user_input[3]) + int(user_input[4]) * 0.8

        if roundbuff == 2:
            player_score = player_score * 0.6
        elif roundbuff == 1:
            player_score = player_score * 1.25

        attack_card = []
        defense_card = []

        for item in card_on_deck:
            if item <= 6:
                attack_card.append(item)
                if ((item + 1) * 2 >= (int(user_input[3]) + int(user_input[4])) and roundbuff == 2) or (roundbuff != 2 and (item + 1) >= (int(user_input[3]) + int(user_input[4]))):
                    result = str(item)
                    break
            else:
                defense_card.append(item)

        attack_card.sort(reverse = True)
        defense_card.sort(reverse = True)

        if result == 's':
            if (len(attack_card) == 0 and len(defense_card) != 0) or user_input[0] == 1:
                result = str(defense_card[0])

            elif len(defense_card) == 0 or user_input[0] == 2:
                result = str(attack_card[0])

            elif ai_score > player_score:
                # we attack
                result = str(attack_card[0])

            else:
                result = str(defense_card[0])

    learning_list_result[index].append(result)

    return result

#changed also may need to edit the path
cleaned_data_result_onread = open("../AI/Generating_Database/cleaned_data_result.txt", "r")
cleaned_data_result = []
for i, line in enumerate(cleaned_data_result_onread):
    cleaned_data_result.append(line.strip())

cleaned_data_onread = open("../AI/Generating_Database/cleaned_data.txt", "r")
cleaned_data = []
for i, line in enumerate(cleaned_data_onread):
    cleaned_data.append(line.strip())

# Run the server
receive()