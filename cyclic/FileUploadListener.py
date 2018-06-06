from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import requests
import json
import socket, time

class UploadSearchFileToServer(FileSystemEventHandler):
    def on_created(self, event):
        print "Got event for file %s" % event.src_path
        with open(event.src_path, 'rb') as f:
       		file_name = f.readline().rstrip()
       		file_content = f.readline()
       		json_data = {"graphName": file_name, "graphString": file_content}
       		print(json_data)
       		header = {'Content-type': 'application/json'}
       		r = requests.post('http://34.201.106.13:8080/uploadGraph', data = json.dumps(json_data), headers = header)

class UploadIncrementFileToServer(FileSystemEventHandler):
    def on_created(self, event):
        print "Got event for file %s" % event.src_path
        with open(event.src_path, 'rb') as f:
       		file_name = f.readline().rstrip()
       		file_content = f.readline()
       		json_data = {"graphName": file_name, "graphString": file_content}
       		print(json_data)
       		header = {'Content-type': 'application/json'}
       		r = requests.post('http://34.201.106.13:8080/incrementGraph', data = json.dumps(json_data), headers = header)

observer = Observer()
event_handler_search = UploadSearchFileToServer() # create event handler
event_handler_increment = UploadIncrementFileToServer()
observer.schedule(event_handler_search, path='GeneratedGraphs/')
observer.schedule(event_handler_increment, path='IncrementedGraphs/')
observer.start()

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    observer.stop()

observer.join()
