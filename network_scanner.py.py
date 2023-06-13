from scapy.all import ARP, Ether, srp
import pyperclip
import webbrowser  


target_ip = "192.168.18.63"
# IP Address for the destination
# creating ARP packet
arp = ARP(pdst=target_ip)
# create the Ether broadcast packet
# ff:ff:ff:ff:ff:ff MAC address indicates broadcasting
ether = Ether(dst="ff:ff:ff:ff:ff:ff")
# stack them
packet = ether/arp

result = srp(packet, timeout=1, verbose=0)[0]

# a list of clients, we will fill this in the upcoming loop
clients = []

for sent, received in result:
    # for each response, append ip and mac address to `clients` list
    clients.append({'ip': received.psrc, 'mac': received.hwsrc})

# print clients
print("Available devices in the network:")
print("IP" + " "*18+"MAC")
for client in clients:
    print("{:16}    {}".format(client['ip'], client['mac']))
for client in clients:
    print(client['mac'])
    #comaprison stuff
    if(client['mac']=="48:3f:da:87:ae:07"):
        pyperclip.copy(client['ip']) #copy to clip board
        address=client['ip']
        print('copied')
    else:
        print('not copied')

# nesting open url code in try-catch block
try:
    url= f'{address}'  
    # print(url)
    webbrowser.open_new_tab(url)
except:
    print("EXCEPTION: address is not defined")
else:
    print("URL opening: successful")