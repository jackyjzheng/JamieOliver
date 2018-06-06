import subprocess
import sys
from subprocess import call

advanceWorkers = ['18.232.91.179', '18.206.171.173']
incrementWorkers = ['34.235.152.153', '18.204.245.243', '34.228.196.116', '52.23.250.133', '54.208.204.102', '54.82.246.180', '34.201.6.179', '18.205.7.217']

for ip in advanceWorkers:
	print(ip)
	cmd = 'ssh -i /Users/dennisfong/test.pem ' + 'ubuntu@' + ip
	call(cmd.split())


for ip in incrementWorkers:
	cmd = 'ssh -i /Users/dennisfong/test.pem ' + 'ubuntu@' + ip
	call(cmd.split())