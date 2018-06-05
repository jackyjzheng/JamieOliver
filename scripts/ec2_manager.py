import boto3
import time
import socket
from subprocess import call


ec2 = boto3.resource('ec2', region_name="us-east-1")
securityGroupId = 'sg-07aea7b9c198d6fbc'
keyName = 'test'

# For the web server
print('Creating web server instance')
instance = ec2.create_instances(
    ImageId='ami-a4dc46db',
    MinCount=1,
    MaxCount=1,
    SecurityGroupIds=[
    	securityGroupId
    ],
    KeyName=keyName,
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
print('Creating worker 1')
instance = ec2.create_instances(
    ImageId='ami-a4dc46db',
    MinCount=1,
    MaxCount=1,
    SecurityGroupIds=[
    	securityGroupId
    ],
    KeyName=keyName,
    InstanceType='c5.large',
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
print('Creating worker 2')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c5.xlarge',
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

# Worker 3
print('Creating worker 3')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c5.2xlarge',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker3'
	    		}
    		]
    	}
  	])

# Worker 4
print('Creating worker 4')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c5.4xlarge',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker4'
	    		}
    		]
    	}
  	])

# Worker 5
print('Creating worker 5')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c5.9xlarge',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker5'
	    		}
    		]
    	}
  	])

# Worker 6
print('Creating worker 6')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c4.large',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker6'
	    		}
    		]
    	}
  	])

# Worker 7
print('Creating worker 7')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c4.2xlarge',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker7'
	    		}
    		]
    	}
  	])

# Worker 8
print('Creating worker 8')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c4.4xlarge',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker8'
	    		}
    		]
    	}
  	])

# Worker 9
print('Creating worker 9')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c4.8xlarge',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker9'
	    		}
    		]
    	}
  	])

# Worker 10
print('Creating worker 10')
instanceList = ec2.create_instances(
    ImageId=imageId,
    MinCount=1,
    MaxCount=1,
    KeyName=keyName,
    SecurityGroupIds=[
    	securityGroupId
    ],
    InstanceType='c5d.large',
    TagSpecifications=[
    	{
    		'ResourceType': 'instance',
    		'Tags': [
	    		{
	    			'Key': 'Name',
	    			'Value' : 'worker10'
	    		}
    		]
    	}
  	])