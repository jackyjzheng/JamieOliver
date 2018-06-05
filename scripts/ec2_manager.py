import boto3
import time
import socket
from subprocess import call


ec2 = boto3.resource('ec2', region_name="us-east-1")

# For the web server
print('Creating web server instance')
instance = ec2.create_instances(
    ImageId='ami-a4dc46db',
    MinCount=1,
    MaxCount=1,
    SecurityGroupIds=[
    	'sg-07aea7b9c198d6fbc'
    ],
    KeyName='test',
    InstanceType='t2.micro',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'server'
	    		}
    		]
    	}
  	])

currInstance = instance[0]
print('Waiting for completion')
currInstance.wait_until_running()
currInstance.load()
dnsName = currInstance.public_dns_name

time.sleep(20)
#cmd = 'scp -i /Users/dennisfong/test.pem /Users/dennisfong/Desktop/Projects/JamieOliver/rest-server/target/gs-rest-service-0.1.0.jar ' + 'ubuntu@' + dnsName + ':~'
cmd = 'scp -i /Users/dennisfong/test.pem /Users/dennisfong/test.txt ' + 'ubuntu@' + dnsName + ':~'

call(cmd.split())

# Worker 1
print('Creating worker instance')
instance = ec2.create_instances(
    ImageId='ami-a4dc46db',
    MinCount=1,
    MaxCount=1,
    SecurityGroupIds=[
    	'sg-07aea7b9c198d6fbc'
    ],
    KeyName='test',
    InstanceType='t2.micro',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker1'
	    		}
    		]
    	}
  	])

currInstance = instance[0]
print('Waiting for completion')
currInstance.wait_until_running()
currInstance.load()
dnsName = currInstance.public_dns_name

time.sleep(20)
cmd = 'scp -i /Users/dennisfong/test.pem /Users/dennisfong/test.txt ' + 'ubuntu@' + dnsName + ':~'
call(cmd.split())

print('Creating image of client')
image = currInstance.create_image(
	Name='Ramsey-Client')
imageId = image.image_id
print('Waiting for image to exist')
while image.state == 'pending':
	print('Pending')
	time.sleep(5)
	image.load()
	if image.state == 'available':
		break
# print('Device mappings are' + str(image.block_device_mappings))
# snapshotId = image.block_device_mappings[0]['Ebs']['SnapshotId']
# snapshot = ec2.Snapshot(snapshotId)
# print('Waiting for snapshot ' + snapshotId + ' to complete')
# snapshot.wait_until_completed()

# Worker 2
print('Creating worker instances 2-10')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=9,
    KeyName='test',
    SecurityGroupIds=[
    	'sg-07aea7b9c198d6fbc'
    ],
    InstanceType='t2.micro',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker2'
	    		}
    		]
    	}
  	])

# # Worker 3
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 4
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 5
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 6
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 7
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 8
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 9
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')

# # Worker 10
# instance = ec2.create_instances(
#     ImageId='ami-a4dc46db',
#     MinCount=1,
#     MaxCount=1,
#     InstanceType='t2.micro')
