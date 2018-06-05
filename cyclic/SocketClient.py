from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import requests
import json
import socket, time

class UploadFileToServer(FileSystemEventHandler):
    ''' Run when file is created in directory '''
    def on_created(self, event):
        print "Got event for file %s" % event.src_path
        with open(event.src_path, 'rb') as f:
       		file_name = f.readline().rstrip()
       		file_content = f.readline()
       		json_data = {"graphName": file_name, "graphString": file_content}
       		header = {'Content-type': 'application/json'}
       		print(json_data)
       		r = requests.post('http://13.57.17.226:8080/uploadGraph', data = json.dumps(json_data), headers = header)
       		print(r)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('localhost', 2049))
#print client.send('Hello world!'), 'bytes sent.'
print client.send('0101010101010101010101010101010101010101010101010101010101010101010101'), 'bytes sent.'
time.sleep(0.2)
print 'Received message:', client.recv(1024)

'''observer = Observer()
event_handler = UploadFileToServer() # create event handler
# set observer to use created handler in directory
observer.schedule(event_handler, path='/home/jzheng/Classes/cs293b/search/cyclic/test_dir')
observer.start()

# sleep until keyboard interrupt, then stop + rejoin the observer
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    observer.stop()

observer.join()'''