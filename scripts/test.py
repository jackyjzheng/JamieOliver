import socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    s.connect(('ubuntu@ec2-54-89-221-31.compute-1.amazonaws.com', 22))
    print "Port 22 reachable"
except socket.error as e:
    print "Error on connect: %s" % e
s.close()